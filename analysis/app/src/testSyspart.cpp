#include<iostream>
#include<vector>
#include<fstream>
using namespace std;
#include <chrono> 
using namespace std::chrono; 

#include <argp.h> 
#include "syspart.h"
#include "nonreturnAnalysis.h"
#include "conductor/interface.h"
#include "conductor/setup.h"
#include "pass/resolveplt.h"
#include "pass/collapseplt.h"

/* Required for parsing command line options */
char *filename=NULL;
char *func_name;
char *func_addr;
bool funcFlag = false;
bool isAddr = true;
bool typearmorFlag = false;
string typearmorPath;
bool icanalysisFlag = false;
bool log_flag = false;
bool direct_flag = false;
int option;
string option_args;
bool option_flag = false;

static int parse_opt (int key, char *arg, struct argp_state *state) 
{ 
    switch (key) 
    { 
        case 'p':  
                  filename = arg;
                  break; 
        case 's': if(arg[0] == '0' && arg[1] == 'x')
                    {
                        func_addr = arg;
                        
                    }
                  else
                   {
                        isAddr = false;
                        func_name = arg;
                       
                   }
                  funcFlag = true;
                  break; 
        case 't': typearmorFlag = true;
                  typearmorPath = arg;
                  
                  break;
        case 'i': icanalysisFlag = true;
                  
                  break;
        case 'l': log_flag = true;
                  
                  break;
        case 'g': direct_flag = true;
                  
                  break;
        case 'a' : option_flag = true;
        		   option_args = arg;
                   auto found = option_args.find(',');
                   if(found != string::npos)
                   {
                     option = stoi(option_args.substr(0,found+1),nullptr,10);
                   }
                   else
                   {
                     option = stoi(option_args,nullptr,10);
                   }
                   
    }
    return 0;
}

int main(int argc, char *argv[])
{
    auto start = high_resolution_clock::now(); 

    struct argp_option options[] = 
    { {0, 'p', "PROG", 0, "The binary program to be analyzed"}, 
      {0, 's', "STARTFN", 0, "Name or address of the root function of the function call graph (usually main). Address should start with 0x"},
      {0, 't', "PATH", 0 , "To enable typearmor. PATH refers to the path to the typearmor output file"},
      {0, 'i', 0, 0, "To enable indirect call target analysis"},
      {0, 'l', 0, 0 , "To enable logging"},
      {0, 'g', 0, 0, "To construct FCG with only direct edges. By default indirect edges are considered"},
      {0, 'a', "ANALYSIS",0, "The type of analysis to be performed. Pass the number corresponding to each analysis followed by a comma separated argument list. \n \
      1. Prints the callgraph (no args) \n \
      2. Print system calls filtered, given the address at which to partition and function containing the partition point (args : partition_point_address, func_name) \n \
      3. Prints the difference of system calls accessible from STARTFN and syscalls accessible from a specific function (args : func_name) \n \
      4. Prints the global AT list \n \
      5. Prints AICT (Average Indirect call target) \n \
      6. Prints the possible values of argument passed to a function (args : func_name, register_id) \n \
      7. Prints the system call info of all functions or of a specific function(args : func_name or * (for all fns)) \n  \
      8. Prints system calls filtered after including specified libraries (args : no_of_libs, lib1, lib2,...,libn) \n \
      9. Prints the callgraph with only indirect edges \n \
      10. If indirect call exists in any path or to trace the path of functions back to main (args : list of functions\n \
      11. If func2 is accessible from instruction with address addr in func1 (args : addr1, func1, func2) \n \
      12. Find system calls after including modules of dlsymed functions (args : output file from dlsym analysis) \n \
      13. Prints the no: of noreturn functions with and without our noreturn analysis \n \
      14. Prints the system calls which are directly invoked from AT functions \n \
      15. Prints if there is a path between from startfunc to endfun (args: startfunc, endfunc) \n \ 
      16. Print direct system calls of module (args : modulename) \n \
      17. Prints the callgraph of a module (args : modulename) \n \
      18. Prints the number of instructions in a function (args : functionname) \n \
      19. Prints if fork() and pthread() functions are invoked within the application"},
      { 0 } 
    }; 
    struct argp argp = { options, parse_opt }; 
    argp_parse (&argp, argc, argv, 0, 0, 0);
    if(filename == NULL)
    {
    	cout<<"Binary program should be provided"<<endl;
    	return -1;
    }
    if(!funcFlag)
    {
    	cout<<"Start(root) function name/address should be provided"<<endl;
    	return -1;
    }
    if(!option_flag)
    {
    	cout<<"Analysis option should be provided"<<endl;
    	return -1;
    }
    Syspart sp;
    Program *prog=NULL;

	EgalitoInterface egalito(/*verboseLogging=*/ log_flag, /*useLoggingEnvVar=*/ true);
	egalito.initializeParsing();
	egalito.parse(filename, true); //true to include shared lib
	sp.setConductorSetup(egalito.getSetup());
    sp.setProgram(egalito.getProgram());
    prog = egalito.getProgram();
    ResolvePLTPass resolvePLT(egalito.getConductor());
    prog->accept(&resolvePLT);

    CollapsePLTPass collapsePLT(egalito.getConductor());
    prog->accept(&collapsePLT);
    
	Function *start_func = NULL;
	if(!isAddr)
	{
		start_func = sp.findFunctionByName(func_name);
	}
	else
	{
		address_t address = (address_t)strtol(func_addr, NULL, 16); 
		start_func = sp.findFunctionByAddress(address);
	}
	sp.setStartFunc(start_func);
	if(typearmorFlag)
		sp.setTypeArmorPath(typearmorPath);
    switch(option)
    {
        case 1 : {
                    sp.run1(direct_flag, icanalysisFlag, typearmorFlag);                    
                    break;
                 }
        case 2 : {
                 auto found = option_args.find(',');
                 if(found == string::npos)
                 {
                    cout<<"Args(partition_point_address, func_name) required"<<endl;
                    break;
                 } 
                 auto pos = option_args.find(',',found+1);
                 if(pos == string::npos)
                 {
                    cout<<"Args(partition_point_address, func_name) required"<<endl;
                    break;
                 }
                 string part_addr = option_args.substr(found+1,pos-found-1);
                 if(part_addr.length() == 0)
                 {
                    cout<<"Args(partition_point_address, func_name) required"<<endl;
                    break;
                 }

                 string func_name = option_args.substr(pos+1);
                 if(func_name.length() == 0)
                 {
                    cout<<"Args(partition_point_address, func_name) required"<<endl;
                    break;
                 }
                 sp.syscallsOfMainLoop(icanalysisFlag, typearmorFlag, part_addr, func_name);
                 break; 
                 }
        case 3 : {
                 auto found = option_args.find(',');
                 if(found == string::npos)
                 {
                    cout<<"Args(func_name) required"<<endl;
                    break;
                 }
                 string func_name = option_args.substr(found+1);
        		 sp.run7(direct_flag, icanalysisFlag, typearmorFlag, func_name);
        		 break;
        		 }
        case 4 : {
                    sp.run5(direct_flag, icanalysisFlag, typearmorFlag);
                    break;
                 }
        case 5 : {
                    sp.printAICT(icanalysisFlag, typearmorFlag);
                    break;
                 }
        case 6 : {
                    auto found = option_args.find(',');
                    if(found == string::npos)
                    {
                        cout<<"Args(func_name, registerid) required"<<endl;
                        break;
                    } 
                    auto pos = option_args.find(',',found+1);
                    if(pos == string::npos)
                    {
                        cout<<"Args(func_name, registerid) required"<<endl;
                        break;
                    }
                    string func_name = option_args.substr(found+1,pos-found-1);
                    if(func_name.length() == 0)
                    {
                        cout<<"Args(func_name, registerid) required"<<endl;
                        break;
                    }
    
                    string reg = option_args.substr(pos+1);
                    if(reg.length() == 0)
                    {
                        cout<<"Args(func_name, registerid) required"<<endl;
                        break;
                    }
                    int regid = stoi(reg);
                    sp.getArgumentValue(icanalysisFlag, typearmorFlag, func_name, regid, filename);
                    break;
                 } 
        case 7 : {
                    auto found = option_args.find(',');
                    if(found == string::npos)
                    {
                        cout<<"Args(func_name) required"<<endl;
                        break;
                    }
                    string func_name = option_args.substr(found+1);
                    sp.run2(direct_flag, icanalysisFlag, typearmorFlag,func_name);
                    break;
                 }
        case 8 : {
                    auto found = option_args.find(',');
                    if(found == string::npos)
                    {
                        cout<<"Args(func_name) required"<<endl;
                        break;
                    }
                    string lib_path = option_args.substr(found+1);
                    std::ifstream infile(lib_path);
                    string lib,sym;
                    while(infile >> lib >> sym)
                    {
                        cout<<lib<<" "<<sym<<endl;

                    }
                    break;
                 }

        case 9 : {
                    sp.run12(direct_flag, icanalysisFlag, typearmorFlag);                    
                    break;
                 }
        case 10 : {
                    auto found = option_args.find(',');
                    if(found == string::npos)
                    {
                        cout<<"Args(func_name) required"<<endl;
                        break;
                    }
                    string func_name = option_args.substr(found+1);
                    vector<string> funcs;
                    funcs.push_back(func_name);
                    sp.run8(direct_flag, icanalysisFlag, typearmorFlag,funcs);
                    break;
                  }
        case 11 : {
                    auto found = option_args.find(',');             
                    if(found == string::npos)
                    {
                        cout<<"Args(addr, func1, func2) required"<<endl;

                    }
                    auto pos1 = option_args.find(',',found+1);          //End of addr
                    if(pos1 == string::npos)
                    {
                        cout<<"Args(addr, func1, func2) required"<<endl;
                        break;
                    }
                    string addr = option_args.substr(found+1,pos1-found-1);
                    if(addr.length() == 0)
                    {
                        cout<<"Args(addr, func1, func2) required"<<endl;
                        break;
                    }
                    auto pos2 = option_args.find(',',pos1+1);
                    if(pos2 == string::npos)
                    {
                        cout<<"Args(addr, func1, func2) required"<<endl;
                        break;
                    }
                    string func_name1 = option_args.substr(pos1+1,pos2-pos1-1);
                    if(func_name1.length() == 0)
                    {
                        cout<<"Args(addr, func1, func2) required"<<endl;
                        break;
                    }

                    string func_name2 = option_args.substr(pos2+1);
                    if(func_name2.length() == 0)
                    {
                        cout<<"Args(addr,func1, func2) required"<<endl;
                        break;
                    }
                    cout<<func_name1<<" "<<func_name2<<" "<<std::hex<<addr<<endl;
                    sp.isFunctionReachable(addr, func_name1, func_name2, icanalysisFlag, typearmorFlag);
                    break;
                  }
        case 12 : {

                    auto found = option_args.find(',');
                    if(found == string::npos)
                    {
                        cout<<"Args(filename) required"<<endl;
                        break;
                    }
                    string file_name = option_args.substr(found+1);
                    sp.getSyscallsFromDlsym(direct_flag, icanalysisFlag, typearmorFlag, file_name);
                    break;

                  }
        case 13 : {
                    auto before = sp.getNoReturnFnCount();
                    NonReturnAnalysis nr;
                    nr.run(egalito.getProgram());
                    auto after = sp.getNoReturnFnCount();
                    cout<<filename<<" Before: "<<before<<" After: "<<after<<endl;
                    break;
                  }
        case 14 : {
                    sp.run9(direct_flag, icanalysisFlag, typearmorFlag);
                    break;
                  }
        case 15 : {
                    auto found = option_args.find(',');
                    if(found == string::npos)
                    {
                        cout<<"Args(startfunc, endfunc) required"<<endl;
                        break;
                    } 
                    auto pos = option_args.find(',',found+1);
                    if(pos == string::npos)
                    {
                        cout<<"Args(startfunc, endfunc) required"<<endl;
                        break;
                    }
                    string start = option_args.substr(found+1,pos-found-1);
                    if(start.length() == 0)
                    {
                        cout<<"Args(startfunc, endfunc) required"<<endl;
                        break;
                    }
    
                    string end = option_args.substr(pos+1);
                    if(end.length() == 0)
                    {
                        cout<<"Args(startfunc, endfunc) required"<<endl;
                        break;
                    }
                    sp.ifPathExists(start, end, icanalysisFlag, typearmorFlag);
                    break;
                  }
        case 16 : {
                    auto found = option_args.find(',');
                    if(found == string::npos)
                    {
                        cout<<"Args(filename) required"<<endl;
                        break;
                    }
                    string s1 = option_args.substr(found+1);
                    cout<<s1<<endl;
                    Module* m = NULL;
                        for(auto module : CIter::children(prog))
                        {
                            string module_name = module->getName();
                            if(module_name.find(s1) != string::npos)
                            {
                                m = module;
                                //break;
                            }
                            cout<<module_name<<endl;
                        }
                        if(m == NULL)
                        {
                            cout<<"No module with the specified name found"<<endl;
                        }
                        else
                        {
                            cout<<"Module found "<<m->getName()<<endl;
                            sp.findDirectSyscallsOfModule(m);
                        }
                        break;
                  }
        case 17 : {
                    auto found = option_args.find(',');
                    if(found == string::npos)
                    {
                        cout<<"Args(filename) required"<<endl;
                        break;
                    }
                    string s1 = option_args.substr(found+1);
                    cout<<s1<<endl;
                    Module* m = NULL;
                        for(auto module : CIter::children(prog))
                        {
                            string module_name = module->getName();
                            if(module_name.find(s1) != string::npos)
                            {
                                m = module;
                                //break;
                            }
                            cout<<module_name<<endl;
                        }
                        if(m == NULL)
                        {
                            cout<<"No module with the specified name found"<<endl;
                        }
                        else
                        {
                            cout<<"Module found "<<m->getName()<<endl;
                            sp.findCallGraphOfModule(m);
                        }
                        break;
                  }
        case 18 : 
            {
                    auto found = option_args.find(',');
                    if(found == string::npos)
                    {
                        cout<<"Args(func_name) required"<<endl;
                        break;
                    }
                    string func_name = option_args.substr(found+1);
                    sp.getPartitionSize(direct_flag, icanalysisFlag, typearmorFlag,func_name);
                    break;
            }
        case 19 :
            {
                sp.run6(direct_flag, icanalysisFlag, typearmorFlag);                    
                break;
            }
        default : {
                    cout<<"\nInvalid option"<<endl;
                    break;
                  }
    }       

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(stop - start); 
    cout << "Time taken for the analysis: "<<std::dec<<duration.count() <<" seconds" << endl; 
    return 0;

}
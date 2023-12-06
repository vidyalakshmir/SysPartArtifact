//Pintool to count loops. Processes based on the output obtained from static analysis
#include<fstream>
#include<iostream>
#include<set>
#include<sstream>
#include<stack>
#include<vector>

#include "pin.H"

using std::hex;
using std::dec;
using std::ifstream;
using std::cout;
using std::set;
using std::istringstream;
using std::endl;
using std::cerr;
using std::string;
using std::size_t;
using std::map;
using std::ostream;
using std::stack;
using std::ofstream;
using std::vector;
using std::pair;

ifstream InFile;
ostream* OutFile=NULL;

KNOB<string> KnobInputFile(KNOB_MODE_WRITEONCE, "pintool",
    "i", "", "specify input file name");
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "pin.out", "specify input file name");
KNOB<string> KnobFilePath(KNOB_MODE_WRITEONCE, "pintool",
    "p", "", "specify input file name");

struct loop
{
	string image_name;
	ADDRINT entry;
	set<ADDRINT> tail;
	set<ADDRINT> exit;
	int iterations;
	map<string, vector<pair<UINT64,UINT64>>> duration_iterations;
	UINT64 start_time;
	UINT64 end_time;
	
	string func_name;

};

map<ADDRINT, loop*> entry_map;
map<ADDRINT, set<loop*>> tail_map;
map<ADDRINT, set<loop*>> exit_map;

string filepath;

INT32 numThreads = 0;

// Force each thread's data to be in its own data cache line so that
// multiple threads do not contend for the same data cache line.
// This avoids the false sharing problem.
#define PADSIZE 56  // 64 byte line size: 64-8

// a running count of the instructions
struct thread_data_t			//change to struct
{
  public:
    thread_data_t() { } 
    ofstream threadFile;
    loop* cur_loop;
    string name;
    set<loop*> executed_loops;
    UINT8 _pad[PADSIZE];
};

// key for accessing TLS storage in the threads. initialized once in main()
static  TLS_KEY tls_key = INVALID_TLS_KEY;

// function to access thread-specific data
thread_data_t* get_tls(THREADID threadid)
{
    thread_data_t* tdata = 
          static_cast<thread_data_t*>(PIN_GetThreadData(tls_key, threadid));
    return tdata;
}

VOID ThreadStart(THREADID threadid, CONTEXT *ctxt, INT32 flags, VOID *v)
{
    *OutFile << "Entering thread "<< std::dec << threadid << " of " << PIN_GetPid() << endl;
    numThreads++;
    thread_data_t* tdata = new thread_data_t;
    std::ostringstream oss, ssname;
    oss << filepath;
    oss << threadid;
    oss << "_";
    oss << PIN_GetPid();
    (tdata->threadFile).open(oss.str().c_str());
    if(!(tdata->threadFile))
    	*OutFile<<"Failed opening file "<<endl;
    *OutFile << "Filename : "<< oss.str().c_str()<<endl;
    //(tdata->threadFile)<<"Filename is "<<endl;
    ssname << threadid;
    ssname << "_";
    ssname << PIN_GetPid();
    tdata->name = ssname.str().c_str();
    tdata->cur_loop=NULL;
    if (PIN_SetThreadData(tls_key, tdata, threadid) == FALSE)
    {
        cerr << "PIN_SetThreadData failed" << endl;
        PIN_ExitProcess(1);
    }
}

VOID ThreadFini(THREADID threadIndex, const CONTEXT *ctxt, INT32 code, VOID *v)
{
    thread_data_t* tdata = static_cast<thread_data_t*>(PIN_GetThreadData(tls_key, threadIndex));
    if(tdata->cur_loop !=NULL)
    {
    	//tdata->cur_loop->end_time = time(NULL);
		//double temp_diff = difftime(tdata->cur_loop->end_time, tdata->cur_loop->start_time);
		OS_Time(&tdata->cur_loop->end_time);
		UINT64 temp_diff = tdata->cur_loop->end_time - tdata->cur_loop->start_time;
		pair<UINT64,UINT64> temp_pair(temp_diff, tdata->cur_loop->iterations);
		map<string, vector<pair<UINT64, UINT64>>>::iterator dur_iter = (tdata->cur_loop->duration_iterations).find(tdata->name);
		if(dur_iter != (tdata->cur_loop->duration_iterations).end())
		{
			vector<pair<UINT64, UINT64>> dur_vec = dur_iter->second;
			dur_vec.push_back(temp_pair);
			dur_iter->second = dur_vec;
		}
		else
		{
			vector<pair<UINT64, UINT64>> dur_vec;
			dur_vec.push_back(temp_pair);
			(tdata->cur_loop->duration_iterations)[tdata->name] = dur_vec;
		}

		//(tdata->threadFile) << "End " << tdata->cur_loop->func_name << " " <<std::hex << (tdata->cur_loop->entry)<<" " << dec << tdata->cur_loop->end_time<<endl;
		
		(tdata->executed_loops).insert(tdata->cur_loop);
		tdata->cur_loop = NULL;
	}
    set<loop*> loops = tdata->executed_loops;
    for(auto l : loops)
    {
    	map<string, vector<pair<UINT64, UINT64>>>::iterator dur_iter = (l->duration_iterations).find(tdata->name);
    	if(dur_iter != (tdata->cur_loop->duration_iterations).end())
    	{
    		vector<pair<UINT64, UINT64>> dur_vec = dur_iter->second;

    		for(auto p : dur_vec)
    		{
    	    
    	    	(tdata->threadFile)<<std::dec << std::fixed << std::setprecision(6) << p.first/1000000.0<<" "<< p.second << " " <<hex<<l->entry<<" "<<l->func_name<<endl;
    		}
    	}

    }
    *OutFile << "Exiting thread "<< std::dec << threadIndex << " of " << PIN_GetPid() << endl;
    //(tdata->threadFile).close();
    delete tdata;
}

VOID PIN_FAST_ANALYSIS_CALL entry_fun(ADDRINT addr, loop *l, THREADID threadid)
{
    thread_data_t* tdata = get_tls(threadid);

	if(tdata->cur_loop == NULL)	//Outer loop starts
	{
		tdata->cur_loop = l;
		OS_Time(&l->start_time);
		l->iterations=0;
		//(tdata->threadFile) << "Enter "<<l->func_name<<" "<<std::hex<<l->entry<<" " << dec <<  l->start_time  << endl;
	}
	else
	{
		if(tdata->cur_loop == l)	//Iteration of cur_loop
		{
			l->iterations++;
			//(tdata->threadFile) << "Iterate "<<l->func_name<<" "<<std::hex<<l->entry<<endl;

		}
		else
		{
			//(tdata->threadFile) <<"Nested loop found at "<<l->func_name<<" "<<hex<<addr<<endl;
		}
	}
}

VOID PIN_FAST_ANALYSIS_CALL exit_fun(ADDRINT addr,  THREADID threadid, set<loop*>* loop_vec)
{	

	thread_data_t* tdata = get_tls(threadid);

    	auto it = loop_vec->find(tdata->cur_loop);
		if(it != loop_vec->end())
		{
			//tdata->cur_loop->end_time = time(NULL);
			OS_Time(&tdata->cur_loop->end_time);
			//double temp_diff = difftime(tdata->cur_loop->end_time, tdata->cur_loop->start_time);
			UINT64 temp_diff = tdata->cur_loop->end_time - tdata->cur_loop->start_time;
			pair<UINT64,UINT64> temp_pair(temp_diff, tdata->cur_loop->iterations);
			map<string, vector<pair<UINT64, UINT64>>>::iterator dur_iter = (tdata->cur_loop->duration_iterations).find(tdata->name);
			if(dur_iter != (tdata->cur_loop->duration_iterations).end())
			{
				vector<pair<UINT64, UINT64>> dur_vec = dur_iter->second;
				dur_vec.push_back(temp_pair);
				dur_iter->second = dur_vec;
			}
			else
			{
				vector<pair<UINT64, UINT64>> dur_vec;
				dur_vec.push_back(temp_pair);
				(tdata->cur_loop->duration_iterations)[tdata->name] = dur_vec;
			}

			

			//(tdata->threadFile) << "End " << tdata->cur_loop->func_name << " " <<std::hex << (tdata->cur_loop->entry)<<" " << dec << tdata->cur_loop->end_time<<" Exit block " << hex << addr << endl;
			
			(tdata->executed_loops).insert(tdata->cur_loop);
			tdata->cur_loop = NULL;

		}



}

VOID AfterForkInChild(THREADID threadid, const CONTEXT* ctxt, VOID * arg)
{
    
    *OutFile << "Entering child " << std::dec << threadid << " of " << PIN_GetPid() << endl;
    numThreads++;
    thread_data_t* tdata = new thread_data_t;
    std::ostringstream oss, ssname;
    oss << filepath;
    oss << threadid;
    oss << "_";
    oss << PIN_GetPid();
    (tdata->threadFile).open(oss.str().c_str());
    ssname << threadid;
    ssname << "_";
    ssname << PIN_GetPid();
    tdata->name = ssname.str().c_str();
    tdata->cur_loop=NULL;

    if (PIN_SetThreadData(tls_key, tdata, threadid) == FALSE)
    {
        cerr << "PIN_SetThreadData failed" << endl;
        PIN_ExitProcess(1);
    }
  
}


VOID Instruction(INS ins, VOID *v)
{
	ADDRINT instr_address = INS_Address(ins);
	IMG img1 = IMG_FindByAddress(instr_address);
	if(IMG_Valid(img1))
	{
		ADDRINT img_addr = IMG_LowAddress(img1);
		ADDRINT offset = instr_address - img_addr;
		string im = IMG_Name(img1);
		string im_name;
		int pos=-1;
		for(size_t i=0; i<im.size(); i++)
		{
			if(im[i] == '/')
				pos = i;
		}
		if(pos >= 0)
		{
			im_name = im.substr(pos+1);
		}
		
		map<ADDRINT,set<loop*>>::iterator exit_iter = exit_map.find(offset);
			if(exit_iter != exit_map.end())
			{
				set<loop*> loop_vec = exit_iter->second;
				INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)exit_fun, IARG_FAST_ANALYSIS_CALL, IARG_ADDRINT , offset,  IARG_THREAD_ID, IARG_PTR, &(exit_iter->second), IARG_END); //IARG_PTR, iter->second,

			}
		map<ADDRINT,loop*>::iterator iter = entry_map.find(offset);
		if(iter != entry_map.end())
		{
			loop* l = iter->second;

			if(im_name == l->image_name)
			{
				INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)entry_fun, IARG_FAST_ANALYSIS_CALL, IARG_ADDRINT, offset, IARG_PTR, l, IARG_THREAD_ID, IARG_END);
			}

		}
		/*else
		{
			map<ADDRINT,set<loop*>>::iterator iter = exit_map.find(offset);
			if(iter != exit_map.end())
			{
				set<loop*> loop_vec = iter->second;
				INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)exit_fun, IARG_FAST_ANALYSIS_CALL, IARG_ADDRINT , offset,  IARG_THREAD_ID, IARG_PTR, &(iter->second), IARG_END); //IARG_PTR, iter->second,

			}
		
		}*/
		
	}
}

void imageLoad(IMG img, VOID *v)
{
    *OutFile << "I: "<< IMG_Name(img) << " "<<hex << IMG_LowAddress(img)<<" "<<IMG_HighAddress(img)<<endl;     
}

INT32 Usage()
{
    cerr << "This tool counts the number of dynamic instructions executed" << endl;
    cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
    return 1;
}

int main(int argc, char* argv[])
{
	PIN_InitSymbols();
	if(PIN_Init(argc, argv))
		return Usage();
	filepath = KnobFilePath.Value().c_str();

    OutFile = KnobOutputFile.Value().empty() ? &cout : new std::ofstream(KnobOutputFile.Value().c_str());

	InFile.open(KnobInputFile.Value().c_str());
	string line;
	
    while ( getline (InFile,line) )
    {
     	istringstream ss(line);
     	loop *l = new loop;
     	string flag="image";
     	int count=0;
     	while(ss)
		{
			if(flag == "image")
			{
				string word;
				ss >> word;
			
			    int pos=-1;
				for(size_t i=0; i<word.size(); i++)
				{
					if(word[i] == '/')
						pos = i;
				}
				if(pos >= 0)
				{
					*OutFile<<"substr "+word.substr(pos+1)<<endl;
					l->image_name = word.substr(pos+1);
				}
				flag = "entry";
			}
			else if(flag == "entry")
			{
				int addr;
				ss >> std::hex >> addr;
				l->entry = addr;
				*OutFile<<"entry addr "<<std::hex<<addr<<endl;
				flag = "tail";
			}
			else if(flag == "tail")
			{
				ss >> std::dec >> count;
				*OutFile<<"tail count "<<count<<endl;
				flag = "tailval";
			}
			else if(flag == "tailval")
			{
				count--;
				int addr;
				ss >> std::hex >> addr;
				*OutFile<<"tail addr "<< std::hex << addr<<" "<<std::dec<<count<<endl;
				(l->tail).insert(addr);
				map<ADDRINT, set<loop*>>::iterator tail_iter = tail_map.find(addr);
				if(tail_iter != tail_map.end())
				{
					set<loop*> loop_vec = tail_iter->second;
					loop_vec.insert(l);
					tail_iter->second = loop_vec;
				}
				else
				{
					set<loop*> loop_vec;
					loop_vec.insert(l);
					tail_map[addr] = loop_vec;
				}

				if(count==0)
				{
					flag = "exit";
				}
			}
			else if(flag == "exit")
			{
				ss >> std::dec >> count;
				*OutFile<<"exit count "<<count<<endl;
				if(count == 0)
					flag = "funcname";
				else
					flag ="exitval";
			}
			else if(flag == "exitval")
			{
				count--;
				int addr;
				ss >> std::hex >> addr;
				*OutFile<<"exit addr "<<std::hex<<addr<<endl;
				(l->exit).insert(addr);
				map<ADDRINT, set<loop*>>::iterator exit_iter = exit_map.find(addr);
				if(exit_iter != exit_map.end())
				{
					set<loop*> loop_vec = exit_iter->second;
					loop_vec.insert(l);
					exit_iter->second = loop_vec;

				}
				else
				{
					set<loop*> loop_vec;
					loop_vec.insert(l);
					exit_map[addr] = loop_vec;
				}
				if(count==0)
				{
					flag = "funcname";
				}
			}
			else if(flag == "funcname")
			{
				string word;
				ss >> word;
				l->func_name = word;
				*OutFile<<"func "<<word<<" "<<l->func_name<<endl;
				flag = "done";

			}
			else if(flag == "done")
			{
				string word;
				ss >> word;
			}	
		}
		l->iterations = 0;
		entry_map[l->entry] = l;

      	*OutFile << line << '\n';
    }

    for(auto entr : entry_map)
    {
    	*OutFile<<std::hex<<entr.first<<" : "<<entr.second->entry<<endl;
    }

    for(auto exi : exit_map)
    {
    	*OutFile<<std::hex<<exi.first<<" : ";
    	for(auto e : exi.second)
    		*OutFile<<std::hex<<e->entry<<" ";
    	*OutFile<<endl;
    }
    tls_key = PIN_CreateThreadDataKey(NULL);
    if (tls_key == INVALID_TLS_KEY)
    {
        cerr << "number of already allocated keys reached the MAX_CLIENT_TLS_KEYS limit" << endl;
        PIN_ExitProcess(1);
    }

       PIN_AddThreadStartFunction(ThreadStart, NULL);

    // Register Fini to be called when thread exits.
    PIN_AddThreadFiniFunction(ThreadFini, NULL);

    //TRACE_AddInstrumentFunction(Trace, NULL);
    IMG_AddInstrumentFunction (imageLoad, 0);
    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddForkFunction(FPOINT_AFTER_IN_CHILD, AfterForkInChild, 0);
	PIN_StartProgram();
}
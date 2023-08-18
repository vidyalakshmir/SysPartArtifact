#include <iostream>
#include <string>
#include <string.h>
using namespace std;

#include "operation/find.h"
#include "pass/syspartpass.h"
#include "conductor/interface.h"
#include "../../analysis/app/src/loopanalysis.h"
#include "../../analysis/app/src/ipcallgraph.h"


char* filename;
char* funcname;
char* newelf;
address_t address;
string sharedlib;
string sharedlibfn;
void handleMainLoopInLib()
{
    EgalitoInterface *egalito = new EgalitoInterface(false, true);
    egalito->initializeParsing();
    egalito->parse(filename,true);
    auto prog = egalito->getProgram();
    auto setup = egalito->getSetup();

    Function* start_func = NULL; 
    Function* source_func = NULL; 

    for(auto module : CIter::children(prog))
    {
        cout<<module->getName()<<endl;
        for(auto func : CIter::functions(module))
        {
                if(func->getName() == "main")
                {
                    start_func = func;
                }
                if(func->getName() == funcname || func->hasName(funcname))
                {
                    source_func = func;
                }
        }
    }
    if(source_func == NULL) 
    {
        cout<<"Function doesnt exist "<<funcname<<endl;
        return;
    }

    auto loop_mod = (Module*)source_func->getParent()->getParent();

    LoopAnalysis loopAnalysis;
    loopAnalysis.detectLoops(source_func);
    
    auto loop_addr = address;
    auto loops = loopAnalysis.getLoops();
    LoopAnalysis::Loop *l = NULL;
    for(auto ll : loops)
    {
        address_t laddr;
        Module* lmod;
        tie(laddr, lmod) = ll.first;
        if(laddr == loop_addr & lmod->getName() == loop_mod->getName())
        {
            l = ll.second;
        }

    }
    if(l == NULL)
    {
        cout<<"Loop NULL";
        return;
    }
    //loopAnalysis.printLoop(l);
    auto non_loop_parents = loopAnalysis.findNonLoopParent(source_func, l);

    auto instr = ChunkFind().findInnermostInsideInstruction(source_func, address);
    auto bl = instr->getParent();
    auto prev_sibling = bl->getPreviousSibling();
    /*cout<<"INSTR "<<std::hex<<instr->getAddress()<<" BL "<<bl->getAddress()<<endl;
    if(prev_sibling != NULL)
        cout<<"Prev "<<prev_sibling->getAddress()<<endl;
    */
    bool special_case = true;
    for(auto p : non_loop_parents) //Previous sibling is one of the non_loop_parents
    {
        //cout<<"Non loop parent "<<std::hex<<p->getAddress()<<endl;
        if(p->getAddress() == prev_sibling->getAddress())
        {
            special_case = false;
            break;
        }
    }
    
    egalito->parse(sharedlib);

    Function *enforcement_func = NULL;

    for(auto module : CIter::children(prog))
    {
        
        auto mod = module->getName();
        if(mod.find(sharedlib) == std::string::npos)
            continue;
        
        for(auto func : CIter::functions(module))
        {
                if(func->getName() == sharedlibfn)
                {
                    enforcement_func = func;
                    break;
                }
        }
    }
    if(enforcement_func == NULL)
    {
        cout<<"Enforcement function doesn't exist"<<endl;
        return;
    }
    SyspartPass syspass(prog, source_func, address, enforcement_func, special_case, (Block*)prev_sibling, non_loop_parents);
    prog->accept(&syspass);
    egalito->generate(newelf, false);

    //Find the caller of this function
    /*
    /
    IPCallGraph ipcallgraph;
    ipcallgraph.setProgram(prog);
    ipcallgraph.setRoot(start_func);
    ipcallgraph.setIcanalysis(true);
    ipcallgraph.setTypeArmor(true);
    ipcallgraph.generate();
    auto ipnode = ipcallgraph.getNode(source_func);
    if(ipnode == NULL)
    {
        cout<<"\n Node NULL"<<endl;
        return false;
    }
    auto pp = ipnode->getParent();
    /
    /*for(auto p : pp)
    {
        cout<<p->getFunction()->getName()<<endl;
    }*/
    return;
}
void injectSharedLibFn()
{
    EgalitoInterface *egalito = new EgalitoInterface(false, true);
    egalito->initializeParsing();
    egalito->parse(filename);
    auto prog = egalito->getProgram();
    auto setup = egalito->getSetup();

    Function* source_func = NULL; 
    for(auto module : CIter::children(prog))
    {        cout<<"M "<<module->getName()<<endl;

        for(auto func : CIter::functions(module))
        {
                if(func->getName() == funcname || func->hasName(funcname))
                {
                    source_func = func;
                    break;
                }
        }
    }
    if(source_func == NULL) 
    {
        cout<<"Function doesnt exist in executable"<<funcname<<endl;
        handleMainLoopInLib();
        return;
    }
    
    auto loop_mod = (Module*)source_func->getParent()->getParent();
    /*if(loop_mod->getName() != "module-(executable)")    //When mainloop is not a part of the binary
    {
        bool ret = handleMainLoopInLib();
        if(!ret)
            return;
    }*/
    LoopAnalysis loopAnalysis;
    loopAnalysis.detectLoops(source_func);
    
    auto loop_addr = address;
    auto loops = loopAnalysis.getLoops();
    LoopAnalysis::Loop *l = NULL;
    for(auto ll : loops)
    {
        address_t laddr;
        Module* lmod;
        tie(laddr, lmod) = ll.first;
        if(laddr == loop_addr & lmod->getName() == loop_mod->getName())
        {
            l = ll.second;
        }

    }
    if(l == NULL)
    {
        cout<<"Loop NULL";
        return;
    }
    //loopAnalysis.printLoop(l);
    auto non_loop_parents = loopAnalysis.findNonLoopParent(source_func, l);

    auto instr = ChunkFind().findInnermostInsideInstruction(source_func, address);
    auto bl = instr->getParent();
    auto prev_sibling = bl->getPreviousSibling();
    /*cout<<"INSTR "<<std::hex<<instr->getAddress()<<" BL "<<bl->getAddress()<<endl;
    if(prev_sibling != NULL)
        cout<<"Prev "<<prev_sibling->getAddress()<<endl;
    */
    bool special_case = true;
    for(auto p : non_loop_parents) //Previous sibling is one of the non_loop_parents
    {
        //cout<<"Non loop parent "<<std::hex<<p->getAddress()<<endl;
        if(p->getAddress() == prev_sibling->getAddress())
        {
            special_case = false;
            break;
        }
    }
    
    egalito->parse(sharedlib);

    Function *enforcement_func = NULL;

    for(auto module : CIter::children(prog))
    {
        
        auto mod = module->getName();
        if(mod.find(sharedlib) == std::string::npos)
            continue;
        
        for(auto func : CIter::functions(module))
        {
                if(func->getName() == sharedlibfn)
                {
                    enforcement_func = func;
                    break;
                }
        }
    }
    if(enforcement_func == NULL)
    {
        cout<<"Enforcement function doesn't exist"<<endl;
        return;
    }
    SyspartPass syspass(prog, source_func, address, enforcement_func, special_case, (Block*)prev_sibling, non_loop_parents);
    prog->accept(&syspass);
    egalito->generate(newelf, false);
}
int main(int argc, char* argv[])
{
	filename = argv[1];
	funcname = argv[2];
	address = (address_t)strtol(argv[3], NULL, 16); 
    newelf = argv[4];
    sharedlib = argv[5];
    sharedlibfn = argv[6];
    injectSharedLibFn();
}
#include "nonreturnAnalysis.h"
#include<iostream>
using namespace std;

#include "analysis/controlflow.h"

const std::vector<std::string> NonReturnAnalysis::knownList = {
    "exit", "_exit", "abort",
    "__libc_fatal", "__assert_fail", "__stack_chk_fail",
    "__malloc_assert", "_dl_signal_error",
    "__cxa_throw",
    "_ZSt20__throw_out_of_rangePKc",
    "_ZSt19__throw_logic_errorPKc",
    "_ZSt17__throw_bad_allocv",
    "_ZSt24__throw_invalid_argumentPKc"
};

bool NonReturnAnalysis::hasLinkToNeverReturn(ControlFlowInstruction *cfi) {
    if(auto pltLink = dynamic_cast<PLTLink *>(cfi->getLink())) {
        auto trampoline = pltLink->getPLTTrampoline();
        auto pltName = trampoline->getExternalSymbol()->getName();
        for(auto name : knownList) {
            if(pltName == name) {
                return true;
            }
        }
    }
    else if(auto target = dynamic_cast<Function *>(
        &*cfi->getLink()->getTarget())) {

        if(!target->returns()) return true;
        if(inList(target)) return true;
        for(auto name : knownList) {
            if(target->hasName(name)) return true;
        }
    }

    return false;
}

bool NonReturnAnalysis::inList(Function *function) {
    if(std::find(nonReturnList.begin(), nonReturnList.end(), function)
        != nonReturnList.end()) {

        return true;
    }
    return false;
}

int NonReturnAnalysis::isNonReturn(ControlFlowGraph *cfg, Block* bl, std::set<Block*> visited)
{
    int ret = 1;    //Assume it is non-return

    auto noreturn_iter = noreturn_done.find(bl);
    if(noreturn_iter != noreturn_done.end())
    {
        return noreturn_iter->second;
    }
    if(visited.count(bl) != 0)  //If already visited in this path
    {
        //cout<<std::hex <<bl->getAddress()<<" returning -1 A"<<endl;
        return -1;
    }
    visited.insert(bl);

    for(auto instr : CIter::children(bl))
    {
        if(auto cfi = dynamic_cast<ControlFlowInstruction *>(
                instr->getSemantic())) 
        {
            if(hasLinkToNeverReturn(cfi))
            {
                cfi->setNonreturn();
            }
            if(cfi->getMnemonic() != "jmp" && cfi->getMnemonic() != "callq") 
            {
                if(auto target = dynamic_cast<Function *>(&*cfi->getLink()->getTarget())) //if cfg is a conditional jump to a function
                {
                    if(cfi->returns())
                        ret = 0;
                    break;
                }
            }
            if(!cfi->returns())   //Non-returning
            {
                //cout<<std::hex <<bl->getAddress()<<" returning 1 B"<<endl;
                return 1;
            }
        }
    }
    bool endNode = true;
    auto node_id = cfg->getIDFor(bl);
    auto node = cfg->get(node_id);
    for(auto& link : node->forwardLinks())
    {
        endNode = false;
        auto cflink = dynamic_cast<ControlFlowLink *>(&*link);
        auto dest_id = cflink->getTargetID();
        auto dest_node = (cfg->get(dest_id))->getBlock();
        int t = isNonReturn(cfg, dest_node, visited);
        if(t == -1)
            continue;
        ret = ret & t;          //Even if one path is returning, this would make the block non-returning
    }
    if(endNode)
        ret = 0;
    //cout<<std::hex <<bl->getAddress()<<" returning "<<ret<<endl;
    noreturn_done[bl] = ret;
    return ret;
}

void NonReturnAnalysis::run(Program* program)
{
	bool flag;
	do
	{
		//std::cout<<"Entering : "<<endl;
   		flag = true;

		for(auto module : CIter::children(program))
    	{
        	for(auto func : CIter::functions(module))
        	{
        		if(!func->returns())
        		{
        			if(nonReturnList.count(func) == 0)
        			{
        				nonReturnList.insert(func);
        				//std::cout<<"A NR "<<func->getName()<<endl;
        				flag = false;
        			}
        			continue;
        		}
        		auto cfg = new ControlFlowGraph(func);
                if((cfg->getGraph()).size() == NULL)
                {
                    continue;
                }
   	        	auto start_bl = (cfg->get(0))->getBlock();
        		std::set<Block*> visited;
        		noreturn_done.clear();
        		int ret = isNonReturn(cfg, start_bl, visited);
        		if(ret == 1)
        		{
            		func->setNonreturn();
            		nonReturnList.insert(func);
		        	//std::cout<<"B NR "<<func->getName()<<endl;
            		flag = false;
        		}
        	}
    	}
    }while(flag == false);
  
  }
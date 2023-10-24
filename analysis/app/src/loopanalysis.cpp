//Given a function, returns the number of loops in that function
#include<iostream>
#include<stack>
using namespace std;

#include "loopanalysis.h"
#include "conductor/setup.h"
#include "conductor/conductor.h"
#include "instr/concrete.h"

int LoopAnalysis::getFnCalls(Block *b)
{
    int nfcalls=0;
    for (auto instr : CIter::children(b))
    {
            auto semantic = instr->getSemantic();
            if(auto v = dynamic_cast<ControlFlowInstruction *>(semantic)) {
                if(v->getMnemonic() == "callq") {   //Direct calls
                    nfcalls++;
                }
                else if(auto link = v->getLink()) {
                    if(link->isExternalJump()) {    //Tail call recursion
                        nfcalls++;
                    }
                }
            }
            else if(dynamic_cast<IndirectCallInstruction *>(semantic)) {    //Indirect calls
                nfcalls++;
            }
            else if(auto v = dynamic_cast<IndirectJumpInstruction *>(semantic)) {
                if(!v->isForJumpTable()) {          //Indirect tail recursion
                    nfcalls++;
                }
            }
            else if(auto v = dynamic_cast<DataLinkedControlFlowInstruction *>(semantic)) {
                if(v->isCall()) {
                    nfcalls++;
                }
            }

    }
    return nfcalls;
}

int LoopAnalysis::computeNumBlocks(id_t startID, ControlFlowNode* endNode, int &reached, int &nblocks)
{
    int nfcalls=0;
    for(const auto& link : endNode->backwardLinks())
    {
        auto predlink = dynamic_cast<ControlFlowLink *>(&*link);
        auto predecessorNode = &graph[predlink->getTargetID()];
        auto predecessor = graph[predlink->getTargetID()].getBlock();
        auto predecessorID = graph[predlink->getTargetID()].getID();
        if(predecessorID == startID)
        {
            reached = 1;
            nfcalls = getFnCalls(predecessor);
            return nfcalls;
        }
        else
        {
            nblocks++;
            int temp = computeNumBlocks(startID, predecessorNode, reached, nblocks);
            nfcalls = nfcalls + temp;
            if(reached == 1)
            {
                temp = getFnCalls(predecessor);
                nfcalls = nfcalls + temp;
                break;
            }

        }
    }
    return nfcalls;
}

/* Algorithm to detect loops, backedge and loopbody
   http://pages.cs.wisc.edu/~fischer/cs701.f14/finding.loops.html

*/
void LoopAnalysis::naturalLoops(ControlFlowNode* headerNode, ControlFlowNode* exitNode, set<Block*> *result)
{
    vector<ControlFlowNode*> loopBody;
    std::vector<Block*> loopBlocks;
    loopBody.push_back(headerNode);                 //Add header node to loopbody
    loopBlocks.push_back(headerNode->getBlock());

 
    stack<ControlFlowNode*> loopStack;
    loopStack.push(exitNode);                       //Push exit node to stack
    while(!loopStack.empty())
    {
        auto topStack = loopStack.top();            //cur =  top of stack
        loopStack.pop();
        if(find(loopBlocks.begin(), loopBlocks.end(), topStack->getBlock()) == loopBlocks.end()) //If cur not in loopbody
        {
            loopBody.push_back(topStack);                           //Add cur to loopbody
            loopBlocks.push_back(topStack->getBlock());
            
            for(const auto& link : topStack->backwardLinks())
            {
                auto predLink = dynamic_cast<ControlFlowLink *>(&*link);
                auto predecessorNode = &graph[predLink->getTargetID()];
                loopStack.push(predecessorNode);                        //Add predecessor(cur) to stack
            }
        }            

    }
    for(auto b : loopBody)
    {
        result->insert(b->getBlock());
    }
}

//Detects if loops exist in the function passed as parameter
void LoopAnalysis::detectLoops(Function* func)
{
    auto cur_module = (Module*)func->getParent()->getParent();
    ControlFlowGraph cfg(func);
    //Identify dominator relationships
    typedef std::vector<Block *> BlockList;
    map<Block *, BlockList> domMap;
    graph = cfg.getGraph();
    BlockList nodeList, remainingNodeList;
    for(auto& node : graph)             //Create a list of all nodes in the CFG
    {
        auto block = node.getBlock();
        nodeList.push_back(block);
    }
    if(nodeList.size() <= 1)
    {   
        return;
    }
    Block* startNode = graph[0].getBlock();
    for(auto& node : nodeList)
    {
        domMap.insert(make_pair(node, nodeList));       //dom(N) = List of all nodes (initialization)
    }

    map<Block *, BlockList>::iterator it;
    BlockList workList, processedList;
    workList.push_back(startNode);              //Push start node to worklist

    while(!workList.empty())
    {
        Block* workingNode;
        workingNode = workList.at(0);  //Get 0th element from list as working node
        processedList.push_back(workingNode);
        workList.erase(workList.begin());                  //Remove the current working node from list

        std::vector<Block *> prevDomList, predecessorDomList,curDomList;
        auto itCurDomList = domMap.find(workingNode);     //Get the dominator list of the working node
        curDomList = itCurDomList->second;
        BlockList newDomList;
        std::vector<Block *>::iterator it;
        int count=0;
        auto node = cfg.get(cfg.getIDFor(workingNode));
        for(const auto& link : node->backwardLinks())    //For each predecessor node of the working node
        {
            newDomList.clear();
            auto predlink = dynamic_cast<ControlFlowLink *>(&*link);
            auto predecessor = graph[predlink->getTargetID()].getBlock(); //Get predecessor block
            auto itPredDomList = domMap.find(predecessor);  //Find the dominator list of the predecessor node
            if(itPredDomList!= domMap.end())
            {
                predecessorDomList = itPredDomList->second;
                if(count>0 )
                {
                    sort(predecessorDomList.begin(), predecessorDomList.end());
                    sort(prevDomList.begin(), prevDomList.end());
                    set_intersection(predecessorDomList.begin(), predecessorDomList.end(), prevDomList.begin(), prevDomList.end(),back_inserter(newDomList)); //Finding the intersection of dominator list of all predecessor nodes
                }
                else
                {
                    newDomList = predecessorDomList;
                }
            }
            prevDomList = newDomList;
            count++;
        }
        newDomList.push_back(workingNode);      
        sort(curDomList.begin(), curDomList.end());
        sort(newDomList.begin(), newDomList.end());
        bool result = (curDomList.size()!=newDomList.size())?false:true;
        for(unsigned int i=0; i<curDomList.size()&&result; i++)
        {
            if(curDomList.at(i) != newDomList.at(i))
                result = false;
        }
        if(!result)     //Push the successors of the working node only if the dominator list of the working node has changed
        {
            domMap.at(workingNode) = newDomList;
            auto node = cfg.get(cfg.getIDFor(workingNode));
            for(const auto& link : node->forwardLinks())
            {
                auto succlink = dynamic_cast<ControlFlowLink *>(&*link);
                auto successor = graph[succlink->getTargetID()].getBlock(); //Get successor block
                                        workList.push_back(successor);
                /*
                if(std::find(workList.begin(), workList.end(), successor) == workList.end()) //Changed to only add sucessor if not present in worklist
                {
                    if(successor->getAddress() != startNode->getAddress())
                    {
                        workList.push_back(successor);
                    }
                }
                */   
            }
        }
    }
    map<Block *, BlockList>::iterator it3;

    int nloops=0;
    for(auto block : nodeList)
    {
        if(std::find(processedList.begin(), processedList.end(), block) != processedList.end())
        {   auto node = cfg.get(cfg.getIDFor(block));
            for(const auto& link : node->forwardLinks())
            {
                auto succlink = dynamic_cast<ControlFlowLink *>(&*link);
                auto successor = graph[succlink->getTargetID()].getBlock();
                auto successorID = graph[succlink->getTargetID()].getID();
                auto successorNode = cfg.get(successorID);
                auto itDomList = domMap.find(block);
                if(itDomList!= domMap.end())
                {
                    auto domList =  itDomList->second;
                    if( find(domList.begin(), domList.end(), successor) != domList.end())
                    {
                        //We have a loop with entry node 'successorNode' and 'tail node 'node'; entry block 'successor' and tail block 'block'
                        //Find if a loop with same entry is recorded
                        bool loop_exists = false;
                        Loop *l;
                        auto successor_addr = successor->getAddress();
                        auto entry_iter = loops.find(make_pair(successor_addr, cur_module));
                        if(entry_iter != loops.end())
                        {
                            loop_exists = true;
                            l = entry_iter->second;
                        }
                     

                        if(!loop_exists)        //New loop object
                        {
                            l = new Loop;
                            l->entry = successor;
                            l->entry_addr = successor_addr;
                            l->func = func;
                            l->module = cur_module; 
                            (l->tail).insert(block);
                      
                            set<Block*> lBody;
                            naturalLoops(successorNode, node, &lBody);
                            for(auto body : lBody)
                                (l->loopBody).insert(body);
                            //Finding exit blocks
                            for(auto x : l->loopBody)
                            {
                                auto loop_node = cfg.get(cfg.getIDFor(x));
                                for(const auto& loop_node_link : loop_node->forwardLinks())
                                {
                                    auto loop_node_succ_link = dynamic_cast<ControlFlowLink *>(&*loop_node_link);
                                    auto loop_succ = graph[loop_node_succ_link->getTargetID()].getBlock();
                                    auto bl_iter = (l->loopBody).find(loop_succ);
                                    if(bl_iter == (l->loopBody).end())
                                    {
                                        (l->exitBlocks).insert(loop_succ);
                                    }
                                }
                            }   
                            loops.insert(make_pair(make_pair(successor_addr, cur_module), l));  //Inserting the loop object in the global loops
                        
                        }
                        else
                        {
                            (l->tail).insert(block);
                            set<Block*> lBody;
                            naturalLoops(successorNode, node, &lBody);

                            for(auto lb: lBody)
                                (l->loopBody).insert(lb);

                            //Recomputing l->exitBlocks
                            (l->exitBlocks).clear();
                            for(auto x : l->loopBody)
                            {
                                auto loop_node = cfg.get(cfg.getIDFor(x));
                                for(const auto& loop_node_link : loop_node->forwardLinks())
                                {
                                    auto loop_node_succ_link = dynamic_cast<ControlFlowLink *>(&*loop_node_link);
                                    auto loop_succ = graph[loop_node_succ_link->getTargetID()].getBlock();
                                    auto bl_iter = (l->loopBody).find(loop_succ);
                                    if(bl_iter == (l->loopBody).end())
                                    {
                                        (l->exitBlocks).insert(loop_succ);
                                    }
                                }
                            }
                            entry_iter->second = l; //Updating the loop object in the global loops
                        }
                        nloops++;
                    }
                }
            }
        }
        
    }
}


int LoopAnalysis::getLoopCount(Function* func)
{
    ControlFlowGraph cfg(func);
   
    //Identify dominator relationships
    typedef std::vector<Block *> BlockList;
    map<Block *, BlockList> domMap;
    graph = cfg.getGraph();
    BlockList nodeList, remainingNodeList;
    for(auto& node : graph)             //Create a list of nodes
    {
        auto block = node.getBlock();
        nodeList.push_back(block);
    }
    Block* startNode = graph[0].getBlock();
    for(auto& node : nodeList)
    {
        domMap.insert(make_pair(node, nodeList));       //Initialize dom(allNodes)
    }

    map<Block *, BlockList>::iterator it;
    BlockList workList, processedList;
    workList.push_back(startNode);

    while(!workList.empty())
    {
        Block* workingNode;
        workingNode = workList.at(0);  //Get 0th element from list as working node
        processedList.push_back(workingNode);
        workList.erase(workList.begin());                  //Remove the current working node from list

        std::vector<Block *> prevDomList, predecessorDomList,curDomList;
        auto itCurDomList = domMap.find(workingNode);
        curDomList = itCurDomList->second;
        BlockList newDomList;
        std::vector<Block *>::iterator it;
        int count=0;
        auto node = cfg.get(cfg.getIDFor(workingNode));
        for(const auto& link : node->backwardLinks())
        {
            newDomList.clear();
            auto predlink = dynamic_cast<ControlFlowLink *>(&*link);
            auto predecessor = graph[predlink->getTargetID()].getBlock(); //Get predecessor block
            auto itPredDomList = domMap.find(predecessor);
            if(itPredDomList!= domMap.end())
            {
                predecessorDomList = itPredDomList->second;
                if(count>0 )
                {
                    sort(predecessorDomList.begin(), predecessorDomList.end());
                    sort(prevDomList.begin(), prevDomList.end());
                    set_intersection(predecessorDomList.begin(), predecessorDomList.end(), prevDomList.begin(), prevDomList.end(),back_inserter(newDomList));
                }
                else
                {
                    newDomList = predecessorDomList;
                }
            }
            prevDomList = newDomList;
            count++;
        }
        newDomList.push_back(workingNode);
        sort(curDomList.begin(), curDomList.end());
        sort(newDomList.begin(), newDomList.end());
        bool result = (curDomList.size()!=newDomList.size())?false:true;
        for(unsigned int i=0; i<curDomList.size()&&result; i++)
        {
            if(curDomList.at(i) != newDomList.at(i))
                result = false;
        }
        if(!result)
        {
            domMap.at(workingNode) = newDomList;
            auto node = cfg.get(cfg.getIDFor(workingNode));
            for(const auto& link : node->forwardLinks())
            {
                auto succlink = dynamic_cast<ControlFlowLink *>(&*link);
                auto successor = graph[succlink->getTargetID()].getBlock(); //Get successor block
                workList.push_back(successor);
            }
        }
    }
    map<Block *, BlockList>::iterator it3;
   

    int nloops=0;
    for(auto block : nodeList)
    {
        if(std::find(processedList.begin(), processedList.end(), block) != processedList.end()) //To account for only nodes whose dominator lists have been computed (Disconnected graphs)
        {   auto node = cfg.get(cfg.getIDFor(block));
            for(const auto& link : node->forwardLinks())
            {
                auto succlink = dynamic_cast<ControlFlowLink *>(&*link);
                auto successor = graph[succlink->getTargetID()].getBlock(); //There exists an edge from 'block' to 'successor'
                auto itDomList = domMap.find(block);
                if(itDomList!= domMap.end())
                {
                    auto domList =  itDomList->second;  //dominator list of 'block'
                    if( find(domList.begin(), domList.end(), successor) != domList.end()) //If successor is in the dominator list of 'block'
                    {
                        nloops++;                   //Loop detected
                    }
                }
            }
        }        
    }    
    return nloops;
}

//Print block offsets
void LoopAnalysis::printLoops()
{
    for(auto loop_obj : loops)
    {
        auto l = loop_obj.second;
        cout<<l->module->getLibrary()->getResolvedPath()<<" ";
        cout<<std::hex<<l->entry_addr<<" ";
        cout<<std::dec<<(l->tail).size()<<" ";
        for(auto t : l->tail)
        {
            Instruction* ins=NULL;
            for(auto instr : CIter::children(t))
            {
                ins = instr;
            }
            cout<<std::hex<<ins->getAddress()<<" ";
        }
     
        
        /*cout<<"\nLoop body : ";
        for(auto b : l->loopBody)
        {
            cout<<std::hex<<b->getAddress()<<" ";
        }
        */
        cout<<std::dec<<(l->exitBlocks).size()<<" ";
        for(auto ex : l->exitBlocks)
        {
            cout<<std::hex<<ex->getAddress()<<" ";
        }
        cout<<l->func->getName();
        cout<<"\n";
    }
}



//Printing block names
void LoopAnalysis::printLoopBlocks()
{
    for(auto loop_obj : loops)
    {
        auto l = loop_obj.second;
        cout<<"\n\n"<<l->func->getName()<<" " <<l->module->getLibrary()->getResolvedPath()<<"\n";
        cout<<"ENTRY : "<<l->entry->getName()<<"\n";
        cout<<"TAIL : ";
        for(auto t : l->tail)
        {
            cout<<t->getName()<<" ";
        }

        cout<<"\nEXIT : ";
        for(auto ex : l->exitBlocks)
        {
            cout<<ex->getName()<<" ";
        }
     
        cout<<"\nBODY : ";
        for(auto b : l->loopBody)
        {
            cout<<b->getName()<<" ";
        }
        cout<<"\n";
    }
}


map<pair<address_t,Module*>,LoopAnalysis::Loop*> LoopAnalysis::getLoops()
{
    return loops;
}

LoopAnalysis::Loop* LoopAnalysis::getLoop(address_t addr,Module* module)
{
    auto loop_iter = loops.find(make_pair(addr, module));
    Loop* l = NULL;
    if(loop_iter != loops.end())
        l = (loop_iter)->second;
    return l;                                              
}

void LoopAnalysis::printLoop(Loop* l)
{
    cout<<"\n"<<l->func->getName()<<" " <<l->module->getLibrary()->getResolvedPath()<<"\n";
    cout<<"ENTRY : "<<l->entry->getName()<<"\n";
    cout<<"TAIL : ";
    for(auto t : l->tail)
    {
        cout<<t->getName()<<" ";
    }
    cout<<"\nEXIT : ";
    for(auto ex : l->exitBlocks)
    {
        cout<<ex->getName()<<" ";
    }     
    cout<<"\nBODY : ";
    for(auto b : l->loopBody)
    {
        cout<<b->getName()<<" ";
    }
    cout<<"\n";

}

set<Block*> LoopAnalysis::findNonLoopParent(Function* func, Loop *l)
{
    set<Block*> parent_blocks;
    ControlFlowGraph cfg(func);
    auto graph = cfg.getGraph();
    for(auto& node : graph)             //Create a list of all nodes in the CFG
    {
        auto block = node.getBlock();
        if(block->getAddress() == l->entry_addr)
        {
            for(const auto& link : node.backwardLinks())
            {
                auto predlink = dynamic_cast<ControlFlowLink *>(&*link);
                auto predecessor = graph[predlink->getTargetID()].getBlock();
                auto flag = false;
                for(auto body : l->loopBody)
                {
                    if(body->getAddress() == predecessor->getAddress())
                        flag = true;
                }
                if(!flag)
                    parent_blocks.insert(predecessor);

            }

        }
    }
    return parent_blocks;
}


#include<queue>
#include<climits> //For INT_MIN


#include "syspart.h"
#include "loopanalysis.h"
//#include "dominance.h"
#include "pass/findsyscalls.h"
#include "conductor/interface.h"
#include "elf/elfdynamic.h"
#include "analysis/usedef.h"
#include "chunk/dump.h"
#include "analysis/walker.h"
#include "operation/find2.h"
#include "nonreturnAnalysis.h"
//#include "indirectcallanalysis.h"
/************************************************
	INITIALIZATION AND GENERAL FUNCTIONALITIES
*************************************************/

void Syspart::setStartFunc(Function *func)
{
	this->start_func = func;
}

void Syspart::setProgram(Program *program)
{
	this->program = program;
}

void Syspart::setConductorSetup(ConductorSetup *setup)
{
    this->setup = setup;
}
/* findFunctionByAddress() and findFunctionByName() can give inaccurate results
   if there are functions of the same address or functions of the same name respectively
*/

/* Given an address, find the function that is stored in that address */
Function* Syspart::findFunctionByAddress(address_t addr)
{
        Function *f=NULL;
        for(auto module : CIter::children(program))
        {
            for(auto func : CIter::functions(module))
            {
                    if(func->getAddress() == addr)
                    {
                        f = func;
                        break;
                    }
            }
        }
        return f;
}

/* Given a string, find the function whose name equals that string */

Function* Syspart::findFunctionByName(string fname)
{
    Function* f = NULL;
    for(auto module : CIter::children(program))
    {
        for(auto func : CIter::functions(module))
        {
                if(func->getName() == fname || func->hasName(fname))
                {
                    f = func;
                    break;
            }
        }
    }
    return f;

}

/*********************************

	SYSTEM CALL RELATED METHODS
	
**********************************/
Syspart::SysNode* Syspart::getSysNode(Function *f)
{
	for(auto s : syscall_mapping)
	{
        //cout<<(s.second)->func->getAddress()<<" "<<f->getAddress()<<endl;
		if((s.second)->func == f)
			return s.second;
        if(((s.second)->func->getAddress() == f->getAddress()) && ((s.second)->func->getName() == f->getName()))
        {
           return s.second;
        }
	}
	return NULL;
}

set<int> Syspart::getSyscalls(SysNode* sysnode)
{
    set<int> total;
    for(int i : sysnode->direct_syscalls)
        total.insert(i);
    for(int i: sysnode->derived_syscalls)
        total.insert(i);
    return total;
}

void Syspart::findDirectSyscallsOfModule(Module* m)
{
    for(auto f : CIter::functions(m))
    {

        FindSyscalls findSyscalls;
        f->accept(&findSyscalls);
        //cout<<f->getName()<<endl;
        auto list = findSyscalls.getNumberMap();
           
        for(auto kv : list)
        {
            auto syscallValues = kv.second;
            for(auto value : syscallValues)
            {
                cout<<f->getName()<<" "<<value<<"\n";
            }
        }
    }
}

void Syspart::findCallGraphOfModule(Module* m)
{
    ip_callgraph.setProgram(program); 
    for(auto f : CIter::functions(m)    )
    {
        ip_callgraph.addFunctionRoot(f);
    }
    ip_callgraph.setIcanalysis(true);

    ip_callgraph.generate();
    finiFuncs = ip_callgraph.getFiniFuncs();
    ip_callgraph.printDirectEdges();
    auto gl = ip_callgraph.getGlobalATList();
    for(auto g : gl)
    {
        cout<<g->getName()<<"\t"<<std::hex<<g->getAddress()<<"\t"<<g->getParent()->getParent()->getName()<<endl;
    }
    cout<<"#GLOBAL AT LIST : "<<std::dec<<gl.size()<<endl;
    for(auto f : ip_callgraph.nodeMap)
        {
            getSyscallInfo(f.first);
        }
    int count=0;
    cout<<"SRC_FN_NAME \t SRC_FN_ADDR \t SRC_FN_MODULE \t SRC_INSTR_ADDR \t AT_FN_NAME \t AT_FN_ADDR \t AT_FN_MODULE ";
    for(auto i : ip_callgraph.nodeMap)
    {
        auto node = i.second;
        auto atlist = node->getATList();
        for(auto at: atlist)
        {
            count++;
            auto instr = at.first;
            for(auto at_set : at.second)
            {
                auto f = at_set->getFunction();
                cout<<(node->getFunction())->getName()<<"\t"<<std::hex<<(node->getFunction())->getAddress()<<"\t"<<(node->getFunction())->getParent()->getParent()->getName()<<"\t"<<instr<<"\t"<<f->getName()<<"\t"<<f->getAddress()<<"\t"<<f->getParent()->getParent()->getName()<<endl;
            }
        }
    }
    cout<<"#GLOBAL AT LIST : "<<count<<endl;
}

void Syspart::findDirectSyscalls()
{
	for(auto i : ip_callgraph.nodeMap)
	{
		auto f = i.first;
		auto ip_node = i.second;
		SysNode *s = new SysNode;

		FindSyscalls findSyscalls;
    	f->accept(&findSyscalls);
        auto list = findSyscalls.getNumberMap();
        s->syscallMap = list;
        s->func = f;
        bitset<350> bs;   //initialized with zero;
        for(auto kv : list)
        {
            auto syscallValues = kv.second;
            for(auto value : syscallValues)
            {
                (s->direct_syscalls).insert(value);
                (s->all_syscalls).insert(value);
                bs[value]=1;
                auto iter = who_invokes_syscalls.find(value);
                if(iter == who_invokes_syscalls.end())
                {
                    vector<SysNode*> vec;
                    vec.push_back(s);
                    who_invokes_syscalls[value] = vec;
                }
                else
                {
                    auto vec = iter->second;
                    vec.push_back(s);
                    who_invokes_syscalls[value] = vec;
                }
            }
        }
        (s->syscall_info)[s] = bs;

        syscall_mapping[ip_node] = s;
    }
}

void Syspart::findDerivedSyscalls1(Function* func)
{
    bool updateFlag = false;
    int iter=0;
    int complexity=0;
    do
    {
        iter++;
        updateFlag = false;
        vector<IPCallGraphNode*> processed;
        set<IPCallGraphNode*> updated;
        queue<IPCallGraphNode*> q;
        auto startnode = ip_callgraph.getNode(func);
        q.push(startnode);
        while(!q.empty())
        {
            auto curnode = q.front();
            q.pop();
            if(iter > 1)     
            {
                if(updated.find(curnode) == updated.end())
                    continue;
            }           

            auto sys_node_iter = syscall_mapping.find(curnode);
            if(sys_node_iter != syscall_mapping.end())
            {
                auto sysnode = sys_node_iter->second;
                auto bs = (sysnode->syscall_info)[sysnode];
                auto par = curnode->getParent();
                for(auto pp : par)
                {
                    auto par_iter = syscall_mapping.find(pp);
                    SysNode *parnode;
                    if(par_iter != syscall_mapping.end())
                    {
                        parnode = par_iter->second;
                    }
                    else
                    {
                        parnode = new SysNode;
                    }
                    auto par_iter1 = (parnode->syscall_info).find(sysnode);
                    if(par_iter1 != (parnode->syscall_info).end())
                    {
                        auto par_bs = par_iter1->second;
                        if(par_bs != bs)
                        {
                            updateFlag = true;
                            (parnode->syscall_info)[sysnode] = bs;
                            (parnode->syscall_info)[parnode] = (parnode->syscall_info)[parnode] | bs;
                            updated.insert(pp);
                        }
                    }
                    else
                    {
                        updateFlag = true;
                        (parnode->syscall_info)[sysnode] = bs;
                        (parnode->syscall_info)[parnode] = (parnode->syscall_info)[parnode] | bs;
                        updated.insert(pp);
                    }
                }
            }
            auto children = curnode->getAllCallTargets();
            for(auto ch : children)
            {
                if(find(processed.begin(), processed.end(), ch) == processed.end())
                {
                    q.push(ch);
                }
            }
            processed.push_back(curnode);
            complexity++;
            //cout<<complexity<<endl;
        }
        processed.clear();
    }while(updateFlag);
}

void Syspart::findDerivedSyscalls(Function* func)
{
    int complexity=0;
    for(auto w : who_invokes_syscalls)
    {
        int sysno = w.first;
        for(auto s : w.second)
        {
            stack<tuple<IPCallGraphNode*,IPCallGraphNode*>> st;
            auto ipnode_s = ip_callgraph.getNode(s->func);
            //cout<<"PROOF "<<s->func->getName()<<" "<<system_calls[sysno]<<" "<<sysno<<endl;
            if(ipnode_s == NULL)
                continue;
            tuple<IPCallGraphNode*,IPCallGraphNode*> tup1(ipnode_s,ipnode_s);
            st.push(tup1);
            set<IPCallGraphNode*> processed;
            while(!st.empty())
            {
                auto cur_tup = st.top();
                st.pop();
                IPCallGraphNode* cur;
                IPCallGraphNode* cur_child;
                tie(cur, cur_child) = cur_tup;
                if(processed.find(cur) != processed.end())
                    continue;
                //cout<<s->func->getName()<<" "<<cur->getFunction()->getName()<<" "<<system_calls[sysno]<<endl;
                complexity++;
                //cout<<complexity<<endl;
                processed.insert(cur);
                auto pp = cur->getParent();
                for(auto p : pp)
                {
                    tuple<IPCallGraphNode*,IPCallGraphNode*> tup2(p, cur);
                    st.push(tup2);
                }
                auto sys_node_iter = syscall_mapping.find(cur);
                if(sys_node_iter == syscall_mapping.end())
                    continue;
                auto sys_node = sys_node_iter->second;
                if(cur == cur_child)   //The function which invokes it directly
                {
                    continue;
                }
                (sys_node->derived_syscalls).insert(sysno);
                auto sys_child_iter = syscall_mapping.find(cur_child);
                if(sys_child_iter == syscall_mapping.end())
                    continue;
                auto sys_child = sys_child_iter->second;
                auto bitset_it = (sys_node->syscall_info).find(sys_child);
                if(bitset_it == (sys_node->syscall_info).end())
                {
                    string bs_string(350,'0');
                    bitset<350> bs(bs_string);
                    bs.set(sysno);
                    (sys_node->syscall_info)[sys_child] = bs;
                }
                else
                {
                    auto bs = bitset_it->second;
                    bs.set(sysno);
                    (sys_node->syscall_info)[sys_child] = bs;

                }

            }
        }
    }
}

void Syspart::findDerivedSyscalls3(Function* func)
{
    bool build_flag = false;
    int build_count = 0;

    auto ip_node = ip_callgraph.getNode(func);
    if(ip_node == NULL)
    	return;

    do
    {   
        //cout<<"Building syscall tree "<<build_count<<endl;
        //build_count++;
        build_flag = false;

       	for(auto s : syscall_mapping)
    	{
    		(s.second)->color = WHITE;
    	}
      
        buildSysCallTree(ip_node, &build_flag);
       
    } while(build_flag);	

    for(auto s : syscall_mapping)
    {
        auto sysnode = s.second;
        string bs_string(350,'0');
        bitset<350> bs(bs_string);
        for(auto ss : sysnode->syscall_info)
        {
            if(sysnode != ss.first)
                bs |= ss.second;
        }
        for(int i=0; i<350; i++)
        {
            if(bs.test(i))
                sysnode->derived_syscalls.insert(i);
        }
    }

}

bitset<350> Syspart::buildSysCallTree(IPCallGraphNode* ip_node, bool *flag)
{
	auto iter = syscall_mapping.find(ip_node);
  	if(iter == syscall_mapping.end())
  		return NULL;

  	auto n = (iter)->second;
	auto children = ip_node->getAllCallTargets();

    if(n->color == GREY)    //When cycle is detected
    {
         return (n->syscall_info)[n];
    }
    else if(children.size()==0 || (n->color!=WHITE))    //Leaf edge or node already processed
    {
        return (n->syscall_info)[n];
    }
    //cout<<"Processing "<<n->func->getName()<<endl;
    n->color = GREY;        //Started processing, not complete
    //cout<<build_recursion<<n->name<<" "<<build_count<<endl;
    build_count++;
    cout<<build_count<<endl;
    bitset<350> bsFinal = (n->syscall_info)[n];
    string bs_init_string(350,'0');
    for(auto child : children)    //Loop to get the all the system calls of its children
    {
        
        auto syscallVec = buildSysCallTree(child, flag);
   
   		auto iter = syscall_mapping.find(child);
  		if(iter == syscall_mapping.end())
  			continue;
  		auto child_node = iter->second;

        auto mapIter = (n->syscall_info).find(child_node);

        
        bitset<350> bs(bs_init_string);
        if(mapIter != (n->syscall_info).end())  //contains
        {
            bs = mapIter->second;
                    //cout<<"\n Before bs : "<<bs<<endl;
        }   
             
        auto bs_result = syscallVec ^ bs;   //Checking if the syscalls of the child have changed from what we already had
        if(bs_result.any())       //Syscalls changed
        {
            *flag = true;
        }


        (n->syscall_info)[child_node] = syscallVec;
        bsFinal |= syscallVec;
       
    }
    (n->syscall_info)[n] = bsFinal;
    n->color = BLACK;
    return (n->syscall_info)[n];
}

set<int> Syspart::getSyscallsofFini()
{
    set<int> fini_syscalls;
    for(auto f : finiFuncs)
    {
        auto node = getSysNode(f);
        if(node == NULL)
            continue;
        for(auto d : node->direct_syscalls)
        {
            fini_syscalls.insert(d);
        }
        for(auto d : node->derived_syscalls)
        {
            fini_syscalls.insert(d);
        }
    }
    return fini_syscalls;
}

set<int> Syspart::getSyscallsofInit()
{
    set<int> init_syscalls;
    for(auto f : initFuncs)
    {
        auto node = getSysNode(f);
        if(node == NULL)
            continue;
        for(auto d : node->direct_syscalls)
        {
            init_syscalls.insert(d);
        }
        for(auto d : node->derived_syscalls)
        {
            init_syscalls.insert(d);
        }
    }
    return init_syscalls;
}

void Syspart::getSyscallInfo(Function *func)
{
	auto sys_node = getSysNode(func);
	if(sys_node == NULL)
		return;
    int ndirect=0, nderived=0, ntotal=0;

    set<int> f_syscalls;
    cout<<"\nTotal syscalls of "<<func->getName()<<" : "<<std::dec << (getSyscalls(sys_node)).size()<<endl;
	cout<<"\nDirect : "<<func->getName()<<" : ";
	for(auto d : sys_node->direct_syscalls)
	{
        //cout<<"0x"<<std::hex<<d<<" ";
		cout<<system_calls[d]<<" ";
        f_syscalls.insert(d);
        ndirect++;
        
	}

	cout<<"\nDerived : "<<func->getName()<<" : ";
	for(auto d : sys_node->derived_syscalls)
	{
        //cout<<"0x"<<std::hex<<d<<" ";
		cout<<system_calls[d]<<" ";
        f_syscalls.insert(d);
        nderived++;
	}
    auto tot_syscalls = getSyscalls(sys_node);
    cout<<"\nTotal : "<<func->getName()<<" : ";
    for(auto t : tot_syscalls)
    {
        cout<<system_calls[t]<<" ";
    }
    cout<<endl;
    for(auto t : tot_syscalls)
    {
        cout<<t<<",";
    }
    cout<<endl;
    set<int> fini_syscalls = getSyscallsofFini();
    cout<<"\nTotal syscalls of fini section("<<fini_syscalls.size()<<") : "<<endl;;
    for(auto d : fini_syscalls)
    {
        cout<<system_calls[d]<<" ";
        f_syscalls.insert(d);
    }
    cout<<endl;

    cout<<"\nTotal syscalls of "<<func->getName()<<" including fini section("<<f_syscalls.size()<<") : "<<endl;
    for(auto d : f_syscalls)
    {
        cout<<system_calls[d]<<" ";
        ntotal++;
    }
    cout<<endl;

    set<int> init_syscalls = getSyscallsofInit();
    cout<<"\nTotal syscalls of init section("<<init_syscalls.size()<<") : "<<endl;;
    for(auto d : init_syscalls)
    {
        cout<<system_calls[d]<<" ";
        f_syscalls.insert(d);
    }
    cout<<endl;

     cout<<"\nTotal syscalls of "<<func->getName()<<" including fini and init section("<<f_syscalls.size()<<") : "<<endl;
    for(auto d : f_syscalls)
    {
        cout<<system_calls[d]<<" ";
        ntotal++;
    }
    cout<<endl;

    cout<<"SUMMARY "<<func->getName()<<" "<<func->getParent()->getParent()->getName()<<" "<<ndirect<<" "<<nderived<<" "<<ntotal<<endl;

}

bool Syspart::findFunctionsReachable(address_t addr, Function* func, string end)
{
    Block* start_bl=NULL;

    for(auto block : CIter::children(func))
    {
        for(auto instr : CIter::children(block))
        {
            if(instr->getAddress() == addr)
            {
                start_bl = block;
            }
        }
    } 

    if(start_bl == NULL)
    {
        cout<<"Invalid start address "<<std::hex<<addr<<endl;
        return false;
    }
    stack<IPCallGraphNode*> stack_of_nodes;
    auto start_node = ip_callgraph.getNode(func);
    auto direct_ch = start_node->getDirectChildren();
    auto indirect_ch = start_node->getIndirectChildren();
    auto cfg = new ControlFlowGraph(func);
    stack<Block*> block_stack;
    auto node = cfg->get(cfg->getIDFor(start_bl));
    for(auto& link : node->forwardLinks())
    {
        auto cflink = dynamic_cast<ControlFlowLink*>(&*link);
        auto dest_id = cflink->getTargetID();
        auto dest_bl = (cfg->get(dest_id))->getBlock();
        block_stack.push(dest_bl);
    }
    vector<Block*> visited;
    bool found = false;
    while(!block_stack.empty() && !found)
    {
        Block* bl = block_stack.top();
        visited.push_back(bl);
        block_stack.pop();
        for(auto instr : CIter::children(bl))
        {
            
            auto d_iter = direct_ch.find(instr->getAddress());
            if(d_iter != direct_ch.end())
            {
                auto ipset = d_iter->second;
                for(auto ip : ipset)
                {
                    stack_of_nodes.push(ip);
		    cout<<ip->getFunction()->getName()<<" direct child of " << func->getName() <<" pushed to stack "<<endl;
                    if(ip->getFunction()->getName() == end)
                    {
			cout<<end<<" found as direct edge to "<<func->getName()<<endl;
                        return true;
                    }
                }
            }

            auto ind_iter = indirect_ch.find(instr->getAddress());
            if(ind_iter != indirect_ch.end())
            {
                auto ipset = ind_iter->second;
                for(auto ip : ipset)
                {
                    stack_of_nodes.push(ip);
		    cout<<ip->getFunction()->getName()<<" indirect child of " << func->getName() <<" pushed to stack "<<endl;
                    if(ip->getFunction()->getName() == end)
                    {
			cout<<end<<" found as indirect edge to "<<func->getName()<<endl;
                        return true;
                    }
                }
                
            }
        }

        auto node = cfg->get(cfg->getIDFor(bl));
        for(auto& link : node->forwardLinks())
        {
            auto cflink = dynamic_cast<ControlFlowLink*>(&*link);
            auto dest_id = cflink->getTargetID();
            auto dest_bl = (cfg->get(dest_id))->getBlock();
            auto visited_iter = find(visited.begin(), visited.end(), dest_bl);
            if(visited_iter == visited.end())
                block_stack.push(dest_bl);
        }
    }
    stack_of_nodes_visited_nodes.push_back(start_node);
    while(!stack_of_nodes.empty())
    {
        auto cur_node = stack_of_nodes.top();
	    //cout<<"Visiting "<<cur_node->getFunction()->getName()<<endl;
        stack_of_nodes.pop();
        auto is_visited = find(stack_of_nodes_visited_nodes.begin(), stack_of_nodes_visited_nodes.end(), cur_node);
        if(is_visited != stack_of_nodes_visited_nodes.end())
            continue;
        stack_of_nodes_visited_nodes.push_back(cur_node);
        auto children = cur_node->getAllCallTargets();
        for(auto ch : children)
        {
            if(ch->getFunction()->getName() == end)
	       {
		      cout<<ch->getFunction()->getName()<<" found as child of "<<cur_node->getFunction()->getName()<<endl;
              return true;
	       }
        stack_of_nodes.push(ch);
	    cout<<ch->getFunction()->getName()<<" child of " << cur_node->getFunction()->getName() <<" pushed to stack "<<endl;
        }
    }

}

void Syspart::isFunctionReachable(string address, string start, string end, bool icanalysisFlag, bool typearmorFlag)
{
    address_t addr = (address_t)stol(address, NULL, 16);
    ip_callgraph.setProgram(program);
    ip_callgraph.setRoot(start_func);
    ip_callgraph.resolveNss(setup);
    ip_callgraph.setIcanalysis(icanalysisFlag);
    ip_callgraph.setTypeArmor(typearmorFlag);
    ip_callgraph.generate();
    finiFuncs = ip_callgraph.getFiniFuncs();
    ip_callgraph.addNssEdges();

    auto fstart = findFunctionByName(start);
    auto fend = findFunctionByName(end);
    auto start_node = ip_callgraph.getNode(fstart);
    auto end_node = ip_callgraph.getNode(fend);
    if(start_node == NULL)
    {
        cout<<"Node for "<<start<<" doesn't exist"<<endl;
        return;
    }
    if(end_node == NULL)
    {
        cout<<"Node for "<<end<<" doesn't exist"<<endl;
        return;
    } 

    stack<tuple<IPCallGraphNode*,address_t>> st;
    tuple<IPCallGraphNode*,address_t> tup1(start_node, addr);
    st.push(tup1);
    vector<tuple<IPCallGraphNode*,address_t>> visited;
    auto thread_start_fns = getThreadStartFunction();
    bool isReachable;
    while(!st.empty())
    {
        auto tup1 = st.top();
        st.pop();
        IPCallGraphNode* ipnode_found;
        address_t call_addr;
        tie(ipnode_found, call_addr) = tup1;
        auto cur_func = ipnode_found->getFunction();
        bool flag = false;
        for(auto v : visited)
        {
            IPCallGraphNode* ipnode_1;
            address_t call_addr_1;
            tie(ipnode_1, call_addr_1) = v;
            if(ipnode_found == ipnode_1 && call_addr == call_addr_1)
                flag = true;
        }
        if(flag)
            continue;
        if(cur_func == NULL)
        {
            cout<<"No function found by name "<<cur_func->getName()<<endl;
            return;
        }
	cout<<std::hex<<call_addr<<" "<<cur_func->getName()<<endl;
        isReachable  = findFunctionsReachable(call_addr, cur_func, end);
        if(isReachable)
        {
            cout<<end<<" is reachable from "<<start<<endl;
            break;
        }
        //cout<<endl;
        visited.push_back(tup1);

        if(!cur_func->returns())
            continue;
        if(thread_start_fns.count(cur_func) != 0)
            continue;
        auto p = ipnode_found->getParentCallSites();
        //cout<<"Parents of "<<cur_func->getName()<<" : ";
        for(auto pp : p)
        {
            //cout<<(pp.second)->getFunction()->getName()<<" ("<<std::hex<<pp.first<<")\t";
            tuple<IPCallGraphNode*, address_t> tup2(pp.second, pp.first);
            st.push(tup2);
        }
    }
    if(!isReachable)
        cout<<end<<" is not reachable from "<<start<<endl;
    
}

set<int> Syspart::findSyscallsAccessible(address_t addr, Function* f)
{
    Block* start_bl=NULL;
    set<int> allsyscalls;
    //cout<<"Function : "<<f->getName()<<" " << std::hex<< addr << endl;
    for(auto block : CIter::children(f))
    {
        for(auto instr : CIter::children(block))
        {
            if(instr->getAddress() == addr)
            {
                start_bl = block;
            }
        }
    }
    if(start_bl == NULL)
        return allsyscalls;
    auto sysnode = getSysNode(f);
    //cout<<endl;
    //cout<<"Finding ipnode of func "<<f->getName()<<endl;
    auto ipnode_found = ip_callgraph.getNode(f);
    if(ipnode_found == NULL)
    {
        cout<<"Sorry no ipnode found"<<endl;
    }
    auto direct_ch = ipnode_found->getDirectChildren();
    auto indirect_ch = ipnode_found->getIndirectChildren();
    auto cfg = new ControlFlowGraph(f);
    stack<Block*> block_stack;

    auto node = cfg->get(cfg->getIDFor(start_bl));
    for(auto& link : node->forwardLinks())
    {
        auto cflink = dynamic_cast<ControlFlowLink*>(&*link);
        auto dest_id = cflink->getTargetID();
        auto dest_bl = (cfg->get(dest_id))->getBlock();
        block_stack.push(dest_bl);
    }
    vector<Block*> visited;
    
    while(!block_stack.empty())
    {
        Block* bl = block_stack.top();
        visited.push_back(bl);
        block_stack.pop();
        //cout<<bl->getName()<<" ";
        for(auto instr : CIter::children(bl))
        {
            partitionSize++;
            auto map_iter = (sysnode->syscallMap).find(instr);
            if(map_iter != (sysnode->syscallMap).end())
            {
                for(auto i :(map_iter)->second)
                {
                    allsyscalls.insert(i);
                    //cout<<"I "<<std::hex<<instr->getAddress()<<" "<<system_calls[i]<<endl;
                }
            }

            auto d_iter = direct_ch.find(instr->getAddress());
            if(d_iter != direct_ch.end())
            {
                auto ipset = d_iter->second;
                for(auto ip : ipset)
                {
                    auto sy = syscall_mapping.find(ip);
                    if(sy == syscall_mapping.end())
                        continue;
                    auto sy_set = getSyscalls(sy->second);
                    allsyscalls.insert(sy_set.begin(), sy_set.end());
                    partitionFns.insert(ip->getFunction());
                   
                }
            }

            auto ind_iter = indirect_ch.find(instr->getAddress());
            if(ind_iter != indirect_ch.end())
            {
                auto ipset = ind_iter->second;
                for(auto ip : ipset)
                {
                    auto sy = syscall_mapping.find(ip);
                    if(sy == syscall_mapping.end())
                        continue;
                    auto sy_set = getSyscalls(sy->second);
                    allsyscalls.insert(sy_set.begin(), sy_set.end());
                    partitionFns.insert(ip->getFunction());

                }
            }
        }
        //cout<<endl;
        auto node = cfg->get(cfg->getIDFor(bl));
        for(auto& link : node->forwardLinks())
        {
            auto cflink = dynamic_cast<ControlFlowLink*>(&*link);
            auto dest_id = cflink->getTargetID();
            auto dest_bl = (cfg->get(dest_id))->getBlock();
            auto visited_iter = find(visited.begin(), visited.end(), dest_bl);
            if(visited_iter == visited.end())
                block_stack.push(dest_bl);
        }
    }
    return allsyscalls;
}



/********************************************************

	METHODS THAT IMPLEMENT EXPERIMENTAL ANALYSIS

*********************************************************/


Function* Syspart::findRegDef(UDState* state, int reg, address_t &thread_func_addr)
{
    for(auto& s : state->getRegRef(reg)) 
    {
        if(auto def = s->getRegDef(reg)) 
        {
            //R0:  0 nodeType=5
            typedef TreePatternCapture<TreePatternTerminal<TreeNodeConstant>>ConstantValue;
            //R6:  %R0 nodeType=6
            typedef TreePatternCapture<TreePatternTerminal<TreeNodePhysicalRegister>>RegisterValue;
            //R0:  (+ %rip=0x749 -101) nodeType=7
            typedef TreePatternBinary<TreeNodeAddition,
            TreePatternCapture<TreePatternRegisterIs<X86_REG_RIP>>,
            TreePatternCapture<TreePatternTerminal<TreeNodeConstant>>> RIPValue;
            //R0:  (deref (+ %rip=0x749 -101)) nodeType=7
            typedef TreePatternUnary<TreeNodeDereference,
            TreePatternCapture<TreePatternBinary<TreeNodeAddition,
                    TreePatternCapture<TreePatternRegisterIs<X86_REG_RIP>>,
                    TreePatternCapture<TreePatternTerminal<TreeNodeConstant>>
                >>> RIPDerefValue;
            TreeCapture cap;
            auto state_instr = s->getInstruction();
            //cout<<"RDX defined in " << std::hex << state_instr->getAddress() <<endl;  
            
            if(auto li = dynamic_cast<LinkedInstruction *>(state_instr->getSemantic()))
            {               
                auto link = li->getLink();
                auto target = link->getTarget();
                if(auto func_target = dynamic_cast<Function *> (target))    
                {
                    //cout<<"Found Thread start function : "<<func_target->getName()<<endl;
                    thread_func_addr = func_target->getAddress();
                    return func_target;
                }
            }
                                   
            if(ConstantValue::matches(def, cap)) {
                    auto const_val = dynamic_cast<TreeNodeConstant *>(cap.get(0))->getValue();
                    //cout<<"\nConstant value encountered"<<const_val<<endl;
                    thread_func_addr = const_val;
                    return NULL;
            }
            else if(RegisterValue::matches(def, cap)) {
                auto reg1 = dynamic_cast<TreeNodePhysicalRegister *>(cap.get(0))->getRegister();
                return(findRegDef(s, reg1,thread_func_addr));
            }
            else if(RIPValue::matches(def, cap)) {             
                auto ripTree = dynamic_cast<TreeNodeRegisterRIP *>(cap.get(0));
                auto dispTree = dynamic_cast<TreeNodeConstant *>(cap.get(1));
                address_t addr = dispTree->getValue() + ripTree->getValue();
                thread_func_addr = addr;
                return NULL;
            }
            else if(RIPDerefValue::matches(def, cap)) 
            {
                TreeCapture cap1;
                if(RIPValue::matches(cap.get(0), cap1)) 
                {
                    auto ripTree = dynamic_cast<TreeNodeRegisterRIP *>(cap1.get(0));
                    auto dispTree = dynamic_cast<TreeNodeConstant *>(cap1.get(1));
                    address_t addr = dispTree->getValue() + ripTree->getValue();
                    thread_func_addr = addr;
                    return NULL;
                }
            }
            
        
}    }
    return NULL;
}
Function* Syspart::find_thread_function(Function* f, address_t addr, address_t &thread_func_addr)
{
    auto graph = new ControlFlowGraph(f);
    auto config = new UDConfiguration(graph);
    auto working = new UDRegMemWorkingSet(f, graph);
    auto usedef =  new UseDef(config, working);
    SccOrder order(graph);
    order.genFull(0);
    usedef->analyze(order.get());
    //TODO : Is there a function to directly get Instruction object, given the address?

    for(auto bl : CIter::children(f))
    {
        for(auto instr : CIter::children(bl))
        {
            //cout<<"\n"<<std::hex<<instr->getAddress()<<"\t"<<addr<<"\n";
            if(instr->getAddress() == addr)
            {
               //cout<<"\nReached here in "<<f->getName()<<"\n";
               InstrDumper instrDumper(instr->getAddress(), INT_MIN);
               auto state = working->getState(instr);
               auto reg = X86Register::convertToPhysical(X86_REG_RDX);
               return(findRegDef(state,reg,thread_func_addr));


            }
        }
    }
    return NULL;
}

set<Function*> Syspart::getThreadStartFunction()
{
    Function* found=NULL;
    set<Function*> found_funcs;
    for(auto i : ip_callgraph.nodeMap)
    {
        auto node = i.second;
        auto f = i.first;
        auto direct_ch = node->getDirectChildren();
        for(auto d : direct_ch)
        {
            auto addr = d.first;
            auto children = d.second;
            for(auto ch : children)
            {
                auto t_name = ch->getFunction()->getName();
                if(t_name.find("pthread_create") != string::npos)
                {
                    //cout<<"\nDirect at "<<std::hex<<addr<<endl;
                    address_t thread_func_addr;
                    found = find_thread_function(f, addr,thread_func_addr);
                    if(found == NULL)
                    {
                        found = findFunctionByAddress(thread_func_addr);
                        if(found != NULL)
                        {

                            found_funcs.insert(found);
                        }
                    }
                    else
                    {

                        found_funcs.insert(found);

                    }
                    break;
                }
            }

        }
        auto indirect_ch = node->getIndirectChildren();
        for(auto d : indirect_ch)
        {
            auto addr = d.first;
            auto children = d.second;
            for(auto ch : children)
            {
                auto t_name = ch->getFunction()->getName();
                if(t_name.find("pthread_create") != string::npos)
                {   
                    //cout<<"\nIndirect at "<<std::hex<<addr<<endl;
                    address_t thread_func_addr;
                    found = find_thread_function(f, addr,thread_func_addr);
                    if(found == NULL)
                    {
                        found = findFunctionByAddress(thread_func_addr);
                        if(found != NULL)
                        {
                            //cout<<"\nThread start function is : "<<found->getName()<<endl;
                            found_funcs.insert(found);
                        }
                    }
                    else
                    {
                        //cout<<"\nThread start function found : "<<found->getName()<<endl;
                        found_funcs.insert(found);

                    }
                    break;
                }
            }

        }
    }
    return found_funcs;
}
//Method 
void Syspart::find_syscalls_in_thread(bool direct, bool icanalysisFlag, bool typearmorFlag)
{
    ip_callgraph.setProgram(program);
    ip_callgraph.setRoot(start_func);
    ip_callgraph.resolveNss(setup);
    ip_callgraph.setIcanalysis(icanalysisFlag);
    ip_callgraph.setTypeArmor(typearmorFlag);
    if(direct)
        ip_callgraph.generateDirectCallGraph();
    else
       ip_callgraph.generate();
    finiFuncs = ip_callgraph.getFiniFuncs();
    initFuncs = ip_callgraph.getInitFuncs();
    ip_callgraph.addNssEdges();
    findDirectSyscalls();
    findDerivedSyscalls(start_func);
    set<Function*> found_funcs = getThreadStartFunction();


    for(auto f : found_funcs)
    {
        //cout<<"\n Syscalls of function : "<<f->getName()<<endl;
        getSyscallInfo(f);
        char ad[50];
        cout<<"\n Enter loop address of function "<<f->getName()<<endl;
        cin >> ad;
        if(strcmp("n",ad) == 0)
            continue;
        address_t addr = (address_t)strtol(ad, NULL, 16); 
        LoopAnalysis la;
        la.detectLoops(f);
        auto cur_loop = la.getLoop(addr, (Module*)f->getParent()->getParent());
        if(cur_loop == NULL)
        {
            cout<<"\n No loop found with the provided info";
            continue;
        }
        auto loop_body = cur_loop->loopBody;
        auto sysnode_found = getSysNode(f);
        auto ipnode_found = ip_callgraph.getNode(f);
        auto direct_ch = ipnode_found->getDirectChildren();
        auto indirect_ch = ipnode_found->getIndirectChildren();
        set<int> loop_syscalls;
        for(auto b : loop_body)
        {
            for(auto instr : CIter::children(b))
            {
                auto iter = ((sysnode_found)->syscallMap).find(instr);
                if(iter != ((sysnode_found)->syscallMap).end())
                {
                    for(auto i : iter->second)
                    {
                        loop_syscalls.insert(i);
                    }
                }
                auto d_iter = direct_ch.find(instr->getAddress());
                if(d_iter != direct_ch.end())
                {
                    auto ipset = d_iter->second;
                    for(auto ip : ipset)
                    {
                        auto sy = syscall_mapping.find(ip);
                        if(sy == syscall_mapping.end())
                            continue;
                        auto sy_set = getSyscalls(sy->second);
                        loop_syscalls.insert(sy_set.begin(), sy_set.end());
                    }
                }

                auto ind_iter = indirect_ch.find(instr->getAddress());
                if(ind_iter != indirect_ch.end())
                {
                    auto ipset = ind_iter->second;
                    for(auto ip : ipset)
                    {
                        auto sy = syscall_mapping.find(ip);
                        if(sy == syscall_mapping.end())
                            continue;
                        auto sy_set = getSyscalls(sy->second);
                        loop_syscalls.insert(sy_set.begin(), sy_set.end());
                    }
                }

            }
        }

        cout<<"\n Syscalls of loop starting at : "<<std::hex<<(cur_loop)->entry->getAddress()<<" "<<std::dec<<loop_syscalls.size()<<endl;

        for(auto i : loop_syscalls)
        {
            cout<<system_calls[i]<<" ";
        }
    }

}

int Syspart::isNonReturn(ControlFlowGraph *cfg, Block* bl, set<Block*> visited)
{
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
            if(!cfi->returns())   //Non-returning
            {
                //cout<<std::hex <<bl->getAddress()<<" returning 1 B"<<endl;
                return 1;
            }
        }
    }
    int ret = 1;
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
        ret = ret & t;
    }
    if(endNode)
        ret = 0;
    noreturn_done[bl] = ret;
    return ret;
}

//Reachability Analysis
void Syspart::findReachableCode(bool direct, bool icanalysisFlag, bool typearmorFlag, address_t addr, string func_name)
{
    ip_callgraph.setProgram(program);
    ip_callgraph.setRoot(start_func);
    ip_callgraph.resolveNss(setup);
    ip_callgraph.setIcanalysis(icanalysisFlag);
    ip_callgraph.setTypeArmor(typearmorFlag);
    if(direct)
        ip_callgraph.generateDirectCallGraph();
    else
       ip_callgraph.generate();
   ip_callgraph.addNssEdges();
   finiFuncs = ip_callgraph.getFiniFuncs();
   initFuncs = ip_callgraph.getInitFuncs();
    //findDirectSyscalls();
    //findDerivedSyscalls(start_func);
    for(auto i : ip_callgraph.nodeMap)
    {
       auto f = i.first;
       if(!f->returns())
           cout<<f->getName()<<" is a no-return function"<<endl;
    }
    auto func = findFunctionByName(func_name);
    if(func == NULL)
    {
        cout<<"Function not found"<<endl;
        return;
    }
   
   Block* start_bl=NULL;
   for(auto block : CIter::children(func))
   {
        if(block->getAddress() == addr)
        {
            start_bl = block;
        }
   }
   if(start_bl == NULL)
        return;
    set<Block*> visited;
    noreturn_done.clear();
    auto graph = new ControlFlowGraph(func);
    int ret = isNonReturn(graph, start_bl, visited);
    if(ret == 1)
    {
        cout<<"\n Is non-returning \n";
    }
    else if(ret == 0)
    {
        cout<<"\nReturns \n";
    }
    else if(ret == -1)
    {
        cout<<"\nLoops\n";
    }                        
}

/***** FINDS ALL FUNCTIONS ACCESSIBLE FROM START_FUNC THAT DIRECTLY INVOKES SYSCALLS ***/
void Syspart::getDirectSyscallsFromStart()
{
	queue<Function*> q;
	set<Function*> processed;
	q.push(start_func);
	processed.insert(start_func);

	while(!q.empty())
	{
		auto f = q.front();
		q.pop();

		auto ip_node = ip_callgraph.getNode(f);
		auto iter = syscall_mapping.find(ip_node);
  		if(iter == syscall_mapping.end())
  			continue;
		auto sys_node = iter->second;

		auto direct = (sys_node)->direct_syscalls;
		//cout<<"FUNC "<<f->getName()<<" "<<std::hex<<f->getAddress()<<" "<<f<<endl;	
		if(direct.size() > 0)
		{
			for(auto d : direct)
			{
				cout<<"\n"<<f->getName()<<" "<<std::hex<<f->getAddress()<<" "<<f<<" ("<<f->getParent()->getParent()->getName()<<") : "<<system_calls[d]<<" ";
			}
		}
		auto children = ip_node->getAllCallTargets();
		for(auto ch : children)
		{
			auto child_func = ch->getFunction();
			if(processed.find(child_func) == processed.end())
			{
				q.push(child_func);
				processed.insert(child_func);

			}
		}
        getDirectSyscalls();
	}
}

void Syspart::getDirectSyscalls()
{
    for(auto d : who_invokes_syscalls)
    {
        auto syscallNo = d.first;
        auto vec = d.second;
        for(auto v : vec)
            cout<<v->func->getName()<<" "<<v->func->getAddress()<<" "<<v->func<<" : "<<system_calls[syscallNo]<<endl;
    }

}

/**** PRINT CALLGRAPH ********/
void Syspart::run1(bool direct, bool icanalysisFlag, bool typearmorFlag)
{
    ip_callgraph.setProgram(program);
    ip_callgraph.setRoot(start_func);
    ip_callgraph.resolveNss(setup);
    ip_callgraph.setIcanalysis(icanalysisFlag);
    ip_callgraph.setTypeArmor(typearmorFlag);
    if(typearmorFlag)
        ip_callgraph.setTypeArmorPath(typearmorPath);
    if(direct)
        ip_callgraph.generateDirectCallGraph();
    else
       ip_callgraph.generate();
    ip_callgraph.addNssEdges();
    finiFuncs = ip_callgraph.getFiniFuncs();
    initFuncs = ip_callgraph.getInitFuncs();
    ip_callgraph.printCallGraph();
    //ip_callgraph.printCallGraphofApplication();
    //ip_callgraph.printDirectEdges();
      
}

/****** PRINTS SYSCALLINFO OF a specific function or ALL FUNCTIONS ****/
void Syspart::run2(bool direct, bool icanalysisFlag, bool typearmorFlag, string func_name)
{
    ip_callgraph.setProgram(program);
    ip_callgraph.setRoot(start_func);
    ip_callgraph.resolveNss(setup);
    ip_callgraph.setIcanalysis(icanalysisFlag);
    ip_callgraph.setTypeArmor(typearmorFlag);
    if(direct)
        ip_callgraph.generateDirectCallGraph();
    else
       ip_callgraph.generate();
    finiFuncs = ip_callgraph.getFiniFuncs();
    initFuncs = ip_callgraph.getInitFuncs();
    ip_callgraph.addNssEdges();
  
    findDirectSyscalls();
    findDerivedSyscalls(start_func);
    //cout<<"Syscall generation DONE"<<endl;
    
    if(func_name != "*")
    {
        auto f = findFunctionByName(func_name);
        if(f != NULL)
        {
	    
            //getSyscallInfo(f);
            auto sys_node = getSysNode(f);
            if(sys_node == NULL)
                    cout<<"No system calls generated for "<<f->getName()<<endl;
            auto tot_syscalls = getSyscalls(sys_node);
	    cout<<"SYSCALLS [";
            for(auto t : tot_syscalls)
                {
                        cout<<system_calls[t]<<",";
                }
	    cout<<"]"<<endl;
	    cout<<"SIZE "<<tot_syscalls.size()<<endl;
        }
    }
    else
    {
        for(auto f : ip_callgraph.nodeMap)
        {
            getSyscallInfo(f.first);
        }
    }
}

/******* PRINTS FUNCTIONS REACHABLE FROM STARTFUNC THAT INVOKE SYSCALLS DIRECTLY ********/

void Syspart::run3(bool direct, bool icanalysisFlag, bool typearmorFlag)
{
    ip_callgraph.setProgram(program);
    ip_callgraph.setRoot(start_func);
    ip_callgraph.resolveNss(setup);

    ip_callgraph.setIcanalysis(icanalysisFlag);
    ip_callgraph.setTypeArmor(typearmorFlag);
    if(direct)
        ip_callgraph.generateDirectCallGraph();
    else
       ip_callgraph.generate();
    finiFuncs = ip_callgraph.getFiniFuncs();
    initFuncs = ip_callgraph.getInitFuncs();
    ip_callgraph.addNssEdges();
    findDirectSyscalls();
    findDerivedSyscalls(start_func); 
    getDirectSyscallsFromStart();
    getDirectSyscalls();
    
}

/*********** FUNCTION THAT PRINTS THE CHILD/CHILDREN FROM WHICH IT DERIVES A SPECIFIC SYSTEM CALL 
            OR IN CASE OF SYSTEM CALLS IT DIRECTLY INVOKES, PRINTS THAT ***********/
void Syspart::run10(bool direct, bool icanalysisFlag, bool typearmorFlag, string sys_name)
{
    ip_callgraph.setProgram(program);
    ip_callgraph.setRoot(start_func);
    ip_callgraph.resolveNss(setup);

    ip_callgraph.setIcanalysis(icanalysisFlag);
    ip_callgraph.setTypeArmor(typearmorFlag);
    if(direct)
        ip_callgraph.generateDirectCallGraph();
    else
       ip_callgraph.generate();
    finiFuncs = ip_callgraph.getFiniFuncs();
    initFuncs = ip_callgraph.getInitFuncs();
    ip_callgraph.addNssEdges();
    findDirectSyscalls();
    findDerivedSyscalls(start_func);

    int sysint;
    bool flag = false;
    for(auto i : system_calls)
    {
        if(i.second == sys_name)
        {
            sysint = i.first;
            flag = true;
            break;
        }
    }

    if(!flag)
    {
        cout<<"System call with name "<<sys_name<<" not found"<<endl;
        return;
    }
    for(auto s : syscall_mapping)
    {
        auto f = s.first;
        auto ss = s.second;
        for(auto i : ss->syscall_info)
        {
            auto b = i.second;
            if(b.test(sysint))
            {
                cout<<"\n"<<f->getFunction()->getName()<<"->"<<(i.first)->func->getName()<<endl;
               
            }
        }
    }
}

/*********** PRINTS GLOBAL AT LIST **************/
void Syspart::run4(bool direct, bool icanalysisFlag, bool typearmorFlag)
{
    ip_callgraph.setProgram(program);
    ip_callgraph.setRoot(start_func);
    ip_callgraph.resolveNss(setup);
    ip_callgraph.setIcanalysis(icanalysisFlag);
    ip_callgraph.setTypeArmor(typearmorFlag);
    if(direct)
        ip_callgraph.generateDirectCallGraph();
    else
       ip_callgraph.generate();
    ip_callgraph.addNssEdges();
    finiFuncs = ip_callgraph.getFiniFuncs();
    initFuncs = ip_callgraph.getInitFuncs();
    auto gl = ip_callgraph.getGlobalATList();
    for(auto g : gl)
    {
        cout<<g->getName()<<"\t"<<std::hex<<g->getAddress()<<"\t"<<g->getParent()->getParent()->getName()<<endl;
    }
    cout<<"#GLOBAL AT LIST : "<<std::dec<<gl.size()<<endl;
}

/******* INFO ABOUT AT FUNCTIONS AND WHERE IT IS ADDRESS TAKEN ***********/
void Syspart::run5(bool direct, bool icanalysisFlag, bool typearmorFlag)
{
    ip_callgraph.setProgram(program);
    ip_callgraph.setRoot(start_func);
    ip_callgraph.resolveNss(setup);
    ip_callgraph.setIcanalysis(icanalysisFlag);
    ip_callgraph.setTypeArmor(typearmorFlag);
    if(direct)
        ip_callgraph.generateDirectCallGraph();
    else
       ip_callgraph.generate();

    finiFuncs = ip_callgraph.getFiniFuncs();
    initFuncs = ip_callgraph.getInitFuncs();
    ip_callgraph.addNssEdges();

    int count=0;
    cout<<"SRC_FN_NAME \t SRC_FN_ADDR \t SRC_FN_MODULE \t SRC_INSTR_ADDR \t AT_FN_NAME \t AT_FN_ADDR \t AT_FN_MODULE ";
    for(auto i : ip_callgraph.nodeMap)
    {
        auto node = i.second;
        auto atlist = node->getATList();
        for(auto at: atlist)
        {
            count++;
            auto instr = at.first;
            for(auto at_set : at.second)
            {
                auto f = at_set->getFunction();
                cout<<(node->getFunction())->getName()<<"\t"<<std::hex<<(node->getFunction())->getAddress()<<"\t"<<(node->getFunction())->getParent()->getParent()->getName()<<"\t"<<instr<<"\t"<<f->getName()<<"\t"<<f->getAddress()<<"\t"<<f->getParent()->getParent()->getName()<<endl;
            }
        }
    }
    cout<<"#GLOBAL AT LIST : "<<count<<endl;
    
}

//Method that prints if fork() and pcreate() methods are invoked in the application
void Syspart::run6(bool direct, bool icanalysisFlag, bool typearmorFlag)
{
    ip_callgraph.setProgram(program);
    ip_callgraph.setRoot(start_func);
    ip_callgraph.resolveNss(setup);
    ip_callgraph.setIcanalysis(icanalysisFlag);
    ip_callgraph.setTypeArmor(typearmorFlag);
    if(direct)
        ip_callgraph.generateDirectCallGraph();
    else
       ip_callgraph.generate();
    finiFuncs = ip_callgraph.getFiniFuncs();
    initFuncs = ip_callgraph.getInitFuncs();
    ip_callgraph.addNssEdges();
    for(auto i : ip_callgraph.nodeMap)
    {
        auto node = i.second;
        auto f = i.first;
        auto mod = (Module*)f->getParent()->getParent();
        //if(mod->getName() != "module-(executable)")
        //    continue;
        auto targets = node->getAllCallTargets();
        for(auto t : targets)
        {
            auto t_name = t->getFunction()->getName();
            //if(t_name.find("fork") != string::npos)
            if(t_name == "fork" || t_name == "fork_alias")
                cout<<"\n"<<t_name<<" is invoked in "<<f->getName()<<" " <<mod->getName()<<endl;
            else if(t_name.find("pthread_create") != string::npos)
                cout<<"\n"<<t_name<<" is invoked in "<<f->getName()<<" " <<mod->getName()<<endl;
            else if(t_name.find("pthread_exit") != string::npos)
                cout<<"\n"<<t_name<<" is invoked in "<<f->getName()<<" " <<mod->getName()<<endl;
            else if(t_name.find("pthread_cancel") != string::npos)
                cout<<"\n"<<t_name<<" is invoked in "<<f->getName()<<" " <<mod->getName()<<endl;


        }

    }
    auto thread_start_fns = getThreadStartFunction();
    cout<<"Thread start functions : "<<endl;
    for(auto th : thread_start_fns)
    {
        cout<<th->getName()<<" "<<th->getAddress()<<endl;
    }

}

/********* Function that find the difference between syscalls accessible from startfunc 
           and the syscalls accessible from function named 'fname' which is passed as 
           argument to the function ***********/
void Syspart::run7(bool direct, bool icanalysisFlag, bool typearmorFlag, string fname)
{
    ip_callgraph.setProgram(program);
    ip_callgraph.setRoot(start_func);
    ip_callgraph.resolveNss(setup);
    ip_callgraph.setIcanalysis(icanalysisFlag);
    ip_callgraph.setTypeArmor(typearmorFlag);
    if(direct)
        ip_callgraph.generateDirectCallGraph();
    else
       ip_callgraph.generate();
    finiFuncs = ip_callgraph.getFiniFuncs();
    initFuncs = ip_callgraph.getInitFuncs();
    ip_callgraph.addNssEdges();
    findDirectSyscalls();
    findDerivedSyscalls(start_func);
    getSyscallInfo(start_func);

    auto main_loop_func = findFunctionByName(fname);
    if(main_loop_func == NULL)
        return;
    getSyscallInfo(main_loop_func);
    auto ip1 = ip_callgraph.getNode(start_func);
    if(ip1 == NULL)
        return;
    auto s1 = syscall_mapping.find(ip1 );
    if(s1 == syscall_mapping.end())
        return;
    auto all = getSyscalls(s1->second);

    auto ip2 = ip_callgraph.getNode(main_loop_func);
    if(ip2 == NULL)
        return;
    auto s2 = syscall_mapping.find(ip2);
    if(s2 == syscall_mapping.end())
        return;
    auto mainloop = getSyscalls(s2->second);
    set<int> filtered;
    set_difference(all.begin(), all.end(), mainloop.begin(), mainloop.end(), std::inserter(filtered, filtered.begin()));
    cout<<"\nFiltered system calls : ";
    for(auto i : filtered)
        cout<<system_calls[i]<<" ";
    cout<<"\nTotal system calls of function : "<<mainloop.size()<<endl;
}

void Syspart::getPartitionSize(bool direct, bool icanalysisFlag, bool typearmorFlag, string fname)
{
    ip_callgraph.setProgram(program);
    ip_callgraph.setRoot(start_func);
    ip_callgraph.resolveNss(setup);
    ip_callgraph.setIcanalysis(icanalysisFlag);
    ip_callgraph.setTypeArmor(typearmorFlag);
    if(direct)
        ip_callgraph.generateDirectCallGraph();
    else
       ip_callgraph.generate();
    finiFuncs = ip_callgraph.getFiniFuncs();
    initFuncs = ip_callgraph.getInitFuncs();
    ip_callgraph.addNssEdges();
    auto f = findFunctionByName(fname);
    stack<Function*> fStack;
    set<Function*> visitedPartitionFns;
    fStack.push(f);
    int partitionSize=0;
    while(!fStack.empty())
    {
        auto fun = fStack.top();
        fStack.pop();
        if(visitedPartitionFns.find(fun) != visitedPartitionFns.end())
            continue;
        visitedPartitionFns.insert(fun);

        auto ip_node = ip_callgraph.getNode(fun);
        if(ip_node == NULL)
            continue;
        auto direct_ch = ip_node->getDirectChildren();
        auto indirect_ch = ip_node->getIndirectChildren();
        for(auto bl : CIter::children(fun))
        {
            for(auto instr : CIter::children(bl))
            {
                partitionSize++;

                auto d_iter = direct_ch.find(instr->getAddress());
                if(d_iter != direct_ch.end())
                {
                    auto ipset = d_iter->second;
                    for(auto ip : ipset)
                    {
                        if(visitedPartitionFns.find(ip->getFunction()) == visitedPartitionFns.end())
                            fStack.push(ip->getFunction());
                    }
                }
                auto ind_iter = indirect_ch.find(instr->getAddress());
                if(ind_iter != indirect_ch.end())
                {
                    auto ipset = ind_iter->second;
                    for(auto ip : ipset)
                    {
                        if(visitedPartitionFns.find(ip->getFunction()) == visitedPartitionFns.end())
                            fStack.push(ip->getFunction());
                    }
                }
            }
        }
    }
    cout<<"Partition size of function "<<fname<<" "<<partitionSize<<endl;
}

void Syspart::ifPathExists(string start,string end, bool icanalysisFlag, bool typearmorFlag)
{
    ip_callgraph.setProgram(program);
    ip_callgraph.setRoot(start_func);
    ip_callgraph.resolveNss(setup);

    ip_callgraph.setIcanalysis(icanalysisFlag);
    ip_callgraph.setTypeArmor(typearmorFlag);
    ip_callgraph.generate();
    finiFuncs = ip_callgraph.getFiniFuncs();
    initFuncs = ip_callgraph.getInitFuncs();
    ip_callgraph.addNssEdges();

    auto start_func = findFunctionByName(start);
    auto end_func = findFunctionByName(end);
    auto start_node = ip_callgraph.getNode(start_func);
    auto end_node = ip_callgraph.getNode(end_func);
    if(start_node == NULL)
    {
        cout<<"Node for "<<start<<" doesn't exist"<<endl;
        return;
    }
    if(end_node == NULL)
    {
        cout<<"Node for "<<end<<" doesn't exist"<<endl;
        return;
    }
    stack<IPCallGraphNode*> st;
    st.push(start_node);
    set<IPCallGraphNode*> visited;
    bool flag = false;
    while(!st.empty() && !flag)
    {
        auto cur = st.top();
        st.pop();
        if(visited.count(cur) != 0)
            continue;
        auto children = cur->getAllCallTargets();
        for(auto ch : children)
        {
            if(ch->getFunction()->getName() == end)
            {
                cout<<end<<" is reachable from "<<start<<endl;
                flag = true;
                break;
            }
            st.push(ch);
        }
        visited.insert(cur);
    }
    if(!flag)
        cout<<"NOT REACHABLE"<<endl;
}

void Syspart::getCallPath(IPCallGraphNode* n, int depth, set<IPCallGraphNode*> *visited, set<Function*> atlist)
{
    if(visited->count(n) != 0)
        return;
    visited->insert(n);
    if(n->hasIndirectCall())
    {
        cout<<"Indirect call at "<<n->getFunction()->getName()<<endl;
        //return;
    }

    if(atlist.count(n->getFunction()) != 0)
    {
        cout<<"AT "<<n->getFunction()->getName()<<endl;
        //return;
    }
    auto parents = n->getParentWithType();
    for(auto p : parents)
    {
        if(p.second)
        {
            address_t p_addr;
            IPCallGraphNode* parent_node;
            tie(p_addr, parent_node) = p.first;
            for(int i=0; i<depth; i++)
            {
                cout<<"   ";
            }
            cout<<n->getFunction()->getName()<<" "<<n->getFunction()->getParent()->getParent()->getName()<<" <- "<<parent_node->getFunction()->getName()<<" "<<parent_node->getFunction()->getParent()->getParent()->getName()<<endl;
            getCallPath(parent_node, depth+1, visited, atlist);
        }
    }
   // auto parents = n->getDirectCallTargets();//n->getParent();
    if(parents.size() == 0)
    {
        cout<<"No direct call to function "<<n->getFunction()->getName()<<endl;
    }

}

/****** Function which traces back the callpath of functions which are passed as argument to the function, 
        back to main() *****/
void Syspart::run8(bool direct, bool icanalysisFlag, bool typearmorFlag, vector<string> funcs)
{
    ip_callgraph.setProgram(program);
    ip_callgraph.setRoot(start_func);
    ip_callgraph.resolveNss(setup);
    ip_callgraph.setIcanalysis(icanalysisFlag);
    ip_callgraph.setTypeArmor(typearmorFlag);
    if(direct)
        ip_callgraph.generateDirectCallGraph();
    else
       ip_callgraph.generate();

    finiFuncs = ip_callgraph.getFiniFuncs();
    initFuncs = ip_callgraph.getInitFuncs();
    ip_callgraph.addNssEdges();
    auto atlist = ip_callgraph.getGlobalATList();
    //string funcs[] = { "longjmp","__longjmp", "_setjmp", "__pthread_unwind" };
    for(auto f : funcs)
    {
        cout<<"Function "<<f<<endl;
         auto func = findFunctionByName(f);
         if(func == NULL)
         {
            cout<<"No function with name "<<f<<endl;
             continue;
         }
         auto ip_node = ip_callgraph.getNode(func);
         if(ip_node == NULL)
         {
            cout<<"No ipcallgraph node for function "<<f<<endl;
             continue;      
         }
         set<IPCallGraphNode*> visited;
         getCallPath(ip_node, 0, &visited, atlist);
             
    }


}

/**** FUNCTION WHICH PRINTS FOR EACH AT FUNCTION, THE SYSTEM CALLS THAT ARE ACCESSIBLE 
      ONLY USING DIRECT EDGES FROM THE AT FUNCTION ****/
void Syspart::run9(bool direct, bool icanalysisFlag, bool typearmorFlag)
{
    ip_callgraph.setProgram(program);
    ip_callgraph.setRoot(start_func);
    ip_callgraph.resolveNss(setup);
    ip_callgraph.setIcanalysis(icanalysisFlag);
    ip_callgraph.setTypeArmor(typearmorFlag);
    if(direct)
        ip_callgraph.generateDirectCallGraph();
    else
       ip_callgraph.generate();
    ip_callgraph.addNssEdges();
    finiFuncs = ip_callgraph.getFiniFuncs();
    initFuncs = ip_callgraph.getInitFuncs();
    auto atlist = ip_callgraph.getGlobalATList();

    findDirectSyscalls();
    findDerivedSyscalls(start_func);

    for(auto at : atlist)
    {
        //cout<<"FUNC : "<<at->getName()<<endl;
        auto node = ip_callgraph.getNode(at);
        
        stack<IPCallGraphNode*> st;
        st.push(node);
        vector<IPCallGraphNode*> visited;
        while(!st.empty())
        {
            auto ipnode = st.top();
            st.pop();
            if(find(visited.begin(), visited.end(), ipnode) != visited.end())
                continue;
            auto f1 = ipnode->getFunction();
            //cout<<"STACK FUNC : "<<f1->getName()<<endl;
            for(auto w : who_invokes_syscalls)
            {
                for(auto snode : w.second)
                {
                    auto f2 = snode->func;
                    //cout<<"W " <<f2->getName()<<" "<<system_calls[w.first]<<endl;  
                    if(f1->getName() == f2->getName() && f1->getAddress() == f2->getAddress() && f1->getParent()->getParent() == f2->getParent()->getParent())
                    {
                        cout<<system_calls[w.first]<<"\t"<<f1->getName()<<"\t"<<at->getName()<<"\t"<<endl;
                    }
                }
            }
            auto direct_children = ipnode->getDirectCallTargets();
            for(auto d : direct_children)
            {
                st.push(d);
            }
            visited.push_back(ipnode);
        }
    }
}

/* GIVEN THE ADDRESS OF THE MAINLOOP AND THE NAME OF THE MAINLOOP FUNCTION,
   CALCULATES ALL SYSTEM CALL ACCESSIBLE FROM THE MAIN LOOP
   USING NORETURN ANALYSIS AND IPCALLGRAPH AND SYSCALL ANALYSIS,
   AND PRINTS THE FILTERED SYSTEM CALLS */

void Syspart::syscallsOfMainLoop(bool icanalysisFlag, bool typearmorFlag, string addr, string func_name)
{
    ip_callgraph.setProgram(program);
    ip_callgraph.setRoot(start_func);
    ip_callgraph.resolveNss(setup);

    ip_callgraph.setIcanalysis(icanalysisFlag);
    ip_callgraph.setTypeArmor(typearmorFlag);
    if(typearmorFlag)
        ip_callgraph.setTypeArmorPath(typearmorPath);
    ip_callgraph.generate();
    finiFuncs = ip_callgraph.getFiniFuncs();
    initFuncs = ip_callgraph.getInitFuncs();
    ip_callgraph.addNssEdges();
    findDirectSyscalls();
    findDerivedSyscalls(start_func);
    //getSyscallInfo(start_func);
    NonReturnAnalysis nr;
    nr.run(program);

    auto thread_start_fns = getThreadStartFunction();

    set<int> tot_syscalls;
    
    auto func = findFunctionByName(func_name);
    if(func == NULL)
    {
        cout<<"No function named "<<func_name<<" found"<<endl;
        return;
    }
    auto ipnode_found = ip_callgraph.getNode(func);
    if(ipnode_found == NULL)
    {
        cout<<"Sorry no ipnode found"<<endl;
        return;
    }
    //auto dir = ipnode_found->getDirectChildren();
    address_t call_addr = (address_t)stol(addr, NULL, 16);
    stack<tuple<IPCallGraphNode*,address_t>> st;
    tuple<IPCallGraphNode*,address_t> tup1(ipnode_found, call_addr);
    st.push(tup1);
    vector<tuple<IPCallGraphNode*,address_t>> visited;

    while(!st.empty())
    {
        auto tup1 = st.top();
        st.pop();
        IPCallGraphNode* ipnode_found;
        address_t call_addr;
        tie(ipnode_found, call_addr) = tup1;
        auto cur_func = ipnode_found->getFunction();
        bool flag = false;
        for(auto v : visited)
        {
            IPCallGraphNode* ipnode_1;
            address_t call_addr_1;
            tie(ipnode_1, call_addr_1) = v;
            if(ipnode_found == ipnode_1 && call_addr == call_addr_1)
                flag = true;
        }
        if(flag)
            continue;
        if(cur_func == NULL)
        {
            cout<<"No function found by name "<<cur_func->getName()<<endl;
            return;
        }
        auto s = findSyscallsAccessible(call_addr,cur_func);

        for(auto ss : s)
        {
            tot_syscalls.insert(ss);
        }
        visited.push_back(tup1);

        if(!cur_func->returns())
        {
            continue;
        }
        if(thread_start_fns.count(cur_func) != 0)
            continue;
        auto p = ipnode_found->getParentCallSites();
        for(auto pp : p)
        {
            tuple<IPCallGraphNode*, address_t> tup2(pp.second, pp.first);
            st.push(tup2);
        }
    }
    set<int> fini_syscalls = getSyscallsofFini();
    for(auto fini_sys : fini_syscalls)
        tot_syscalls.insert(fini_sys);

    stack<Function*> part_fns;
    for(auto f : finiFuncs)
    {
        part_fns.push(f);
    }
    for(auto f : partitionFns)
    {
        part_fns.push(f);
    }
    set<Function*> visitedPartitionFns;
    while(!part_fns.empty())
    {
        auto f = part_fns.top();
        part_fns.pop();
        if(visitedPartitionFns.find(f) != visitedPartitionFns.end())
            continue;
        auto ip_node = ip_callgraph.getNode(f);
        if(ip_node == NULL)
            continue;
        auto direct_ch = ip_node->getDirectChildren();
        auto indirect_ch = ip_node->getIndirectChildren();
        for(auto bl : CIter::children(f))
        {
            for(auto instr : CIter::children(bl))
            {
                partitionSize++;
                auto d_iter = direct_ch.find(instr->getAddress());
                if(d_iter != direct_ch.end())
                {
                    auto ipset = d_iter->second;
                    for(auto ip : ipset)
                    {
                        //cout<<"direct func  "<<ip->getFunction()<<endl;
                        if(visitedPartitionFns.find(ip->getFunction()) == visitedPartitionFns.end())
                            part_fns.push(ip->getFunction());
                    }
                }
                auto ind_iter = indirect_ch.find(instr->getAddress());
                if(ind_iter != indirect_ch.end())
                {
                  auto ipset = ind_iter->second;
                 //cout<<"IND "<<std::hex<<instr->getAddress()<<" "<<endl;
                 for(auto ip : ipset)
                 {
                    //cout<<"indirect func  "<<ip->getFunction()<<endl;

                    if(visitedPartitionFns.find(ip->getFunction()) == visitedPartitionFns.end())
                        part_fns.push(ip->getFunction());

                 }
             }
            }
        }
        visitedPartitionFns.insert(f);
    }
    cout<<"\nMAINLOOP "<<std::dec<<tot_syscalls.size()<<endl;
    /*
    cout<<"\nMAINLOOP "<<std::dec<<tot_syscalls.size()<<" : (";
    
    for(auto i : tot_syscalls)
    {
        cout<<system_calls[i]<<" ";
    }
    cout<<") ";
    */
    cout<<"JSON [";
    int j=0;
    for(auto i : tot_syscalls)
    {
        if(j != 0)
            cout<<",";
        cout<<std::dec<<i;
        j++;
    }
    cout<<"]"<<endl;
    /***************** FILTERED SYSTEM CALLS *******************/
    auto sys_node = getSysNode(start_func);
    if(sys_node == NULL)
        return;
    set<int> start_syscalls;
    set<int> filtered_syscalls;
    for(auto d : sys_node->direct_syscalls)
    {
        start_syscalls.insert(d);
    }
    for(auto d : sys_node->derived_syscalls)
    {
        start_syscalls.insert(d);
    }
    for(auto fini_sys : fini_syscalls)
        start_syscalls.insert(fini_sys);
    set_difference(start_syscalls.begin(), start_syscalls.end(), tot_syscalls.begin(), tot_syscalls.end(), std::inserter(filtered_syscalls, filtered_syscalls.begin()));
    cout<<"\nMAIN "<<std::dec<<start_syscalls.size()<<endl;
    cout<<"\nAT "<<std::dec<<(ip_callgraph.getGlobalATList()).size()<<endl;
    /*cout<<"The system calls which are filtered because of temporal specialization are : "<<endl;
    for(auto s : filtered_syscalls)
    {
        cout<<system_calls[s]<<" ";
    }
    cout<<endl;*/
    cout<<"PARTITION_SIZE "<<partitionSize<<endl;
}

void Syspart::run11()
{
    for(auto module : CIter::children(program))
        {
            for(auto func : CIter::functions(module))
            {
                    cout<<func->getName()<<"\t"<<std::hex<<func->getAddress()<<"\t"<<module->getName()<<endl;
            }
        }
}

/**** PRINT ONLY INDIRECT EDGES OF CALLGRAPH ********/
void Syspart::run12(bool direct, bool icanalysisFlag, bool typearmorFlag)
{
    ip_callgraph.setProgram(program);
    ip_callgraph.setRoot(start_func);
    ip_callgraph.resolveNss(setup);
    ip_callgraph.setIcanalysis(icanalysisFlag);
    ip_callgraph.setTypeArmor(typearmorFlag);
    if(direct)
        ip_callgraph.generateDirectCallGraph();
    else
       ip_callgraph.generate();
    finiFuncs = ip_callgraph.getFiniFuncs();
    initFuncs = ip_callgraph.getInitFuncs();
    ip_callgraph.addNssEdges();
    ip_callgraph.printIndirectEdges();
      
}

/**** PRINT ONLY DIRECT EDGES OF CALLGRAPH ********/
void Syspart::run13(bool direct, bool icanalysisFlag, bool typearmorFlag)
{
    ip_callgraph.setProgram(program);
    ip_callgraph.setRoot(start_func);
    ip_callgraph.resolveNss(setup);
    ip_callgraph.setIcanalysis(icanalysisFlag);
    ip_callgraph.setTypeArmor(typearmorFlag);
    if(direct)
        ip_callgraph.generateDirectCallGraph();
    else
       ip_callgraph.generate();
    finiFuncs = ip_callgraph.getFiniFuncs();
    initFuncs = ip_callgraph.getInitFuncs();
    ip_callgraph.addNssEdges();
    ip_callgraph.printDirectEdges();
      
}

void Syspart::printAllSections()
{
    for(auto module : CIter::children(program))
    {
        for(auto region : CIter::regions(module))
        {

            for(auto dataSection : CIter::children(region))
            {
         
                    for(auto gl : dataSection->getGlobalVariables())
                    {        
                        auto glOffset = gl->getAddress()-module->getBaseAddress();
                        cout<<"Module "<<module->getName()<<" " <<std::hex<<module->getBaseAddress() << " Region "<<region->getName()<<" DataSection "<<dataSection->getName()<<" " <<dataSection->getType()<<"Global "<<gl->getName()<<" "<<std::hex<<gl->getAddress()<<" "<<glOffset<<" " <<std::dec<<gl->getSize()<<endl;
                    }
                for(auto var : CIter::children(dataSection))
                {
                    auto link = var->getDest();
                    auto varOffset = var->getAddress()-module->getBaseAddress();
                    cout<<"Module "<<module->getName()<<" " <<std::hex<<module->getBaseAddress() << " Region "<<region->getName()<<" DataSection "<<dataSection->getName()<<" " <<dataSection->getType();
                    cout<<" DataVariable "<<std::hex<<var->getAddress()<<" " <<varOffset<<" "<<var->getName()<<" Size "<<std::dec<<var->getSize();
                    if(link)
                    {
                        //LOG(20, "TypeID " << typeid(*link).name());
                        if(dynamic_cast<DataOffsetLink *>(&*link))
                            cout<<" Dataoffset_link_address "<<std::hex<<link->getTargetAddress();
                        else
                            cout<<" Dataoffset_link_address NULL";
                        if(link->getTarget())
                            cout<<" Target " << typeid(*link->getTarget()).name() << " " <<std::hex<<link->getTargetAddress();
                        else
                            cout<<" Target NULL NULL"; 
                        if(auto target = dynamic_cast<Function *>(&*link->getTarget()))
                        {   
                            cout<<" FUNC " << target->getName()<<" "<<target->getAddress()<<endl;
                        }
                        else if(auto target = dynamic_cast<DataSection *>(&*link->getTarget()))
                        {
                            cout<<" DS "<< target->getName()<<" "<<target->getType()<<endl;

                        }
                        else
                        {
                            cout<<" UNKNOWN NULL NULL"<<endl;
                        }
                        
                    }
                }
            }
        }
    }
}

void Syspart::getArgumentValue(bool icanalysisFlag, bool typearmorFlag, string function, int reg, char* filename)
{
    ip_callgraph.setProgram(program);
    ip_callgraph.setRoot(start_func);
    ip_callgraph.resolveNss(setup);
    ip_callgraph.setIcanalysis(icanalysisFlag);
    ip_callgraph.setTypeArmor(typearmorFlag);
    ip_callgraph.generate();
    finiFuncs = ip_callgraph.getFiniFuncs();
    initFuncs = ip_callgraph.getInitFuncs();
    ip_callgraph.addNssEdges();
    SyspartUtility util(program, &ip_callgraph, 0);
    util.initialize();
    for(auto module : CIter::children(program))
        {
            for(auto func : CIter::functions(module))
            {
                auto fname = func->getName();
                if(fname == function)
                {
                    vector<UDResult> res;
                    util.getArgumentsPassedToFunction(func, reg , res);
                  
                }                        
                         
            }
        }   
    
    
}


/****** PRINTS SYSCALLINFO OF a specific function or ALL FUNCTIONS ****/
void Syspart::run14(bool direct, bool icanalysisFlag, bool typearmorFlag, string func_name)
{
    ip_callgraph.setProgram(program);
    ip_callgraph.setRoot(start_func);
    ip_callgraph.resolveNss(setup);
    ip_callgraph.setIcanalysis(icanalysisFlag);
    ip_callgraph.setTypeArmor(typearmorFlag);
    if(typearmorFlag)
        ip_callgraph.setTypeArmorPath(typearmorPath);
    if(direct)
        ip_callgraph.generateDirectCallGraph();
    else
       ip_callgraph.generate();
    finiFuncs = ip_callgraph.getFiniFuncs();
    initFuncs = ip_callgraph.getInitFuncs();
    ip_callgraph.addNssEdges();
    SyspartUtility util(program, &ip_callgraph, 1);
    if(func_name != "*")
    {
        auto f = findFunctionByName(func_name);
        auto n = ip_callgraph.getNode(f);
        if(n != NULL)
            util.findIndirectCallTargets(n);

    }
    else
    {
        for(auto f : ip_callgraph.nodeMap)
        {
            auto fnode = f.second;
            if(fnode->hasIndirectCall())
                util.findIndirectCallTargets(fnode);

        }
    }
}


void Syspart::printAICT(bool icanalysisFlag, bool typearmorFlag)
{
    ip_callgraph.setProgram(program);
    ip_callgraph.setRoot(start_func);
    ip_callgraph.resolveNss(setup);
    ip_callgraph.setIcanalysis(icanalysisFlag);
    ip_callgraph.setTypeArmor(typearmorFlag);
    ip_callgraph.generate();
    finiFuncs = ip_callgraph.getFiniFuncs();
    initFuncs = ip_callgraph.getInitFuncs();
    ip_callgraph.addNssEdges();
    int sum = 0;
    int n = 0;
    int napp=0;
    int sumapp=0;
    for(auto node_map : ip_callgraph.nodeMap)
    {
        auto node = node_map.second;
        auto indirect_ch  = node->getIndirectChildren();
        auto mod_node = node->getFunction()->getParent()->getParent();

        for(auto ind : indirect_ch)
        {
            sum += (ind.second).size();
            n++;
            if(mod_node->getName() == "module-(executable)")
            {
                sumapp += (ind.second).size();
                napp++;
            }
        }

    }

    cout<<"#INDIRECT CALLS : "<< std::dec<< n <<endl;
    cout<<"#AVERAGE INDIRECT CALL TARGET : "<< (sum/n) << endl;
    cout<<"GLOBAL AT LIST : "<<(ip_callgraph.getGlobalATList()).size()<<endl;
    cout<<"#APP INDIRECT CALLS : "<< napp <<endl;
    cout<<"#APP AVERAGE INDIRECT CALL TARGET : "<< (sumapp/napp) << endl;
}

void Syspart::getSyscallsFromDlsym(bool direct, bool icanalysisFlag, bool typearmorFlag, string file_name)
{
    struct data
    {
        Function* called_fn;
        Instruction* called_addr;
        Instruction* used_addr;
        string sym;
        string lib;
        Module* mod;
    };
    vector<data> values;
    DataFlow df;
    for(auto module : CIter::children(program))
    {
        for(auto func : CIter::functions(module))
        {
            df.addUseDefFor(func);
        }
    }
    ifstream myfile(file_name);
    string line;
    map<string,Module*> seen_libs;
    if(myfile.is_open())
    {
        while(getline(myfile, line))
        {
            istringstream ss(line);
            int i=0;
            string fn,addr,sym,lib;
            while(ss)
            {
                string word;
                ss >> word;
                if(i==0)
                    fn=word;
                else if(i==1)
                    addr=word;
                else if(i==2)
                    sym=word;
                else if(i==3)
                    lib=word;
                i++;
            }
            if(sym=="UNKNOWN")
                continue;      
    
            address_t call_addr = (address_t)stol(addr, NULL, 16);
            int flag = 0;
            Function* call_func=NULL;
            Instruction* call_instr=NULL;
            for(auto module : CIter::children(program))
            {
                for(auto func : CIter::functions(module))
                {
                    if(func->getName() != fn)
                        continue;
                    for(auto bl : CIter::children(func))
                    {
                        for(auto instr : CIter::children(bl))
                        {
                            if(instr->getAddress() == call_addr)
                            {
                                call_func=func;
                                call_instr=instr;
                                flag = 1;
                                break;
                            }

                        }
                    }
                }
            }
            if(flag == 0)
            {
                cout<<"Function "<<fn<<" with addr "<<std::hex<<call_addr<<" not found"<<endl;
                continue;
            }
            
                    Module* mm;
                    auto it = seen_libs.find(lib);
                    if(it == seen_libs.end())
                    {
                        std::vector<std::string> libToLoad;
                        libToLoad.push_back(lib);
                        auto modules = setup->addExtraLibraries(libToLoad);
                        for(auto m : modules)
                        {
                            mm = m;
                        }
                        seen_libs[lib] = mm;
                    }
                    else
                    {
                        mm=it->second;
                    }
                    int foundSym=0;
                    Function* sym_func;
                    Module* sym_mod;
                    Instruction* used_addr = NULL;
                    struct data d1 = {call_func, call_instr, used_addr, sym, lib, mm};
                    
                    
                    values.push_back(d1);
        }
    }
    else
    {
        cout<<"Error file"<<endl;
    }
    myfile.close();
    
    ip_callgraph.setProgram(program);
    ip_callgraph.setRoot(start_func);
    ip_callgraph.setIcanalysis(false);
    ip_callgraph.setTypeArmor(false);
    ip_callgraph.resolveNss(setup);

    ip_callgraph.addNssEdges();
    for(auto v : values)
    {
        cout<<"Value "<<v.sym<<" "<<v.lib<<endl;
        //Find Function obj of v.sym in v.mod
        vector<Function*> sym_funcs;
        int sym_found=0;
        for(auto f : CIter::functions(v.mod))
        {
            if(f->getName() == v.sym)
            {
                sym_found=1;
                sym_funcs.push_back(f);
                break;
            }
        }
        if(sym_found == 0)
        {
            for(auto region : CIter::regions(v.mod))
            {
                for(auto ds : CIter::children(region))
                {
                    for(auto gl : ds->getGlobalVariables())
                    {
                        if(gl->getName() == v.sym)
                        {
                            auto st = gl->getAddress();
                            auto en = gl->getAddress() + gl->getSize();
                            cout<<"Global variable at address "<<std::hex<<st<<" and  "<<en<<endl;
                            for(auto dv : CIter::children(ds))
                            {
                                cout<<"\n \t \t \t \t"<<std::hex<<dv->getAddress()<<" " <<dv->getName()<<" Size "<<dv->getSize()<<endl;
                                if(dv->getAddress() >= st && dv->getAddress() < en)
                                {
                                    auto dvLink = dv->getDest();
                                    if(dvLink)
                                    {
                                        auto dvTarget = dvLink->getTarget();
                                        if(dvTarget)
                                        {
                                            auto dvTargetAddr = dvLink->getTargetAddress();
                                            if(Function* global_fun = dynamic_cast<Function*>(dvTarget))
                                            {
                                                cout<<"Link to function "<<global_fun->getName()<<endl;
                                                sym_funcs.push_back(global_fun);
                                                sym_found=1; 
                                            }
                                            else if(auto plt = dynamic_cast<PLTTrampoline *>(dvTarget))
                                            {
                                                if(auto plt_target = dynamic_cast<Function *>(plt->getTarget()))
                                                {
                                                    cout<<"Link to PLT "<<plt_target->getName()<<endl;
                                                    sym_funcs.push_back(global_fun);
                                                    sym_found=1;
                                                }
                                            }
                                      
                                            else if(DataSection* global_data = dynamic_cast<DataSection*>(dvTarget))
                                            {
                                                cout<<"Refers to "<<dvLink->getTargetAddress()<<" DS "<<global_data->getName()<<endl;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if(sym_found == 0)
        {
            cout<<"No sym "<<v.sym<<" found in "<<v.lib<<endl;
            continue;
        }
        for(auto sym_func : sym_funcs)
        {
            ip_callgraph.addFunctionRoot(sym_func);
        }
        
    }
    cout<<"Going to generate"<<endl;
    ip_callgraph.generate();
    finiFuncs = ip_callgraph.getFiniFuncs();
    initFuncs = ip_callgraph.getInitFuncs();
    cout<<"Generation done"<<endl;
    /*for(auto f : ip_callgraph.nodeMap)
    {
        cout<<(f.first)->getName()<<" "<<(f.first)->getParent()->getParent()->getName()<<endl;
    }*/
    vector<IPCallGraphNode*> moreAT;
    for(auto v : values)
    {
        auto node = ip_callgraph.getNode(v.called_fn);
        if(node == NULL)
        {
            cout<<"No node with "<<v.called_fn->getName()<<endl;
            continue;
        }
        //Find Function obj of v.sym in v.mod
        vector<Function*> sym_funcs;
        int sym_found=0;
        for(auto f : CIter::functions(v.mod))
        {
            if(f->getName() == v.sym)
            {
                sym_found=1;
                sym_funcs.push_back(f);
                break;
            }
        }
        if(sym_found == 0)
        {
            for(auto region : CIter::regions(v.mod))
            {
                for(auto ds : CIter::children(region))
                {
                    for(auto gl : ds->getGlobalVariables())
                    {
                        if(gl->getName() == v.sym)
                        {
                            auto st = gl->getAddress();
                            auto en = gl->getAddress() + gl->getSize();
                            cout<<"Global variable at address "<<std::hex<<st<<" and  "<<en<<endl;
                            for(auto dv : CIter::children(ds))
                            {
                                if(dv->getAddress() >= st && dv->getAddress() < en)
                                {
                                    auto dvLink = dv->getDest();
                                    if(dvLink)
                                    {
                                        auto dvTarget = dvLink->getTarget();
                                        if(dvTarget)
                                        {
                                            auto dvTargetAddr = dvLink->getTargetAddress();
                                            if(Function* global_fun = dynamic_cast<Function*>(dvTarget))
                                            {
                                                cout<<"Link to function "<<global_fun->getName()<<endl; 
                                                sym_funcs.push_back(global_fun);
                                                sym_found=1;
                                            }
                                            else if(auto plt = dynamic_cast<PLTTrampoline *>(dvTarget))
                                            {
                                                if(auto plt_target = dynamic_cast<Function *>(plt->getTarget()))
                                                {
                                                    cout<<"Link to PLT "<<plt_target->getName()<<endl;
                                                    sym_funcs.push_back(plt_target);
                                                    sym_found=1;
                                                }
                                            }
                                            else if(DataSection* global_data = dynamic_cast<DataSection*>(dvTarget))
                                            {
                                                cout<<"Refers to "<<dvTargetAddr<<" DS "<<global_data->getName()<<endl;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if(sym_found == 0)
        {
            cout<<"No sym "<<v.sym<<" found in "<<v.lib<<endl;
            continue;
        }        
        for(auto sym_func : sym_funcs)
        {
            auto called_node = ip_callgraph.getNode(sym_func);
            if(called_node == NULL)
            {
                cout<<"No node with "<<sym_func->getName()<<endl;
                continue;
            }
            cout<<"Adding AT function "<<v.sym<<" "<<v.lib<<endl;
            ip_callgraph.addATFunction((v.called_addr)->getAddress(), v.called_fn, sym_func);
            auto res = ip_callgraph.forwardDataFlow(v.called_fn, v.called_addr, sym_func);               
            if(!res)        //Cannot be filtered
            {
                ip_callgraph.addtoGlobalATList(sym_func);
                auto sym_node = ip_callgraph.getNode(sym_func);
                if(sym_node != NULL)
                    moreAT.push_back(sym_node);
            }
            cout<<"Done adding AT function"<<endl;
        }

        //node->insertCallTarget((v.used_addr)->getAddress(), false, called_node);
    }

    if(moreAT.size() > 0)
    {
        cout<<"Updating"<<endl;
        for(auto f : ip_callgraph.nodeMap)
        {
            auto ipnode = f.second;
            auto indirect_children = ipnode->getIndirectChildren();
            for(auto ind : indirect_children)
            {
                auto addr = ind.first;
                auto node_set = ind.second;
                for(auto at : moreAT)
                {
                    node_set.insert(at);
                }
                ind.second = node_set;
                ipnode->updateIndirectChildren(addr,node_set);
            }
        }
    }  
    //ip_callgraph.printCallGraph();
    cout<<"Finding direct system calls"<<endl;
    findDirectSyscalls();
    cout<<"Finding derived system calls"<<endl;
    findDerivedSyscalls(start_func);
    cout<<"Done"<<endl;
    for(auto f : ip_callgraph.nodeMap)
        {
            getSyscallInfo(f.first);
        }    
}

int Syspart::getNoReturnFnCount()
{
    int count=0;
    for(auto module : CIter::children(program))
    {
        for(auto func : CIter::functions(module))
        {
            if(!func->returns())
            {
                count++;
            }
        }
    }
    return count;
}


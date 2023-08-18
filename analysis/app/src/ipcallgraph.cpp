#include<iostream>
#include <typeinfo>
#include<stack>
#include <fstream>
#include<sstream>
using namespace std;

#include "chunk/link.h"
#include "instr/linked-x86_64.h"
#include "ipcallgraph.h"
#include "elf/elfspace.h"
#include "chunk/dump.h"
#include "chunk/program.h"
#include "elf/elfspace.h"
#include "instr/linked-x86_64.h"
#include "disasm/makesemantic.h"
#include "conductor/conductor.h"
#include "pass/chunkpass.h"
#include "analysis/walker.h"
#include "elf/elfdynamic.h"
#include "pass/collapseplt.h"
#include "pass/resolveplt.h"
#include "syspartUtility.h"
#include<climits> //For INT_MIN

#undef DEBUG_GROUP
#define DEBUG_GROUP ipcallgraph
#define D_ipcallgraph 20

#include "log/log.h"


void IPCallGraphNode::insertCallTarget(address_t iaddr, bool isDirect, IPCallGraphNode* t)		
{
	if(isDirect)
	{
		auto child_iter = direct_children.find(iaddr);
		if(child_iter != direct_children.end())
		{
			auto call_targets = (child_iter)->second;
			(call_targets).insert(t);
			direct_children[iaddr] = call_targets;

		}
		else
		{
			set<IPCallGraphNode*> call_targets;
			(call_targets).insert(t);
			direct_children[iaddr] = call_targets;
		}
	}
	else
	{
		auto child_iter = indirect_children.find(iaddr);
		if(child_iter != indirect_children.end())
		{
			auto call_targets = (child_iter)->second;
			(call_targets).insert(t);
			indirect_children[iaddr] = call_targets;

		}
		else
		{
			set<IPCallGraphNode*> call_targets;
			(call_targets).insert(t);
			indirect_children[iaddr] = call_targets;
		}
	}
}

void IPCallGraphNode::insertCallTargetSet(address_t iaddr, bool isDirect, set<IPCallGraphNode*> t)
{
	if(isDirect)
	{
		auto child_iter = direct_children.find(iaddr);
		if(child_iter != direct_children.end())
		{
			auto call_targets = (child_iter)->second;
			(call_targets).insert(t.begin(), t.end());
			direct_children[iaddr] = call_targets;

		}
		else
		{
			direct_children[iaddr] = t;
		}
	}
	else
	{
		auto child_iter = indirect_children.find(iaddr);
		if(child_iter != indirect_children.end())
		{
			auto call_targets = (child_iter)->second;
			(call_targets).insert(t.begin(), t.end());
			indirect_children[iaddr] = call_targets;

		}
		else
		{
			indirect_children[iaddr] = t;
		}
	}
}

void IPCallGraphNode::removeAllCallTargets(IPCallGraphNode* t)
{
	for(auto dir : direct_children)
	{
		auto children = dir.second;
		auto child_iter = (children).find(t);
		if (child_iter != children.end())
		{
			LOG(20, "Found "<<t->getFunction()->getName()<< " in direct children");
			children.erase(child_iter);
		}
		dir.second = children;
	}

	for(auto indir : indirect_children)
	{
		auto children = indir.second;
		auto child_iter = (children).find(t);
		if (child_iter != children.end())
		{
			LOG(20, "Found "<<t->getFunction()->getName()<< " in indirect children");
			children.erase(child_iter);
		}
		indir.second = children;
	}
}

void IPCallGraphNode::removeCallTarget(address_t addr, IPCallGraphNode* t)
{
	auto dir_iter = direct_children.find(addr);
	if(dir_iter != direct_children.end())
	{
		auto children = (dir_iter)->second;
		auto child_iter = (children).find(t);
		if (child_iter != children.end())
		{
			LOG(20, "Found "<<t->getFunction()->getName()<< " in direct children");
			children.erase(child_iter);
		}
		(dir_iter)->second = children;
	}

	auto indir_iter = indirect_children.find(addr);
	if(indir_iter != indirect_children.end())
	{
		auto children = (indir_iter)->second;
		auto child_iter = (children).find(t);
		if (child_iter != children.end())
		{
			LOG(20, "Found "<<t->getFunction()->getName()<< " in indirect children");
			children.erase(child_iter);
		}
		(indir_iter)->second = children;
	}
}

void IPCallGraphNode::removeParent(IPCallGraphNode* p)
{
	address_t addr_to_be_removed;
	int flag = 0;
	for(auto parent_iter : parent)
	{
		auto par = parent_iter.second;
		if(par == p)
		{
			flag = 1;
			addr_to_be_removed = parent_iter.first;
		}
	}
	if(flag == 1)
	{
		parent.erase(addr_to_be_removed);
		LOG(20, "Removed parent "<<p->getFunction()->getName()<<" from "<<this->func->getName());

	}
}

void IPCallGraphNode::removeParent(address_t addr, IPCallGraphNode* p)
{
	auto parent_iter = parent.find(addr);
	if(parent_iter != parent.end())
	{
		parent.erase(parent_iter);
		LOG(20, "Removed "<<p->getFunction()->getName()<<" at "<<addr<<" from "<<this->func->getName());

	}

}

void IPCallGraphNode::setFunction(Function* f)
{
	func = f;
}

Function* IPCallGraphNode::getFunction()
{
	return func;
}

void IPCallGraphNode::insertParent(address_t addr, IPCallGraphNode* p, bool type)
{
	parent[addr] = p;
	tuple<address_t, IPCallGraphNode*> t1(addr, p);
	parentType[t1] = type;
}

set<IPCallGraphNode*> IPCallGraphNode::getAllCallTargets()
{
	set<IPCallGraphNode*> s;
	for(auto c : direct_children)
	{
		s.insert((c.second).begin(), (c.second).end());
	}
	for(auto c : indirect_children)
	{
		s.insert((c.second).begin(), (c.second).end());
	}
	return s;
}

set<IPCallGraphNode*> IPCallGraphNode::getDirectCallTargets()
{
	set<IPCallGraphNode*> s;
	for(auto c : direct_children)
	{
		s.insert((c.second).begin(), (c.second).end());
	}
	return s;
}

map<address_t, set<IPCallGraphNode*>> IPCallGraphNode::getATList()
{
	return ATFunctions;
}


void IPCallGraph::addFunctionRoot(Function* func)
{
	//if(visitedFunctions.count(func) != 0)
	//s	return;
	functionRoots.push_back(func);
}

void IPCallGraph::setRoot(Function* func)
{
	startfunc = func;
	functionRoots.push_back(func);
}

vector<Function*> IPCallGraph::getFiniFuncs()
{
	return finiFuncs;
}

vector<Function*> IPCallGraph::getInitFuncs()
{
	return initFuncs;
}

void IPCallGraph::removeAllEdges(Function* start, Function* end)
{
	auto start_iter = nodeMap.find(start);
	auto end_iter = nodeMap.find(end);
	if(start_iter == nodeMap.end())
	{
		LOG(1, "No node exists for "<<start->getName()<<"\n Returning \n.");
		return;
	}
	if(end_iter == nodeMap.end())
	{
		LOG(1, "No node exists for "<<end->getName()<<"\n Returning \n.");
		return;
	}
	auto start_node = (start_iter)->second;
	auto end_node = (end_iter)->second;
	(start_node)->removeAllCallTargets(end_node);
	(end_node)->removeParent(start_node);
}

void IPCallGraph::removeEdge(address_t addr, Function* start, Function* end)
{
	auto start_iter = nodeMap.find(start);
	auto end_iter = nodeMap.find(end);
	if(start_iter == nodeMap.end())
	{
		LOG(1, "No node exists for "<<start->getName()<<"\n Returning \n.");
		return;
	}
	if(end_iter == nodeMap.end())
	{
		LOG(1, "No node exists for "<<end->getName()<<"\n Returning \n.");
		return;
	}
	auto start_node = (start_iter)->second;
	auto end_node = (end_iter)->second;
	(start_node)->removeCallTarget(addr,end_node);
	(end_node)->removeParent(addr,start_node);

}
void IPCallGraph::addEdge(address_t addr, Function* start, Function* end, bool isDirect)
{
	auto start_iter = nodeMap.find(start);
	auto end_iter = nodeMap.find(end);
	IPCallGraphNode* start_node; 
	IPCallGraphNode* end_node;

	if(start_iter == nodeMap.end())
	{
		start_node = new IPCallGraphNode(start);
		df.getWorkingSet(start);
		nodeMap[start] = start_node;

	}
	else
		start_node = (start_iter)->second;

	if(end_iter == nodeMap.end())
	{
		end_node = new IPCallGraphNode(end);
		df.getWorkingSet(end);

		nodeMap[end] = end_node;
	}
	else
		end_node = (end_iter)->second;

	start_node->insertCallTarget(addr, isDirect, end_node);
	end_node->insertParent(addr, start_node, isDirect);
}

void IPCallGraph::addATFunction(address_t addr, Function* start, Function* ATfunc)
{
	auto start_iter = nodeMap.find(start);
	auto ATfunc_iter = nodeMap.find(ATfunc);

	IPCallGraphNode* start_node;
	IPCallGraphNode* ATfunc_node;

	if(start_iter == nodeMap.end())
	{
		start_node = new IPCallGraphNode(start);
		df.getWorkingSet(start);
		LOG(15,start<<" "<<start_node<<" "<<std::hex<<start->getAddress()<<" "<<start->getName());
		nodeMap[start] = start_node;
	}
	else
		start_node = (start_iter)->second;

	if(ATfunc_iter == nodeMap.end())
	{
		ATfunc_node = new IPCallGraphNode(ATfunc);
		df.getWorkingSet(ATfunc);
		LOG(15, ATfunc<<" "<<ATfunc_node<<" "<<std::hex<<ATfunc->getAddress()<<" "<<ATfunc->getName());

		nodeMap[ATfunc] = ATfunc_node;
	}
	else
		ATfunc_node = (ATfunc_iter)->second;

	(start_node)->addATFunction(addr, ATfunc_node);

}

void IPCallGraph::addIndirectSource(address_t addr, Function* func)
{
	auto f_iter = nodeMap.find(func);
	IPCallGraphNode* n;
	if(f_iter == nodeMap.end())
	{
		n = new IPCallGraphNode(func);
		df.getWorkingSet(func);
		LOG(15, func<<" "<<n<<" "<<std::hex<<func->getAddress()<<" "<<func->getName());

		nodeMap[func] = n;
	}
	else
		n = (f_iter)->second;
	n->addIndirectSource(addr);
}

void IPCallGraph::printCallGraphofApplication()
{
	int i=0;
	int cfg_tot=0, fcg_tot=0,nic=0,nat=0;
	set<IPCallGraphNode*> allat;
	for(auto n : nodeMap)
	{
		auto node = n.second;
		auto module = (n.first)->getParent()->getParent();
		if(module->getName() != "module-(executable)")
			continue;
		for(auto c : node->getDirectChildren())
		{
			cfg_tot+=(c.second).size();
		}
		for(auto c : node->getIndirectChildren())
		{
			cfg_tot+=(c.second).size();
		}
		auto target_set  = node->getAllCallTargets();
		fcg_tot += target_set.size();
		
		for(auto t : target_set)
		{	
			i++;		
			cout<<std::hex<<(n.first)->getName() << " " << (n.first)->getAddress() << " (" << (n.first)->getParent()->getParent()->getName() << ") -> " << std::hex<<t->getFunction()->getName()<< " " << t->getFunction()->getAddress() << " (" << t->getFunction()->getParent()->getParent()->getName() << ")" <<endl;
		}
		nic+=(node->getIcallSite()).size();
		auto atlist = (node->getATList());

		for(auto x : atlist)
		{
			for(auto y : x.second)
				allat.insert(y);
		}
	}
	cout<<"TOTAL EDGES OF CALLGRAPH (FCG) : "<<std::dec<<fcg_tot<<endl;
	cout<<"TOTAL EDGES OF CALLGRAPH (CFG) : "<<std::dec<<cfg_tot<<endl;
	cout<<"TOTAL ICALLS : "<<nic<<endl;
	cout<<"TOTAL RESOLVED ICALLS : "<<resolvedIcalls<<endl;
	cout<<"TOTAL GLOBAL AT FUNCTIONS : "<<allat.size()<<endl;
	cout<<"TOTAL DIRECT EDGES : "<<ndirectedges<<endl;
}

void IPCallGraph::printCallGraph()
{
	int i=0;
	int cfg_tot=0, fcg_tot=0;
	int tot_ic_target=0;
	int tot_ic_callsites=0;
	for(auto n : nodeMap)
	{
		auto node = n.second;
		for(auto c : node->getDirectChildren())
		{
			cfg_tot+=(c.second).size();
		}
		for(auto c : node->getIndirectChildren())
		{
			cfg_tot+=(c.second).size();
			tot_ic_target += (c.second).size();
			tot_ic_callsites++;
		}
		auto target_set  = node->getAllCallTargets();
		fcg_tot += target_set.size();
		
		for(auto t : target_set)
		{	
			i++;		
			cout<<std::hex<<(n.first)->getName() << " " << (n.first)->getAddress() << " (" << (n.first)->getParent()->getParent()->getName() << ") -> " << std::hex<<t->getFunction()->getName()<< " " << t->getFunction()->getAddress() << " (" << t->getFunction()->getParent()->getParent()->getName() << ")" <<endl;
		//cout<<std::hex<<(n.first)->getName() << " -> " << t->getFunction()->getName()<<endl;
		}
	}
	cout<<"TOTAL EDGES OF CALLGRAPH (FCG) : "<<std::dec<<fcg_tot<<endl;
	cout<<"TOTAL EDGES OF CALLGRAPH (CFG) : "<<std::dec<<cfg_tot<<endl;
	cout<<"TOTAL ICALLS : "<<nicalls<<endl;
	cout<<"TOTAL ICALL SITES : "<<tot_ic_callsites<<endl;
	cout<<"TOTAL RESOLVED ICALLS : "<<resolvedIcalls<<endl;
	cout<<"TOTAL RESOLVED ICALL TARGETS : "<<totResolvedIcTarget<<endl;
	cout<<"TOTAL INDIRECT CALL TARGETS "<<tot_ic_target<<endl;
	cout<<"TOTAL GLOBAL AT FUNCTIONS : "<<globalATList.size()<<endl;
	cout<<"TOTAL DIRECT EDGES : "<<ndirectedges<<endl;
	cout<<"TOTAL INDIRECT CALL TARGETS TYPEARMOR : "<<totTypeArmorTarget<<endl;
}

void IPCallGraph::printIndirectEdges()
{
	int i=0;
	for(auto n : nodeMap)
	{
		auto node = n.second;
		auto indirect_ch  = node->getIndirectChildren();
		set<IPCallGraphNode*> target_set;
		for(auto ind : indirect_ch)
		{
			auto indirect_set = ind.second;
			target_set.insert(indirect_set.begin(), indirect_set.end());
		}
		for(auto t : target_set)
		{	
			i++;		
			cout<<std::hex<<(n.first)->getName() << " " << (n.first)->getAddress() << " (" << (n.first)->getParent()->getParent()->getName() << ") -> " << std::hex<<t->getFunction()->getName()<< " " << t->getFunction()->getAddress() << " (" << t->getFunction()->getParent()->getParent()->getName() << ")" <<endl;
		}
	}
	cout<<"TOTAL INDIRECT EDGES OF CALLGRAPH : "<<std::dec<<i<<endl;
}

void IPCallGraph::printDirectEdges()
{
	int i=0;
	for(auto n : nodeMap)
	{
		auto node = n.second;
		auto direct_ch  = node->getDirectChildren();
		set<IPCallGraphNode*> target_set;
		for(auto d : direct_ch)
		{
			auto direct_set = d.second;
			target_set.insert(direct_set.begin(), direct_set.end());
		}
		for(auto t : target_set)
		{	
			i++;		
			cout<<std::hex<<(n.first)->getName() << " " << (n.first)->getAddress() << " (" << (n.first)->getParent()->getParent()->getName() << ") -> " << std::hex<<t->getFunction()->getName()<< " " << t->getFunction()->getAddress() << " (" << t->getFunction()->getParent()->getParent()->getName() << ")" <<endl;
		}
	}
	cout<<"TOTAL DIRECT EDGES OF CALLGRAPH : "<<std::dec<<i<<endl;
}

void IPCallGraph::findData()
{
	for(auto module : CIter::children(this->program))
	{
		auto elfSpace = module->getElfSpace();
		auto symList = elfSpace->getSymbolList();
		LOG(20, "\n Module : "<<module->getName()<<" " <<std::hex<<module->getBaseAddress()<<endl);
		//isModuleVisited[module] = false;
		for(auto region : CIter::regions(module))
		{
			LOG(20, "\n \t Region : "<<region->getName()<<endl);

			for(auto dataSection : CIter::children(region))
			{
				if(dataSection->isData())
				{
					auto it = dsInModule.find(module);
					vector<DataSection*> vec;
					if(it != dsInModule.end())
					{
						vec = it->second;
					}
					
					vec.push_back(dataSection);
					isModuleVisited[module][dataSection] = false;
					dsInModule[module] = vec;
				}
				LOG(20, "\n \t \t Data Section : "<<dataSection->getName()<<" " <<dataSection->getType()<<endl);

				
					LOG(20, "\t \t \t GLOBAL VARIABLES");
                    for(auto gl : dataSection->getGlobalVariables())
                    {
                        LOG(20, "\t \t \t \t "<<gl->getName()<<" "<<std::hex<<gl->getAddress()<<" "<<gl->getSize());
                        tuple<Module*, GlobalVariable*> tup1(module, gl);
                        globalVariables.insert(tup1);
                    }
               
               	LOG(20, "\n \t \t \t DATA VARIABLES");

				for(auto var : CIter::children(dataSection))
				{
					tuple<Module*, DataVariable*> tup1(module, var);
					dataVariables.insert(tup1);
					auto link = var->getDest();
					LOG(20, "\n \t \t \t \t"<<std::hex<<var->getAddress()<<" " <<var->getName()<<" Size "<<var->getSize());
					if(link)
					{
						LOG(20, "TypeID " << typeid(*link).name());
						if(auto li = dynamic_cast<DataOffsetLink *>(&*link))
							LOG(20, "Dataoffset link address "<<link->getTargetAddress());
						if(link->getTarget())
							LOG(20, "Target is a " << typeid(*link->getTarget()).name() << " " <<std::hex<< link->getTargetAddress());
						
						if(auto target = dynamic_cast<Function *>(&*link->getTarget()))
						{	
							LOG(20, "\t \t \t \t Link to func " << target->getName()<<" "<<target->getAddress()<<" "<<link->getTargetAddress());
							
							if(dataSection->getName()==".fini" || dataSection->getName()==".fini_array" || dataSection->getName()==".dtors")
							{
								LOG(20, "FINI Function "<<target->getName()<<" found in "<<dataSection->getName());
								functionRoots.push_back(target);
								finiFuncs.push_back(target);

							}
							if(dataSection->getName()==".preinit_array" || dataSection->getName()==".init_array" || dataSection->getName()==".ctors")
							{
								LOG(20, "INIT Function "<<target->getName()<<" found in "<<dataSection->getName());

								functionRoots.push_back(target);
								initFuncs.push_back(target);
							}
							
						}
						else if(auto target = dynamic_cast<DataSection *>(&*link->getTarget()))
						{
							LOG(20, "\t \t \t \t Link to DataSection " << var->getAddress()<<" : "<<target->getName()<<" "<<target->getType()<<" "<<link->getTargetAddress());

						}
						
					}
				}
			}
		}
		auto init = CIter::named(module->getFunctionList())->find("_init");
        if (init) 
        {
        	functionRoots.push_back(init);
			initFuncs.push_back(init);
        }
        auto fini = CIter::named(module->getFunctionList())->find("_fini");
        if (fini) 
        {
        	functionRoots.push_back(fini);
        	finiFuncs.push_back(fini);
		}
	}
}

Function* IPCallGraph::getFunction(address_t addr, Module* mod)
{
	Function *f = NULL;
	for(auto module : CIter::children(program))
	{
		if(module->getName() != mod->getName())
			continue;
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

void IPCallGraph::parseTypeArmor()
{
	map<string,Module*> moduleMap;
	for(auto mod : CIter::children(program))
	{
		string modname = mod->getName();
		if(modname.find("executable") != string::npos)
		{
			moduleMap["executable"] = mod;
			continue;
		}
		auto pos1 = modname.find("-");
		if(pos1 == string::npos)
			continue;
		auto pos2 = modname.find(".", pos1);
		if(pos2 == string::npos)
			pos2 = modname.length();

		auto temp_name = modname.substr(pos1+1,pos2-pos1-1);
		int i;
		for(i = 0; i < temp_name.length(); i++)
		{
			if(isalpha(temp_name[i]))
				continue;
			else
				break;
		}
		auto fin_name = temp_name.substr(0,i);
		moduleMap[fin_name] = mod;
	}
	for(auto mp : moduleMap)
	{
		LOG(1,"MODULE MAP "<<mp.first<<" "<<(mp.second)->getName());
	}
	ifstream myfile(typeArmorPath);
	string line;
	if(myfile.is_open())
	{
		while(getline(myfile, line))
		{
			Module* cur_mod=NULL;
			Function* cur_fun=NULL;
			address_t icalladdr;
			string pos_flag="reset";
			string type_flag="";
			istringstream ss(line);
			while(ss)
			{
				string word;
				ss >> word;
				//cout<<word<<endl;
				if(pos_flag == "reset")
				{
					pos_flag = "module";
					type_flag = word;
				
				}
				else if(pos_flag == "module")
				{
					bool found = false;
					for(auto mm : moduleMap)
					{
						int i = 0;
						for(i = 0; i < word.length(); i++)
						{
							if(isalpha(word[i]))
								continue;
							else
								break;
						}
						if(i < word.length())
						{
							word = word.substr(0,i);
						}
						if(word == mm.first)
						{
							cur_mod = mm.second;
							found = true;
							break;
						}
					}
					if(!found)
						cur_mod = moduleMap["executable"];
					LOG(1,"MMOD"<<word<<" "<<cur_mod->getName());
					pos_flag = "args";
				}
				else if(pos_flag =="args")
				{
					if(type_flag == "fnargs")
					{
						address_t addr = (address_t)stol(word, NULL, 16);
						cur_fun = getFunction(addr, cur_mod);
						pos_flag = "fnargs1";
					}
					else if(type_flag=="icallargs")
					{
						icalladdr = (address_t)stol(word, NULL, 16);
						pos_flag = "icallargs1";

					}
					else if(type_flag=="nonvoidfn")
					{
						address_t addr = (address_t)stol(word, NULL, 16);
						cur_fun = getFunction(addr, cur_mod);
						if(cur_fun != NULL)
						{
							nonvoidFn.insert(cur_fun);
						}
						pos_flag = "end";
					}
					else if(type_flag=="nonvoidicall")
					{
						address_t addr = (address_t)stol(word, NULL, 16);
						tuple<address_t,Module*> tup1(addr, cur_mod);
						nonvoidIcall.insert(tup1);
						pos_flag = "end";
					}
				}
				else if(pos_flag == "fnargs1")
				{
					int nargs = std::stoi(word);
					if(cur_fun != NULL)
					{
						functionNargs[cur_fun] = nargs;

					}
					pos_flag = "end";
				}
				else if(pos_flag == "icallargs1")
				{
					int nargs = std::stoi(word);
					tuple<address_t,Module*> tup1(icalladdr, cur_mod);
					icallNargs[tup1] = nargs;
					pos_flag = "end";
				}
				else if(pos_flag == "end")
				{
					break;
				}
			}
		}
	}
	myfile.close();	
}

set<Function*> IPCallGraph::getFunctionByAddress(address_t addr)
{
	set<Function*> func_set;
	for(auto module : CIter::children(program))
	{
		for(auto func : CIter::functions(module))
		{
			if(func->getAddress() == addr)
			{
				func_set.insert(func);
			}
		}
	}
	return func_set;
}
void IPCallGraph::generateIndirectEdges(IPCallGraphNode* n)
{
	auto ics = n->getIndirectCallSites();
	set<IPCallGraphNode*> s;
	for(auto f : globalATList)
	{
		auto node_iter = nodeMap.find(f);
		IPCallGraphNode* node_f;
		if(node_iter == nodeMap.end())
		{
			node_f = new IPCallGraphNode(f);
			df.getWorkingSet(f);
			LOG(15, f<<" "<<node_f<<" "<<std::hex<<f->getAddress()<<" "<<f->getName());
			nodeMap[f] = node_f;
		}
		else
			node_f = (node_iter)->second;
		s.insert(node_f);
	}
	
	auto cur_func = n->getFunction();
	for(auto bl : CIter::children(cur_func))
	{
		for(auto instr : CIter::children(bl))
		{
			auto addr = instr->getAddress();
			if(ics.count(addr) == 0)				//Not an indirect call
				continue;
			if(auto ici = dynamic_cast<IndirectCallInstruction*>(instr->getSemantic()))
			{
				nicalls++;
				if(icanalysisFlag)
				{
					if(n->isIcallResolved(addr))
					{
						resolvedIcalls++;
						auto inc = n->getIndirectChildren();
						for(auto i : inc)
						{
							if(i.first != addr)
								continue;

							for(auto ict : i.second)
							{
								totResolvedIcTarget++;
							}

						}
						continue;
					}
						
				}
				n->setIcallResolved(addr, false);

				if(!typeArmorFlag)							//If TypeArmor not enabled
				{
					n->insertCallTargetSet(addr, false, s);
					for(auto node_f : s)
					{
						node_f->insertParent(addr, n, false);
					}
					continue;
				}
				else
					generateIndirectEdgesWithTypeArmor(n, addr,  s);	
			}
			else //Not an indirect call instruction, but can be datalinked instruction or an indirect jump instruction
			{
				nicalls++;
				n->setIcallResolved(addr, false);
				if(!typeArmorFlag)							//If TypeArmor not enabled
				{
					n->insertCallTargetSet(addr, false, s);
					for(auto node_f : s)
					{
						node_f->insertParent(addr, n, false);
					}
					continue;
				}
				else
					generateIndirectEdgesWithTypeArmor(n, addr,  s);
			}
		}
	}
} 

void IPCallGraph::pruneIndirectEdges(IPCallGraphNode* node)
{
	
                auto  module = node->getFunction()->getParent()->getParent();
                auto indirect_children = node->getIndirectChildren();

                for(auto ind : indirect_children)
                {
                	auto addr = ind.first;
                	auto node_set = ind.second;
                	set<IPCallGraphNode*> s;
                	for(auto nn : node_set)
                	{

                		auto mod = nn->getFunction()->getParent()->getParent();
                		if(module->getName() == mod->getName())
                			s.insert(nn);
                		else
                			nn->removeParent(node);
                	}
                	ind.second = s;
                	node->updateIndirectChildren(addr,s);
                }
	
}

void IPCallGraph::generateIndirectEdgesWithTypeArmor(IPCallGraphNode* n, address_t addr, set<IPCallGraphNode*> at)
{
	auto module = (Module*)n->getFunction()->getParent()->getParent();
	
	bool found = false;
	int nargs;
	for(auto ic : icallNargs)
	{
		address_t icall_addr;
		Module* icall_mod;
		tie(icall_addr, icall_mod) = ic.first;
		if(icall_addr == addr && icall_mod->getName() == module->getName())
		{
			found = true;
			nargs = ic.second;
		}
	}

	if(!found)			//Couldn't find nargs for this icall
	{
		//LOG(1,"UNRESOLVED TA ICALL "<<std::hex<<addr<<" "<<module->getName());
		n->insertCallTargetSet(addr, false, at);
		for(auto node_f : at)
		{
			node_f->insertParent(addr, n, false);
		}
		totTypeArmorTarget+=at.size();

		return;
	}
	bool nonvoid = false;
	for(auto nv : nonvoidIcall)
	{
		address_t nv_addr;
		Module* nv_mod;
		tie(nv_addr, nv_mod) = nv;
		if(nv_addr == addr && nv_mod->getName() == module->getName())
		{
			nonvoid = true;
			break;
		}
	}
	
	set<IPCallGraphNode*> pruned_set;
	for(auto atexec : at)
	{
		auto nargs_at_iter = functionNargs.find(atexec->getFunction());
		if(nargs_at_iter == functionNargs.end())	//Don't have nargs for this function
		{
			pruned_set.insert(atexec);
		}
		auto nargs_at = nargs_at_iter->second;
		if(nargs_at > nargs)	//Function expects more args than what is passed through icall
			continue;
		if(nonvoid)	//Icall is non-void
		{
			if(nonvoidFn.count(atexec->getFunction()) == 0)	//Function is void
				continue;
		}
		pruned_set.insert(atexec);
	}
	//cout<<"Pruned : "<<std::hex<<addr<<" "<<std::dec<<pruned_set.size()<<" "<<at.size()<<endl;
	n->insertCallTargetSet(addr, false, pruned_set);
	for(auto node_f : pruned_set)
	{
		node_f->insertParent(addr, n, false);
	}
	totTypeArmorTarget+=pruned_set.size();

}

void IPCallGraph::findDirectEdges(Function* f)
{
	if(visited_direct.count(f) != 0)
		return;

	for(auto bl : CIter::children(f))
	{
		for(auto instr : CIter::children(bl))
		{
			auto semantic = instr->getSemantic();
			if(auto cfi = dynamic_cast<ControlFlowInstruction *>(semantic))	//Handling control flow instruction(direct edges)
			{
				auto link = cfi->getLink();
				auto target = link->getTarget();

				if(auto func_target = dynamic_cast<Function *>(target))	//Call to a local function
				{
					addEdge(instr->getAddress(), f, func_target, true);
					ndirectedges++;
					addFunctionRoot(func_target);
					LOG(5, "DIR_FUNC "<<std::hex<<f->getAddress()<<" "<<f->getName()<<" "<<std::hex<<func_target->getAddress()<<" "<<func_target->getName()<<" "<<instr->getAddress());
				}
				else if(auto plt = dynamic_cast<PLTTrampoline *>(target)) //Call to a library function called through PLt
				{
					if (auto ext_target = dynamic_cast<Function *>(plt->getTarget())) 
					{
						if(ext_target)
						//cout<<"PLT Target "<<endl;
						addEdge(instr->getAddress(), f, ext_target, true);
						ndirectedges++;
						addFunctionRoot(ext_target);
						LOG(5, "PLT "<<std::hex<<f->getAddress()<<" "<<std::hex<<ext_target->getAddress()<<" "<<f->getName()<<" "<<ext_target->getName());

					}
				}
				else if (auto i = dynamic_cast<Instruction *>(target)) //Call/jump to an instruction
				{
					auto gp = static_cast<Function *>(i->getParent()->getParent());
					if(gp)
					{
						if(f->getName() != gp->getName())
						{
							LOG(5, "CFI at "<<f->getName()<<" : "<<instr->getAddress()<<" pointing to "<<i->getAddress()<<endl);
							addEdge(instr->getAddress(), f, gp, true);
							ndirectedges++;
						}
					}
				}
			}
		
		}
	}

	visited_direct.insert(f);

}

bool IPCallGraph::handleArgumentFnPtr(int reg, Function* f, Instruction* instr, Function* atfunc)
{
	auto ipnode = getNode(f);
	if(f == NULL)
	{
		return false;
	}
	auto dir_ch = ipnode->getDirectChildren();
	auto dir_it = dir_ch.find(instr->getAddress());
	bool flag = true;
	bool found = false;
	if(dir_it != dir_ch.end())
	{
		auto dir_ch_set = dir_it->second;
		for(auto s : dir_ch_set)
		{
			auto ch_func = s->getFunction();
			auto graph = new ControlFlowGraph(ch_func);
    		auto config = new UDConfiguration(graph);
    		auto working = new UDRegMemWorkingSet(ch_func, graph);
    		auto usedef =  new UseDef(config, working);
    		SccOrder order(graph);
    		order.genFull(0);
    		usedef->analyze(order.get());

    		UDState* startState=NULL;
    		typedef TreePatternCapture<TreePatternTerminal<TreeNodePhysicalRegister>>RegisterValue;
    		
    		found = false;
    		for(auto bl : CIter::children(ch_func))
    		{
    			for(auto ins : CIter::children(bl))
    			{
    				auto state = working->getState(ins);
    				for(auto& def : state->getRegDefList())
    				{
    					TreeCapture cap;
 						if(RegisterValue::matches(def.second, cap))
 						{
 							
 							auto reg2 = dynamic_cast<TreeNodePhysicalRegister *>(cap.get(0))->getRegister();

 							if(reg2 == reg)					//First instruction where reg is referenced
 							{
 								startState = state;			//State found
 								auto use1 = state->getRegUse(def.first);
 								auto use2 = state->getRegUse(reg2);
 								for(auto u : use1)
 								{
 									auto tempFlag = searchDownDef(u, def.first, atfunc);
            						LOG(20, std::hex<<instr->getAddress()<<" Received "<<tempFlag);
            						flag = tempFlag & flag;
 								}
 								for(auto u : use2)
 								{
 									auto tempFlag = searchDownDef(u, reg2, atfunc);
            						LOG(20, std::hex<<instr->getAddress()<<" Received "<<tempFlag);
            						flag = tempFlag & flag;
 								}
 								found = true;
 								break;
 							}
 						}
    				}
    				if(!found)
    				{
    					auto treenode = state->getMemDef(reg);
    					if(treenode != NULL)					//Assigned to a memory address, return false
    					{
    						return false;
    					}
    				}
    				else
    					break;

    			}
    			if(found)
    				break;
    		}
    	}
	}
	if(!found)
		return false;
	return flag;
}

bool IPCallGraph::searchDownDef(UDState* state, int reg1, Function* atfunc)
{
   
    for(auto v : visited_states)
    {
    	Instruction* ins;
    	int rr;
    	tie(ins, rr) = v;
    	if((ins == state->getInstruction()) && (rr == reg1))
    		return true;
    }
    tuple<Instruction*,int> tup1(state->getInstruction(),reg1);
	visited_states.insert(tup1);
    auto instr = state->getInstruction();
    if(dynamic_cast<ReturnInstruction *>(instr->getSemantic())) //DF ends in a return statement
    {
    	if(reg1 == 0) //RAX
    	{
    		return false;
    	}
        return true;
    }
    else if(auto ici =  dynamic_cast<IndirectCallInstruction *>(instr->getSemantic())) //DF ends in an indirect call
    {
        if(reg1 == 7 || reg1 == 6 || reg1 == 2 || reg1 == 1 || reg1 == 8 || reg1 == 9) //Argument to indirect call?
        {
            return false;
        }
        else  						//Flowing into an indirect call. Adding as an edge at this indirect call
        {
        	addEdge(instr->getAddress(), (Function*)instr->getParent()->getParent(), atfunc, false);
        	return true;
        }
        
    }
    else if(dynamic_cast<DataLinkedControlFlowInstruction *>(instr->getSemantic())) 
    {
        if(reg1 == 7 || reg1 == 6 || reg1 == 2 || reg1 == 1 || reg1 == 8 || reg1 == 9) //Argument to data link instruction
        {
            return false;
        }
        else 
        {                                         //If any other register, then it is actually not used in this instruction
        }
        return true;
    }
    if(auto cfi = dynamic_cast<ControlFlowInstruction *>(instr->getSemantic())) //DF ends in a call/jmp
    {
        auto mnemonic = cfi->getMnemonic();
        if(mnemonic.find("call") == string::npos)   //Not a call instruction
        {
            return false;
        }
        if(reg1 == 7 || reg1 == 6 || reg1 == 2 || reg1 == 1 || reg1 == 8 || reg1 == 9) //Argument to CFI?
        {
        	bool res = handleArgumentFnPtr(reg1, (Function*)instr->getParent()->getParent(), instr, atfunc);
            return res;
        }

        
        return true;
    }
    

    bool regFlag = false;
    bool flag = true;
    int reg=-1;
    for(auto& def : state->getRegDefList())                      //Register definition is found
    {
        regFlag = true;
       
        
        reg = def.first;
        if(reg1 != -2)  //Handling the starting call of this function when reg1 value is not set
        {
            typedef TreePatternCapture<TreePatternTerminal<TreeNodePhysicalRegister>>RegisterValue;
            TreeCapture cap;
            if(RegisterValue::matches(def.second, cap)) 
            {
                auto reg2 = dynamic_cast<TreeNodePhysicalRegister *>(cap.get(0))->getRegister();
                if(reg2 != reg1)
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
            
        }
        auto reguse = state->getRegUse(reg);
        
        for(auto use : reguse)
        {
            auto tempFlag = searchDownDef(use, reg, atfunc);
            flag = tempFlag & flag;
        }
    }
    if(regFlag)
    {
        	
        return flag;
    }
    auto memdef = state->getMemDefList();
    bool memFlag = false;
    flag = true;
    for(auto def : memdef)                  //If memory definition is found, return false
    {	
    	if(def.first != reg1)				//Iff reg1 not used in memory definition, then we can return true 
    		flag = true;
    	else
        	flag = false;
    }
    if(flag)
    {
        //cout<<std::hex<<instr->getAddress()<<" MEMDEF TRUE "<<endl;
    }
    return flag;                            //If none of the above cases match, return true;
}

bool IPCallGraph::forwardDataFlow(Function* f, Instruction* instr, Function* atfunc)
{
	auto graph = new ControlFlowGraph(f);
    auto config = new UDConfiguration(graph);
    auto working = new UDRegMemWorkingSet(f, graph);
    auto usedef =  new UseDef(config, working);
    SccOrder order(graph);
    order.genFull(0);
    usedef->analyze(order.get()); 
	auto state = working->getState(instr);

	

    auto res = searchDownDef(state, -2, atfunc);    //We don't have a register value to pass, so passing -2
    visited_states.clear();
    return res;
}

void IPCallGraph::findATList(Function* f)
{
	LOG(20, "Finding AT list of "<<f->getName()<<" "<<f->getAddress());
	if(visited_direct.count(f) == 0)
		findDirectEdges(f);

	if(visited_AT.count(f) != 0)
		return;
	for(auto bl : CIter::children(f))
	{
		for(auto instr : CIter::children(bl))
		{
			auto semantic = instr->getSemantic();
			if (auto cfi = dynamic_cast<ControlFlowInstruction *>(semantic)) {
                        LOG(20, "Control Flow Instruction : "<<std::hex<<instr->getAddress());

                continue;
            }

           
			else if(auto li = dynamic_cast<LinkedInstruction *>(semantic))
			{
				auto link = li->getLink();
				LOG(20, "linked instructin "<<instr->getAddress());
				LOG(20, "LINK TYPE " << typeid(*link).name());
				if(!link)
					continue;
				
				auto target = link->getTarget();
				auto targetAddress = link->getTargetAddress();
				LOG(20, "Target Address : " <<std::hex<<targetAddress);

				if(auto func_target = dynamic_cast<Function *> (target))	
				{
					LOG(20, "AT func at : "<<std::hex<<instr->getAddress()<<" targetting func "<<func_target->getAddress()<<" "<<func_target->getName());
					
					addATFunction(instr->getAddress(), f, func_target);
					addFunctionRoot(func_target);
					LOG(5, " "<<std::hex<<func_target->getAddress()<<" "<<func_target->getName()<<" DIRECT FUNC");
					
					if(!icanalysisFlag)
					{
						globalATList.insert(func_target);
					}
					else
					{
						auto res = forwardDataFlow(f, instr, func_target);
						//cout<<"Received  Flag : "<<res<<endl;
						if(!res)		//Cannot be filtered
						{
							globalATList.insert(func_target);
						}
						
					}
				}
				else if(auto plt = dynamic_cast<PLTTrampoline *>(target))
				{
					if(auto plt_target = dynamic_cast<Function *>(plt->getTarget()))
					{
						LOG(20, "AT func at : "<<std::hex<<instr->getAddress()<<" targetting plt "<<plt_target->getAddress()<<" "<<plt_target->getName());
						
						addATFunction(instr->getAddress(), f, plt_target);
						addFunctionRoot(plt_target);
						LOG(5, " "<<std::hex<<plt_target->getAddress()<<" "<<plt_target->getName()<<" DIRECT PLT");
						if(!icanalysisFlag)
						{
							globalATList.insert(plt_target);
						}
						else
						{
							auto res = forwardDataFlow(f, instr, plt_target);
							if(!res)		//Cannot be filtered
							{
								globalATList.insert(plt_target);
							}
						}
					}
					else
					{
						LOG(1, "plt "<<plt->getName()<<" not resolved to a function in "<<f->getName());
					}
				}
				else if(DataSection *ds = dynamic_cast<DataSection *>(target))
				{
					LOG(20, "Refering a datasection "<<ds->getName());
					set<address_t> visited;
					auto mod = (Module*)f->getParent()->getParent(); 
             		auto it = find(modulesWithSymbols.begin(), modulesWithSymbols.end(), mod);
             		if(it != modulesWithSymbols.end())
             		{
             			LOG(20, "Parse data");
             			
						parseData(mod, instr, f, ds, targetAddress, &visited);
             		}
					else
					{
             			LOG(20, "Parse data without symbols");
             			
						parseDataWithNoSymbols(mod, instr->getAddress(), f, ds);
					}					
				}
			}

			if( auto ici =  dynamic_cast<IndirectCallInstruction *>(semantic))
			{
                addIndirectSource(instr->getAddress(), f);
            }
            else if(auto iji = dynamic_cast<IndirectJumpInstruction *>(semantic))
            {
                if(!iji->isForJumpTable()) 
                {
                	addIndirectSource(instr->getAddress(), f);
            	}
            }
			
            else if(auto dlcfi = dynamic_cast<DataLinkedControlFlowInstruction *>(semantic)) 
            {
             	LOG(20, "Data linked control flow instruction : "<<std::hex<<instr->getAddress());
             	auto link = dlcfi->getLink();
             	if(!link)
             		continue;
             	LOG(20, "dl LINK TYPE "<<typeid(*link).name());
             	auto target = link->getTarget();
             	if(!target)
             		continue;
             	auto targetAddress = link->getTargetAddress();

             	addIndirectSource(instr->getAddress(), f);
             	if(DataSection *ds = dynamic_cast<DataSection *> (target))
             	{
             		LOG(20, "Target DL"<<typeid(*target).name()<<std::hex<<" "<<targetAddress);
             		set<address_t> visited;
             		auto mod = (Module*)f->getParent()->getParent(); 
             		auto it = find(modulesWithSymbols.begin(), modulesWithSymbols.end(), mod);
             		if(it != modulesWithSymbols.end())
             		{
             			LOG(20, "Parse data");
             			
						parseData(mod, instr, f, ds, targetAddress, &visited);
             		}
					else
					{
						LOG(20, "Parse data without symbols");
						
						parseDataWithNoSymbols(mod, instr->getAddress(), f, ds);
					}
				}
				else
				{
					LOG(20, "DIFFERENT "<<typeid(*target).name()<<std::hex<<" "<<targetAddress);
				}
             	
            }
		}
	}
	visited_AT.insert(f);


}

void IPCallGraph::parseDataWithNoSymbols(Module* module, address_t instrAddr, Function* f, DataSection *ds)
{
	if(ds->getType() != DataSection::TYPE_DATA)
	{
		LOG(20, "DataSection "<<ds->getName()<<" not DATA");
		return;
	}
	//Find all function pointers in data section
	auto pos = isModuleVisited.find(module);
	bool fl = false;
	if(pos != isModuleVisited.end())
	{
		fl = isModuleVisited[module][ds];
	}
	if(fl == false)
	{
		set<Function*> s;
		auto total_ds = dsInModule[module];
		DataSection *dsVec[total_ds.size()];
		dsVec[0] = ds;
		LOG(20, "Adding "<<ds->getName()<<" at 0");

		int next = 1;
		for(int i = 0; i<next; i++)
		{
			auto d = dsVec[i];
			auto pos = isModuleVisited.find(module);
			if(pos != isModuleVisited.end())
				if(isModuleVisited[module][ds])
					continue;
			LOG(20, "Adding all AT from section " << d << " " << d->getName()<<" in module "<<module->getName()<<" "<<total_ds.size());

			for(auto dv : CIter::children(d))
			{

				auto dvLink = dv->getDest();
				if(dvLink)
				{
					auto dvTarget = dvLink->getTarget();
					if(dvTarget)
					{
						if(Function* global_fun = dynamic_cast<Function*>(dvTarget))
						{
							addFunctionRoot(global_fun);
							LOG(5, " "<<std::hex<<global_fun->getAddress()<<" "<<global_fun->getName()<<" ALL DATA");
							globalATList.insert(global_fun);
							s.insert(global_fun);
						}
						else if(auto plt = dynamic_cast<PLTTrampoline *>(dvTarget))
						{
							if(auto plt_target = dynamic_cast<Function *>(plt->getTarget()))
							{
								addFunctionRoot(plt_target);
								LOG(5, " "<<std::hex<<plt_target->getAddress()<<" "<<plt_target->getName()<<" ALL DATA");
								globalATList.insert(plt_target);
								s.insert(plt_target);
							}
							else
							{
								LOG(5, "plt "<<plt->getName()<<" not resolved to a function in "<<f->getName());
							}
						}
						else if(auto data_section = dynamic_cast<DataSection *>(dvTarget))
						{
							if(data_section->getType() != DataSection::TYPE_DATA)
								continue;
							bool exisiting_ds = false;
							for(int j = 0; j <next ; j++)
							{
								if(dsVec[j] == data_section)
									exisiting_ds = true;
							}
							if(!exisiting_ds)
							{
								LOG(20, "Adding "<<data_section->getName()<<" at "<<next);
								dsVec[next++] = data_section;
								for(int k=0; k<next; k++)
								{
									LOG(20, "DS Array "<<k);
									if(dsVec[k] !=NULL)
									{
										LOG(20, "DS Array "<<dsVec[k]);
										LOG(20, "DS Array "<<dsVec[k]->getName());
									}
									else
										LOG(20, "DS Array NULL");
									LOG(20, "DS Array "<<module->getName());


								}
							}
						}
					}
				}
			}
			isModuleVisited[module][ds] = true;
		}
		ATInData[module][ds] = s;
	}
	for(auto e : ATInData[module][ds])
	{
		addATFunction(instrAddr, f, e);
	}

}

void IPCallGraph::parseData(Module* module, Instruction* instr, Function* f, DataSection *ds, address_t targetAddress, set<address_t> *visited)
{
	address_t instrAddr = instr->getAddress();
	if(visited->count(targetAddress) != 0)
	{
		LOG(20, "Target address "<<targetAddress<<" already visited");
		return;
	}
	if(ds->getType() != DataSection::TYPE_DATA)
	{
		LOG(20, "DataSection "<<ds->getName()<<" not DATA");
		return;
	}
	LOG(20,"Search for "<<std::hex<<targetAddress<<" from "<<std::hex<<instrAddr<<" in DS "<<ds->getName()<<" "<<ds->getType());
	LOG(20, "module is "<<module->getName());
	visited->insert(targetAddress);
	address_t start_addr = targetAddress;
	address_t end_addr = targetAddress; 
	bool isGlobal = false;
	for(auto gl : ds->getGlobalVariables())
    {
    	
        auto st = gl->getAddress();
        auto en = gl->getAddress() + gl->getSize();
        if(targetAddress >= st && targetAddress < en)
        {
            LOG(20, "Target address "<<targetAddress<<" is in globalvar "<<gl->getName());
            start_addr = st;
            end_addr = en;
            isGlobal = true;
            break;
        }
    }
    for(auto dv : CIter::children(ds))
    {
    	
    	if(isGlobal)
    	{
    		if(dv->getAddress() >= start_addr && dv->getAddress() < end_addr)
    		{
    			LOG(20, "GLOBAL Variable address "<<module->getName());
    			auto dvLink = dv->getDest();
    			if(dvLink)
    			{
    				auto dvTarget = dvLink->getTarget();
    				if(dvTarget)
    				{
    					auto dvTargetAddr = dvLink->getTargetAddress();
    					if(Function* global_fun = dynamic_cast<Function*>(dvTarget))
						{
							LOG(20, "AT func at "<<instrAddr<<" in fun "<<f->getName()<<" AT function "<<global_fun->getAddress()<<" "<<global_fun->getName());
							addATFunction(instrAddr, f, global_fun);
							addFunctionRoot(global_fun);
							LOG(5, " "<<std::hex<<global_fun->getAddress()<<" "<<global_fun->getName()<<" DATA REG SYM");
							globalATList.insert(global_fun);							
						}
						else if(auto plt = dynamic_cast<PLTTrampoline *>(dvTarget))
						{
							if(auto plt_target = dynamic_cast<Function *>(plt->getTarget()))
							{
								LOG(20, "AT func at "<<std::hex<<instrAddr<<" in fun "<<f->getName()<<" AT function "<<plt_target->getAddress()<<" "<<plt_target->getName());
								addATFunction(instrAddr, f, plt_target);
								addFunctionRoot(plt_target);
								LOG(5, " "<<std::hex<<plt_target->getAddress()<<" "<<plt_target->getName()<<" DATA REG PLT SYM");
								globalATList.insert(plt_target);								
							}
							else
							{
								LOG(1, "plt "<<plt->getName()<<" not resolved to a function in "<<f->getName());
							}
						}
						else if(DataSection* global_data = dynamic_cast<DataSection*>(dvTarget))
						{
							LOG(20, "Addr "<<targetAddress<<" refers to "<<dvTargetAddr<<endl);
							parseData(module, instr, f, global_data, dvTargetAddr, visited);
						}
    				}
    			}
    		}
		}
    	else
    	{
    		if(dv->getAddress() == targetAddress)
    		{
    			LOG(20, "UNGLOBAL Variable address "<<ds->getName()<<" " <<module->getName()<<" "<<targetAddress);
				auto dvLink = dv->getDest();
    			if(dvLink)
    			{
    				auto dvTarget = dvLink->getTarget();
    				if(dvTarget)
    				{
    					auto dvTargetAddr = dvLink->getTargetAddress();
    					LOG(20, "DV Target Address : "<<std::hex<<dvTargetAddr);
    					if(Function* global_fun = dynamic_cast<Function*>(dvTarget))
						{
							LOG(20, "AT func at "<<instrAddr<<" in fun "<<f->getName()<<" AT function "<<global_fun->getAddress()<<" "<<global_fun->getName());
							addATFunction(instrAddr, f, global_fun);
							addFunctionRoot(global_fun);
							LOG(5, " "<<std::hex<<global_fun->getAddress()<<" "<<global_fun->getName()<<" DATA REG");
							globalATList.insert(global_fun);
						}
						else if(auto plt = dynamic_cast<PLTTrampoline *>(dvTarget))
						{
							if(auto plt_target = dynamic_cast<Function *>(plt->getTarget()))
							{
								LOG(20, "AT func at "<<std::hex<<instrAddr<<" in fun "<<f->getName()<<" AT function "<<plt_target->getAddress()<<" "<<plt_target->getName());
								addATFunction(instrAddr, f, plt_target);
								addFunctionRoot(plt_target);								
								LOG(5, " "<<std::hex<<plt_target->getAddress()<<" "<<plt_target->getName()<<" DATA REG PLT");
								globalATList.insert(plt_target);
							}
							else
							{
								LOG(1, "plt "<<plt->getName()<<" not resolved to a function in "<<f->getName());
							}
						}
						else if(DataSection* global_data = dynamic_cast<DataSection*>(dvTarget))
						{
							LOG(20, "Addr "<<targetAddress<<" refers to "<<dvTargetAddr<<endl);
							parseData(module, instr, f, global_data, dvTargetAddr, visited);
						}
    				}
    			}
    		}
    	}
    }
}

void IPCallGraph::printNodeInfo()
{
	
	for(auto m : nodeMap)
    {
    	auto f = m.first;
    	auto s = m.second;
        cout<<f->getAddress()<<" "<<std::hex<<" "<<f<<" "<<f->getName()<<" "<<s<<endl;
    }
	
	stack<IPCallGraphNode*> st;
	auto start_node = nodeMap[startfunc];
	vector<IPCallGraphNode*> visited_nodes;
	st.push(start_node);
	while(!st.empty())
	{
		auto n = st.top();
		st.pop();
		auto it = find(visited_nodes.begin(), visited_nodes.end(), n);
		if(it != visited_nodes.end())
			continue;
		cout<<"\n\nFunc : "<<n->getFunction()->getName()<<endl;
		cout<<"AT functions : ";
		for(auto at : n->getATList())
		{
			auto addr = at.first;
			for(auto at_node : at.second)
			{
				cout<<std::hex<<addr<<" "<<(at_node->getFunction())->getName()<<" ";
			}
		}
		
		if ((n->hasIndirectCall()) > 0)
			cout<<"\nIndirect Call : YES";
		else
			cout<<"\nIndirect Call : NO";
		
		cout<<"\nChildren :";
		for(auto ch : n->getAllCallTargets())
		{
			cout<<(ch->getFunction())->getName()<<" ";
			st.push(ch);
		}
		
		cout<<"\nParents : ";
		for(auto p : n->getParent())
		{
			cout<<(p->getFunction())->getName()<<" ";
		}
		cout<<"\n Direct children : ";
		for(auto d : n->getDirectCallTargets())
		{
			cout<<(d->getFunction())->getName()<<" ";
		}
		
		visited_nodes.push_back(n);
	}

}

void IPCallGraph::generateDirectCallGraph()
{
	checkForSymbols();
	findData();
	while(functionRoots.size())
	{
		auto it = functionRoots.begin();
		auto next = *it;
		functionRoots.erase(it);
		findDirectEdges(next);
	}
	copy(visited_direct.begin(), visited_direct.end(), back_inserter(functionRoots)); 
	
		while(functionRoots.size())
		{
			auto it = functionRoots.begin();
			auto next = *it;
			functionRoots.erase(it);
			findATList(next);
		}
}

void IPCallGraph::generate()
{
	checkForSymbols();
	findData();

	while(functionRoots.size())
	{
		auto it = functionRoots.begin();
		auto next = *it;
		functionRoots.erase(it);
		findDirectEdges(next);
	}

	copy(visited_direct.begin(), visited_direct.end(), back_inserter(functionRoots)); 
	while(functionRoots.size())
	{
		auto it = functionRoots.begin();
		auto next = *it;
		functionRoots.erase(it);
		findATList(next);
	}

	if(icanalysisFlag)
	{
		SyspartUtility spUtil(program, this,1);
		spUtil.initialize();
		bool resolved  = false;
		do
		{
			resolved = false;		
			spUtil.prev_resolvedFns = spUtil.new_resolvedFns;
			spUtil.new_resolvedFns.clear();
			spUtil.iter++;
			for(auto n : nodeMap)
			{
				bool fl = spUtil.findIndirectCallTargets(n.second);
				resolved = resolved | fl;
			}
		}while(resolved);
	}
	if(typeArmorFlag)
	{
	    parseTypeArmor();	
	   
	}
	for(auto n : nodeMap)
	{
		generateIndirectEdges(n.second);
		
	}
	
	
	
}

void IPCallGraph::checkForSymbols()
{	
	for(auto module : CIter::children(this->program))
	{
		auto elfSpace = module->getElfSpace();
		auto symList = elfSpace->getSymbolList();
		auto dynsymList = elfSpace->getDynamicSymbolList();
		auto relocList = elfSpace->getRelocList();
		LOG(20, "\n MODULE : "<<module->getName()<<"\n");
		LOG(20, "\n SYM LIST : \n");
		if(symList != NULL)
		{
		modulesWithSymbols.push_back(module);
		for(auto it = symList->begin(); it != symList->end(); it++)
		{
			auto s = (*it);
			LOG(20, "\n"<<std::hex<<s->getAddress()<<"\t"<<s->getName()<<"\t"<<s->getType()<<"\t"<<s->getSize());
		}
		}
		LOG(10, "\n*****DYN SYM LIST*** \n");
		if(dynsymList != NULL)
		{
		for(auto it = dynsymList->begin(); it != dynsymList->end(); it++)
		{
			auto s = (*it);
			LOG(20, "\n"<<std::hex<<s->getAddress()<<"\t"<<s->getName()<<"\t"<<s->getType()<<"\t"<<s->getSize());
		}
		}

		LOG(20, "\n*****RELOC LIST*** \n");
		if(relocList != NULL)
		{
		for(auto it = relocList->begin(); it != relocList->end(); it++)
		{
			auto r = (*it);
			LOG(20, "\n REL "<<std::hex<<r->getAddress()<<"\t"<<r->getType()<<"\t"<<r->getAddend());
			auto s = r->getSymbol();
			if( s != NULL)
			{
				LOG(20, "\n SYM "<<std::hex<<s->getAddress()<<"\t"<<s->getName()<<"\t"<<s->getType()<<"\t"<<s->getSize());
			}
		}
		}


	}
}

Function* IPCallGraph::getNSSFunctionByName(string name)
{
	Function* func_set=NULL;
	for(auto module : CIter::children(program))
	{
		
   		if(module->getLibrary()->getRole() != Library::ROLE_LIBC)
   			continue;
		for(auto func : CIter::functions(module))
		{
			if(func->getName() == name)
			{
				func_set = func;
				break;
			}
		}
	}
	return func_set;
}
void IPCallGraph::resolveNss(ConductorSetup* setup) {
    
    setup->getConductor()->acceptInAllModules(&nss, false);

    // get libc library
    auto program = setup->getConductor()->getProgram();
    if (program->getLibc() == nullptr) {
        LOG(1, "No libc found, skipping NSS detection --- parsing loader?");
        return; 
    }
    auto libc = program->getLibc()->getLibrary();
    ElfDynamic edyn(program->getLibraryList());
    for(auto &ns : nss.ns_info)
    {
    	Module* mod = ns.module;
    	string ext_sym_name = ns.ext_symb;	//ext_sym_name is an external symbol in Module mod   	
    	auto libneeded = ns.libneeded;
		for (auto needed : libneeded) 
		{
			std::string soname = "libnss_" + needed.first + ".so.2";	//ext_sym_name is found in nss library soname
        	edyn.addDependency(libc, soname);
        	setup->getConductor()->parseLibraries();
        	auto library = program->getLibraryList()->find(soname);
        	if (!library) continue;
        	auto module = library->getModule();
        	set<Function*> needed_funcs;
        	for (auto fname : needed.second) 
        	{ 
            	auto func = CIter::named(module->getFunctionList())->find(fname);
            	if (!func) 
            	{
                	LOG(1,"Looked for function with name " << fname
                                                     << ", result: " << func);
                	func = CIter::named(module->getFunctionList())->find(fname + "_r");
            	}
            	LOG(1,
                "Looked for function with name " << fname
                                                 << ", result: " << func);
            	// hopefully we found one of those two, but it may not be present
            	// in this NSS library.
            	if (!func) continue;
            	addFunctionRoot(func);
            	addNssFunc(func);
            	needed_funcs.insert(func);
        	}
        	
        	ns.resolved_nss_funcs[module] = needed_funcs;
		} 	
    }
  

    ResolvePLTPass resolvePLT(setup->getConductor());
    program->accept(&resolvePLT);

    CollapsePLTPass collapsePLT(setup->getConductor());
    program->accept(&collapsePLT);

   

}

void IPCallGraph::addNssEdges()
{
	auto ns_info = nss.getNsInfo();
	for(auto ns : ns_info)
    {
    	
    	Module* mod = ns.module;
    	string ext_sym_name = ns.ext_symb;	//ext_sym_name is an external symbol in Module mod   	
    	auto libneeded = ns.libneeded;
    	auto resolvedFns = ns.resolved_nss_funcs;
    	auto nss_func_obj = getNSSFunctionByName(ext_sym_name);
    	if(nss_func_obj)
    	{
	    	auto nss_ip_node = getNode(nss_func_obj);
	      	if(nss_ip_node)
	      	{
	        	auto parent_callsites = nss_ip_node->getParentCallSites();
	        	for(auto pp : parent_callsites)
	        	{
	          		auto parent_addr = pp.first;
	          		auto parent_node = pp.second;

	          		for(auto rr : resolvedFns)
    		  		{
    		   			for(auto needed : rr.second)
    		   			{
	    		  			//cout<<"NSS Resolved Func : "<<needed->getName()<<"\n";
    			   			addEdge(parent_addr, parent_node->getFunction(), needed, true);
    			   			//cout<<"\nNSS Edge from "<<parent_node->getFunction()<<" -> "<<needed->getName()<<"\n";
    		   			}
    		  		}
	        	}
	      	}
	    }
	    else
	    {
	    	LOG(1,"\nFunction with "<<ext_sym_name<<" not found\n");
	    }

    }
}

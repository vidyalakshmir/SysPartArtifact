#ifndef IPCALLGRAPH_ANALYSIS
#define IPCALLGRAPH_ANALYSIS

#include "conductor/setup.h"
#include "chunk/concrete.h"
#include "analysis/usedef.h"
#include "analysis/dataflow.h"
#include "nss.h"

class IPCallGraphNode
{
	
	Function* func;
	map<address_t,IPCallGraphNode*> parent;
	map<address_t, set<IPCallGraphNode*>> direct_children; //Functions which are directly called along with the instruction address where it is called
	map<address_t, set<IPCallGraphNode*>> indirect_children; //Functions which are indirectly called along with the instruction address where it is called
	set<address_t> indirectCalls; //Instruction with an indirect call
	map<address_t, set<IPCallGraphNode*>> ATFunctions;	//Functions which are address taken(AT) along with the address of the instruction where it is AT
	int color;
	//set<IPCallGraphNode*> allATFunctions;
	map<address_t,int> nargs_icall; //From TypeArmor
	map<address_t, bool> icallResolved;
	map<tuple<address_t, IPCallGraphNode*>, bool> parentType;

	public :

		IPCallGraphNode(Function* f)
		{
			func = f;
		}
		void insertCallTarget(address_t iaddr, bool isDirect, IPCallGraphNode* t);
		
		void insertCallTargetSet(address_t iaddr, bool isDirect, set<IPCallGraphNode*> t);

		set<address_t> getIcallSite()
		{
			return indirectCalls;
		}
		bool isIcallResolved(address_t addr)
		{
			return icallResolved[addr];
		}
		void setIcallResolved(address_t addr, bool value)
		{
			icallResolved[addr] = value;
		}
		void addIndirectSource(address_t addr)
		{
			indirectCalls.insert(addr);
		}
		void addATFunction(address_t addr, IPCallGraphNode* t)
		{
			auto at_iter = ATFunctions.find(addr);
			if(at_iter != ATFunctions.end())
			{
				auto at_set = at_iter->second;
				at_set.insert(t);
				at_iter->second = at_set;
			}
			else
			{
				set<IPCallGraphNode*> at_set;
				at_set.insert(t);
				ATFunctions[addr] = at_set;
			}
		}
		void setFunction(Function* f);
		
		Function* getFunction();
		
		void insertParent(address_t,IPCallGraphNode* p, bool type);
		
		void removeParent(IPCallGraphNode* p);

		void removeParent(address_t addr, IPCallGraphNode* p);


		void removeAllCallTargets(IPCallGraphNode* t);
		
		void removeCallTarget(address_t addr, IPCallGraphNode* t);

		bool hasIndirectCall() 
		{ 
			if(indirectCalls.size() > 0)
				return true;
			else
				return false;

		 }
		
		set<IPCallGraphNode*> getParent()
		{
			set<IPCallGraphNode*> s;
			for(auto i : parent)
			{
				s.insert(i.second);
			}
			return s;
		}

		map<tuple<address_t, IPCallGraphNode*>, bool> getParentWithType()
		{
			return parentType;
		}

		map<address_t,IPCallGraphNode*> getParentCallSites()
		{
			return parent;
		}
		set<IPCallGraphNode*> getAllCallTargets(); //Get all functions called

		set<IPCallGraphNode*> getDirectCallTargets(); //Get all functions which are directly called

		set<address_t> getIndirectCallSites()  //Get all instructions which has an indirect call
		{
			return indirectCalls;
		}
		
		map<address_t, set<IPCallGraphNode*>> getATList(); //Get all functions which are AT

		map<address_t, set<IPCallGraphNode*>> getDirectChildren() {
			return direct_children;
		}

		map<address_t, set<IPCallGraphNode*>> getIndirectChildren(){
			return indirect_children;
		}

		void updateIndirectChildren(address_t addr, set<IPCallGraphNode*> s)
		{
			indirect_children[addr] = s;
		}
		/*
		set<IPCallGraphNode*> getallATFunctions()
		{
			return allATFunctions;
		}
		*/
};

class IPCallGraph
{	
	//TypeArmor variables
	map<Function*, int> functionNargs;
	set<Function*> nonvoidFn;
	map<tuple<address_t,Module*>, int> icallNargs;
	set<tuple<address_t,Module*>> nonvoidIcall;
	bool icanalysisFlag = false;
	bool typeArmorFlag = false;
	string typeArmorPath;
	Program* program = NULL;
	Function* startfunc = NULL;
	vector<Function*> finiFuncs;
	vector<Function*> initFuncs;

	NSSFuncsPass nss;
	int nicalls=0;
	int resolvedIcalls=0;
	int ndirectedges=0;

	vector<Module*> modulesWithSymbols;
	map<Module*, vector<DataSection*>> dsInModule;
	map<Module*, map<DataSection*, bool>> isModuleVisited;
	map<Module*, map<DataSection*, set<Function*>>> ATInData;
	void addIndirectSource(address_t addr, Function* func);
	void parseData(Module* module, Instruction* instr, Function* f, DataSection *ds, address_t targetAddress, set<address_t> *visited);
	void parseDataWithNoSymbols(Module* module, address_t instrAddr, Function* f, DataSection *ds);
	void findData();
	void findDirectEdges(Function* f);
	void findATList(Function* f);
	bool searchDownDef(UDState* state, int reg1, Function* atfunc);
	bool handleArgumentFnPtr(int reg, Function* f, Instruction* instr, Function* atfunc);
	void checkForSymbols();
	void generateIndirectEdges(IPCallGraphNode* n);
	void generateIndirectEdgesWithTypeArmor(IPCallGraphNode* n, address_t addr, set<IPCallGraphNode*> at);
	void parseTypeArmor();
	void printNodeInfo();
	vector<Function*> functionRoots;
	vector<address_t> dataRoots;
	vector<Function*> nssFunctions;
	vector<string> nssFuncNames;
	std::set<Function *> visited_direct;
	std::set<Function *> visited_AT;
	std::set<tuple<Instruction*,int>> visited_states;
	std::set<Function *> globalATList;
	std::set<Function *> indirectSources;
	set<tuple<Module*,GlobalVariable*>> globalVariables;
	set<tuple<Module*,DataVariable*>> dataVariables;
	DataFlow df;
	int totResolvedIcTarget=0;
	int totTypeArmorTarget=0;
	public:
	map<Function*, IPCallGraphNode*> nodeMap;
	void addFunctionRoot(Function* func);
	void addATFunction(address_t addr, Function* from, Function* ATfunc);

	IPCallGraph(Program* program)
	{
		this->program = program;

	}

	IPCallGraph()
	{

	}
	
	IPCallGraphNode* getNode(Function* f)
	{
		auto f_iter = nodeMap.find(f);
		if(f_iter == nodeMap.end())
		{
			auto tofind = f->getName();
			auto tofind_mod = (Module*)f->getParent()->getParent();
			for(auto n : nodeMap)
			{
				auto cur_f = n.first;
				auto cur_mod = (Module*)cur_f->getParent()->getParent();
				if(cur_f->getName() == tofind && cur_mod == tofind_mod)
					return n.second; 
			}
			return NULL;
		}
		return f_iter->second;
	}
	void setRoot(Function* func);
	vector<Function*> getFiniFuncs();
	vector<Function*> getInitFuncs();

	void setProgram(Program* program)
	{
		this->program = program;
	}
	void setIcanalysis(bool flag) //icanalysisFlag = true, if you want to include indirect call target analysis
	{
		this->icanalysisFlag = flag;
	}
	void setTypeArmor(bool flag) //typeArmorFlag determines if you want to add typearmor analysis to filter indirect call targets
	{
		this->typeArmorFlag = true;
	}
	void setTypeArmorPath(string path)
	{
		this->typeArmorPath = path;
	}
	void addEdge(address_t addr, Function* start, Function* end, bool isDirect);
	void removeAllEdges(Function* start, Function* end);
	void removeEdge(address_t addr, Function *start, Function* end);
	Function* getFunction(address_t addr, Module* mod);	//For TypeArmor
	void generate();		
	void generateDirectCallGraph();
	void printCallGraph();
	void printCallGraphofApplication();

	void printIndirectEdges();
	void printDirectEdges();
	bool forwardDataFlow(Function* f, Instruction* instr, Function* atfunc);

	set<Function*> getFunctionByAddress(address_t addr);
	Function* getNSSFunctionByName(string name);
	std::set<Function *> getGlobalATList()
	{
		return globalATList;
	}
	void addtoGlobalATList(Function* func)
	{
		globalATList.insert(func);
	}
	void pruneIndirectEdges(IPCallGraphNode* node); //Function to prune indirect edges according to temporal specialization
	void resolveNss(ConductorSetup* setup);
	void addNssFunc(Function* func)
	{
		nssFunctions.push_back(func);
	}

	void addNssFuncName(string fname)
	{
		nssFuncNames.push_back(fname);
	}
	void addNssEdges();
};

#endif


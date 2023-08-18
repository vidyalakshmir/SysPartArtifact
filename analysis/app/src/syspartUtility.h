#ifndef SYSPARTUTILITY
#define SYSPARTUTILITY

#include<vector>

#include "ipcallgraph.h"
#include "analysis/usedef.h"
#include "analysis/usedefutil.h"
#include "analysis/dataflow.h"

struct UDResult
{
	int type;	//0 for unknown, 1 for value, 2 for value stored in address, 3 for complex
	address_t addr;
	string desc;
	Function* func;
	bool operator==(const UDResult& l)
  	{
    	 if((l.type == this->type) && (l.addr == this->addr) && (l.desc == this->desc) && ((l.func)->getName() == (this->func)->getName()))
    	 	return true;
    	 return false;
  	}
};

struct Inter_result
{
	string fname;
	address_t addr;
	int reg;
	vector<UDResult> res;
	bool operator==(const Inter_result& l)
	{
		if((l.fname == this->fname) &&(l.addr == this->addr) && (l.reg == this->reg))
			return true;
		return false;
	}
};

struct Intra_result
{	string fname;
	address_t faddr;
	address_t iaddr;
	int reg;
	vector<UDResult> res;
	bool operator==(const Intra_result& l)
	{
		if((l.fname == this->fname) &&(l.faddr == this->faddr) && (l.iaddr == this->iaddr) && (l.reg == this->reg))
			return true;
		return false;
	}
};

struct FPath
{
	string fname;
	address_t iaddr;
	vector<Function*> path;
	bool operator==(const FPath& l)
	{
		if((l.fname == this->fname) &&(l.iaddr == this->iaddr))
			return true;
		return false;
	}
};

class SyspartUtility
{
	public :
		
		SyspartUtility(Program *program, IPCallGraph *ipc, int analysisType)
		{
			this->program = program;
			this->ip_callgraph = ipc;
			this->iter = 0;
			this->analysisType = analysisType;

		}
		void initialize();
		void getArgumentsPassedToFunction(Function* func, int reg, vector<UDResult> &result);
		void findRegDef(Function* func, UDState *state, int reg, vector<UDResult> &final_result);
		bool findIndirectCallTargets(IPCallGraphNode* n);
		string getFunctionName(address_t addr);
		void printResult(UDResult res);
		vector<Function*> getFunctionByAddress(address_t addr, Module* mod);
		vector<Function*> new_resolvedFns;
		vector<Function*> prev_resolvedFns;
		int iter=0;
	private :
		int analysisType; //0 when passed to find values of argument register
		vector<Inter_result> found_results;
		vector<Intra_result> found_state_results;
		vector<tuple<string,address_t,int>> visitedFuncRegs;
		Program *program=NULL;
		IPCallGraph *ip_callgraph=NULL;
		vector<tuple<Instruction*, int>> visitedStates;
		vector<FPath>icPath;
		Function* cur_function;
		Instruction* cur_instr;
		int arg_count=0;
		int reg_count=0;
		int stack_depth=0;
		DataFlow df;

};
#endif
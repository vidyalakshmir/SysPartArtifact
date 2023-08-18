#ifndef EGALITO_LOOP_ANALYSIS
#define EGALITO_LOOP_ANALYSIS

#include<tuple>
#include<set>
using namespace std;

#include "chunk/function.h"
#include "analysis/controlflow.h"
#include "chunk/block.h"

class LoopAnalysis
{
	public :
	std::vector<ControlFlowNode> graph;
	struct Loop
	{
		Function* func;
		Module* module;
		address_t entry_addr;
		Block* entry;
		set<Block*> tail; //backedge happens from tail->entry; can have multiple backedges to the same entry
		set<Block*> exitBlocks;
		set<Block*> loopBody;
	};

	map<pair<address_t,Module*>,Loop*> loops;

	LoopAnalysis() {}
	void detectLoops(Function *func);
	int getLoopCount(Function *func);

	int computeNumBlocks(id_t startID, ControlFlowNode* endNode, int &reached, int &nblocks);
	int getFnCalls(Block *b);
	void naturalLoops(ControlFlowNode* headerNode, ControlFlowNode* exitNode, set<Block*> *result);
	void printLoops();
	void printLoopBlocks();
	map<pair<address_t,Module*>,Loop*> getLoops();
	Loop* getLoop(address_t addr,Module* module);
	void printLoop(Loop* l);
	set<Block*> findNonLoopParent(Function* func, Loop *l);
};

#endif

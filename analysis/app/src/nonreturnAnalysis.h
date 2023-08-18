#include <set>
#include <vector>
#include <map>

#include "analysis/controlflow.h"
#include "chunk/concrete.h"
#include "instr/linked-x86_64.h"


class NonReturnAnalysis 
{
private:
    const static std::vector<std::string> knownList;
    std::set<Function *> nonReturnList;
    std::map<Block*,int> noreturn_done;  //To keep track of blocks whose nonreturn are evaluated

public:
    NonReturnAnalysis() {}
    int isNonReturn(ControlFlowGraph *cfg, Block* bl, std::set<Block*> visited);
    void run(Program* program);
    std::set<Function *> getNonReturnList() { return nonReturnList; }
private:
    bool hasLinkToNeverReturn(ControlFlowInstruction *cfi);
    bool inList(Function *function);
};
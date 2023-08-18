#include<iostream>
#include<string>
#include <bits/stdc++.h> 
#include<malloc.h>
using namespace std;

#include "analysis/walker.h"
#include "instr/concrete.h"
#include "chunk/dump.h"
#include "disasm/dump.h"
#include "disasm/handle.h"
#include "syspartUtility.h"
#include "chunk/ictarget.h"
#include "elf/elfspace.h"

#include<climits> //For INT_MIN

#undef DEBUG_GROUP
#define DEBUG_GROUP syspartutil
#define D_syspartutil 20

#include "log/log.h"

void SyspartUtility::getArgumentsPassedToFunction(Function* func, int reg, vector<UDResult> &result)
{
    auto gl = ip_callgraph->getGlobalATList();
    if(gl.count(func) != 0)
    {
        UDResult res {0, 0, "unknown", func};
        if(find(result.begin(), result.end(), res) == result.end())
        {
            result.push_back(res);
        }
        return;
    }
    tuple<string,address_t,int> visited_tup{func->getName(), func->getAddress(), reg};
    if(find(visitedFuncRegs.begin(), visitedFuncRegs.end(), visited_tup) != visitedFuncRegs.end())
    {
        
        UDResult res {0, 0, "unknown", func};
        if(find(result.begin(), result.end(), res) == result.end())
        {
            result.push_back(res);
        }
        return;
    }
    vector<UDResult> ir_result;
    Inter_result ir = {func->getName(), func->getAddress(), reg, ir_result};
    auto found_it = find(found_results.begin(), found_results.end(), ir);
    if(found_it != found_results.end())
    {
        auto found_vec = (*found_it).res;
        result.insert(result.end(), found_vec.begin(), found_vec.end());
        return;

    }

    vector<UDResult> temp_res;

    arg_count++;
    tuple<string, address_t, int> tup1(func->getName(), func->getAddress(), reg);
    visitedFuncRegs.push_back(tup1);                   //Add (func, reg) to the visited list


    auto cur_node = ip_callgraph->getNode(func);
    if(cur_node == NULL)
        return;
    auto parentType = cur_node->getParentWithType();
    bool isCalled = false;
    vector<UDResult> alreadyPrinted; //for dlsym & dlopen

    for(auto pt : parentType)
    {
        address_t calling_addr;
        IPCallGraphNode* parent;
        tie(calling_addr, parent) = pt.first;
        bool type = pt.second;
        if(type == false)
        {
            if(!parent->isIcallResolved(calling_addr))
                continue;
        }
        //Only functions which directly invoke and indirect calls which are fully resolved considered
        auto calling_fn = parent->getFunction();
        
        auto working = df.getWorkingSet(calling_fn);
        vector<Function*> fp_vec;
        if(analysisType != 0)
        {
            FPath fp{cur_function->getName(),cur_instr->getAddress(),fp_vec};
            auto icPath_iter = find(icPath.begin(), icPath.end(), fp);
            if(icPath_iter != icPath.end())
            {
                auto path_vec = (*icPath_iter).path;
                if(find(path_vec.begin(), path_vec.end(), calling_fn) == path_vec.end())
                {
                    path_vec.push_back(calling_fn);
                    (*icPath_iter).path = path_vec;
                }
            }
            else
            {
                fp_vec.push_back(calling_fn);
                icPath.push_back(fp);
            }
        }
        for(auto block : CIter::children(calling_fn))
        {
            for(auto instr : CIter::children(block))
            {
                if(instr->getAddress() == calling_addr)
                {
                    isCalled = true;
                    auto state = working->getState(instr);
                   
                    if(analysisType == 0)
                        cout<<"FUNC "<<func->getName()<<" CALLINGFN "<<calling_fn->getName()<<" CALLINGADDR "<<std::hex<<calling_addr<<endl;
                    state->dumpState();
                    bool refFlag = false;
                    for(auto& s : state->getRegRef(reg))
                    {
                        refFlag = true;
                        findRegDef(calling_fn, s, reg, temp_res);
                    }
                    if(!refFlag)
                    {
                        if(reg == 7 || reg == 6 || reg == 2 || reg == 1 || reg == 8 || reg == 9)    //Argument registers?
                        {
                            getArgumentsPassedToFunction(calling_fn, reg, temp_res);
                        }
                        else
                        {
                            UDResult res {0, 0, "unknown", func};
                            if(find(temp_res.begin(), temp_res.end(), res) == temp_res.end())
                            {
                                temp_res.push_back(res);
                            }
                        }
                    }
                    if(analysisType == 0)
                    {
                        for(auto tr : temp_res)
                        {
                            if(find(alreadyPrinted.begin(), alreadyPrinted.end(), tr) == alreadyPrinted.end())
                            {
                                alreadyPrinted.push_back(tr);
                                auto mod = (Module*)(tr.func)->getParent()->getParent();
                                cout<<"TYPE "<<tr.type<<"\tADDRESS "<<std::hex<<tr.addr<<" DESC "<<tr.desc<<" FUNC "<<(tr.func)->getName()<<" MODULE " <<mod->getName();   
                                if((tr.type == 0) | (tr.type == 3))
                                {
                                    cout<<" UNKNOWN"<<endl;
                                    continue;
                                }
                                if(tr.desc == "0x0")
                                {
                                    cout<<" SYM NULL"<<endl;
                                    continue;
                                }
                                ElfMap *elf = mod->getElfSpace()->getElfMap();
                                auto section = elf->findSection(".rodata");
                                auto rodata = elf->getSectionReadPtr<char *>(".rodata");
                                auto offset = section->convertVAToOffset(tr.addr);
                                char* value = rodata + offset;
                                cout<<" SYM "<<value<<endl;
                            }
                        }
                        cout<<"END"<<endl;
                    }
                }
            }
        }
    }
    if(!isCalled)
    {
        UDResult res {0, 0, "unknown", func};
        if(find(temp_res.begin(), temp_res.end(), res) == temp_res.end())
        {
            temp_res.push_back(res);
        }
    }
    result.insert(result.end(), temp_res.begin(), temp_res.end());

    auto it1 = find(visitedFuncRegs.begin(), visitedFuncRegs.end(),tup1);
    // Check if Iterator is valid
    if(it1 != visitedFuncRegs.end())
    {
                
        visitedFuncRegs.erase(it1);                            

    }
    arg_count--;
    Inter_result ir1{func->getName(), func->getAddress(), reg, temp_res};
    found_results.push_back(ir1);
}

void SyspartUtility::findRegDef(Function* func, UDState *state, int reg, vector<UDResult> &final_result)
{
    stack_depth++;
    //if(stack_depth>100)
        //return;
    //cout<<"STACK "<<std::dec<<stack_depth<<" "<<cur_function->getName()<<endl;
    //R0:  (deref (+ %R5 -12)) nodeType=1
    typedef TreePatternUnary<TreeNodeDereference,
                TreePatternCapture<TreePatternBinary<TreeNodeAddition,
                    TreePatternTerminal<TreeNodePhysicalRegister>,
                    TreePatternTerminal<TreeNodeConstant>>>>MemoryAddress1;
    
    //R0:  (deref (- %R5 -12)) nodeType=2
    typedef TreePatternUnary<TreeNodeDereference,
                TreePatternCapture<TreePatternBinary<TreeNodeSubtraction,
                    TreePatternTerminal<TreeNodePhysicalRegister>,
                    TreePatternTerminal<TreeNodeConstant>>>>MemoryAddress2;  

    //R2:  (+ %R5 20) nodeType=3
    typedef
            TreePatternBinary<TreeNodeAddition,
                TreePatternCapture<TreePatternTerminal<TreeNodePhysicalRegister>>,
                TreePatternCapture<TreePatternTerminal<TreeNodeConstant>>> MemoryAddress3;
    //R4:  (- %R4 48) nodeType=4
    typedef
            TreePatternBinary<TreeNodeSubtraction,
                TreePatternCapture<TreePatternTerminal<TreeNodePhysicalRegister>>,
                TreePatternCapture<TreePatternTerminal<TreeNodeConstant>>> MemoryAddress4;
    
    //R0:  (deref %R5) nodeType=9
            typedef TreePatternUnary<TreeNodeDereference, 
                                    TreePatternCapture<TreePatternTerminal<TreeNodePhysicalRegister>>> MemoryAddress5;
    //R0:  0 nodeType=5
            typedef TreePatternCapture<TreePatternTerminal<TreeNodeConstant>>ConstantValue;

    //R6:  %R0 nodeType=6
            typedef TreePatternCapture<TreePatternTerminal<TreeNodePhysicalRegister>>RegisterValue;

    //R0:  (+ %rip=0x749 -101) nodeType=7
            typedef TreePatternBinary<TreeNodeAddition,
                        TreePatternCapture<TreePatternRegisterIs<X86_REG_RIP>>,
                        TreePatternCapture<TreePatternTerminal<TreeNodeConstant>>
                    > RIPValue;

    //R0:  (deref (+ %rip=0x749 -101)) nodeType=8
            typedef TreePatternUnary<TreeNodeDereference,
                    TreePatternCapture<TreePatternBinary<TreeNodeAddition,
                            TreePatternCapture<TreePatternRegisterIs<X86_REG_RIP>>,
                            TreePatternCapture<TreePatternTerminal<TreeNodeConstant>>
                    >>> RIPDerefValue;

    

    //rsi:  (+ %rsi %rax) nodeType=10
            typedef
            TreePatternBinary<TreeNodeAddition,
                TreePatternCapture<TreePatternTerminal<TreeNodePhysicalRegister>>,
                TreePatternCapture<TreePatternTerminal<TreeNodePhysicalRegister>>> RegisterSum;

    //rsi:  (- %rsi %rax) nodeType=11
            typedef
            TreePatternBinary<TreeNodeSubtraction,
                TreePatternCapture<TreePatternTerminal<TreeNodePhysicalRegister>>,
                TreePatternCapture<TreePatternTerminal<TreeNodePhysicalRegister>>> RegisterDifference;


        auto instr = state->getInstruction();
        state->dumpState();
        tuple<Instruction*, int> visited_tup{instr, reg};
        if(find(visitedStates.begin(),visitedStates.end(),visited_tup) != visitedStates.end())
        {
            UDResult res {0, 0, "unknown", func};
            if(find(final_result.begin(), final_result.end(), res) == final_result.end())
            {
                final_result.push_back(res);
            }
            return;
        }
        vector<UDResult> ir_res;
        Intra_result ir{func->getName(), func->getAddress(), instr->getAddress(), reg, ir_res};
        auto found_it = find(found_state_results.begin(), found_state_results.end(), ir);
        if(found_it != found_state_results.end())
        {
            auto found_vec = (*found_it).res;
            final_result.insert(final_result.end(), found_vec.begin(), found_vec.end());
            return;
        }
    
        vector<UDResult> result;
        reg_count++;
        tuple<Instruction*, int> tup1(state->getInstruction(), reg);
        visitedStates.push_back(tup1);
        
        bool stop = true;
    set<tuple<UDState*, int>> vs;
    stack<tuple<UDState*,int>> pending;
    tuple<UDState*,int> pending_tup(state,reg);
    pending.push(pending_tup);
    do
    {
        auto st_top = pending.top();
        UDState* state;
        int reg;
        tie(state, reg) = st_top;
        pending.pop();
        if(vs.find({state, reg}) != vs.end())
        {
            break;
        }
        state->dumpState();
        tuple<UDState*, int> vstup(state, reg);
        vs.insert(vstup);
        stop = true;
        bool defFlag = false;
    
        if(auto def = state->getRegDef(reg)) 
        {
            defFlag = true;
            TreeCapture cap;
            if( MemoryAddress1::matches(def, cap) || MemoryAddress2::matches(def, cap) || MemoryAddress5::matches(def, cap)) 
            {
                MemLocation loadLoc(cap.get(0));
                bool memFlag = false;
                for(auto& ms : state->getMemRef(reg))
                {
                        bool flag = false;
                        memFlag = true;
                        ms->dumpState();
                        for(auto& mem : ms->getMemDefList())
                        {
                            flag = true;
                            MemLocation storeLoc(mem.second);
                            if(loadLoc == storeLoc)
                            {
                                bool refFlag = false;
                                for(auto& mss : ms->getRegRef(mem.first))
                                {
                                    refFlag = true;
                                    stop = false;
                                    tuple<UDState*,int> pending_tup(mss,mem.first);
                                    pending.push(pending_tup);
                                
                                }
                                if(!refFlag)
                                {
                                    if(mem.first == 7 || mem.first == 6 || mem.first == 2 || mem.first == 1 || mem.first == 8 || mem.first == 9)    //Argument registers?
                                    {
                                        getArgumentsPassedToFunction(func, mem.first, result); 
                                    }
                                    else
                                    {
                                        UDResult res {0, 0, "unknown", func};
                                        if(find(result.begin(), result.end(), res) == result.end())
                                        {
                                            result.push_back(res);
                                        }
                                    }
                                }
                            }
                        }
                        if(!flag)
                        {
                            UDResult res {0, 0, "unknown", func};
                            if(find(result.begin(), result.end(), res) == result.end())
                            {
                                result.push_back(res);
                            }
                        }
                }
                if(!memFlag)
                    {
                        auto node1 = loadLoc.getRegTree();
                        auto offset = loadLoc.getOffset();
                        bool refFlag = false;
                        if(auto regnode = dynamic_cast<TreeNodePhysicalRegister *>(node1))
                        {                            
                            auto reg2 = regnode->getRegister();
                            vector<UDResult> inter_res;
                            for(auto& rs : state->getRegRef(reg2))
                            {
                                refFlag = true;
                                findRegDef(func, rs, reg2, inter_res);
                            }
                            if(refFlag)
                            {
                                for(auto res1 : inter_res)
                                {
                                    if(res1.type == 0)
                                    {
                                        result.push_back(res1);
                                        continue;
                                    }
                                    if(offset == 0 && res1.type == 1)
                                    {
                                        std::stringstream sstream;
                                        sstream << "val(0x" << std::hex << res1.addr<<")";
                                        std::string new_str = sstream.str();
                                        UDResult res{3, 0, new_str, func};
                                        result.push_back(res);
                                        continue;
                                    }
                                    std::stringstream sstream;
                                    sstream << "val(" << res1.desc << " + 0x" << std::hex << offset <<")";
                                    std::string new_str = sstream.str();
                                    UDResult res{3, 0, new_str, func};
                                    result.push_back(res);
    
                                }
                            }
                        }
                        if(!refFlag)
                        {
                            UDResult res {0, 0, "unknown", func};
                            if(find(result.begin(), result.end(), res) == result.end())
                            {
                                result.push_back(res);
                            }
                        }
                    }
            }
            else if(MemoryAddress3::matches(def, cap))
            {
                auto reg1 = dynamic_cast<TreeNodePhysicalRegister *>(cap.get(0))->getRegister();
                auto const_val = dynamic_cast<TreeNodeConstant *>(cap.get(1))->getValue();
                vector<UDResult> inter_result;
                bool refFlag = false;
                bool unknown = true;
                for(auto & rf : state->getRegRef(reg1))
                {
                    refFlag = true;
                    findRegDef(func, rf, reg1, inter_result);
                    unknown = false;
                }
                if(!refFlag)
                {
                    if(reg1 == 7 || reg1 == 6 || reg1 == 2 || reg1 == 1 || reg1 == 8 || reg1 == 9)    //Argument registers?
                    {
                        getArgumentsPassedToFunction(func, reg1, inter_result);
                        unknown = false;
                    }
                    else
                    {
                        UDResult res {0, 0, "unknown", func};
                        if(find(result.begin(), result.end(), res) == result.end())
                        {
                            result.push_back(res);
                        }
                    }
                }
                if(!unknown)
                {
                    for(auto udres : inter_result)
                    {
                        if(udres.type == 0)
                        {
                            UDResult res {0, 0, "unknown", func};
                            if(find(result.begin(), result.end(), res) == result.end())
                            {
                                result.push_back(res);
                            }
    
                        }
                        else if(udres.type == 1)
                        {
                            auto new_val = const_val + udres.addr;
                            std::stringstream sstream;
                            sstream << "0x" << std::hex << new_val;
                            std::string new_str = sstream.str();
    
                            UDResult res {1, new_val, new_str, func};
                            result.push_back(res);
                        }
                        else if(udres.type == 2 || udres.type == 3)
                        {
                            std::stringstream sstream;
                            sstream << "0x" << std::hex << const_val << " + " << udres.desc;
                            std::string new_str = sstream.str();
                            UDResult res {3, 0, new_str, func}; 
                            result.push_back(res);
                        }
                    }
                }
    
            }
            else if(MemoryAddress4::matches(def, cap))
            {
                auto reg1 = dynamic_cast<TreeNodePhysicalRegister *>(cap.get(0))->getRegister();
                auto const_val = dynamic_cast<TreeNodeConstant *>(cap.get(1))->getValue();
                vector<UDResult> inter_result;
                bool refFlag = false;
                bool unknown = true;
                for(auto & rf : state->getRegRef(reg1))
                {
                    refFlag = true;
                    findRegDef(func, rf, reg1, inter_result);
                    unknown = false;
                }
                if(!refFlag)
                {
                    if(reg1 == 7 || reg1 == 6 || reg1 == 2 || reg1 == 1 || reg1 == 8 || reg1 == 9)    //Argument registers?
                    {
                        getArgumentsPassedToFunction(func, reg1, inter_result);
                        unknown = false;
                    }
                    else
                    {
                        UDResult res {0, 0, "unknown", func};
                        if(find(result.begin(), result.end(), res) == result.end())
                        {
                            result.push_back(res);
                        }
                    }
                }
                if(!unknown)
                {
                    for(auto udres : inter_result)
                    {
                        if(udres.type == 0)
                        {
                            UDResult res {0, 0, "unknown", func};
                            if(find(result.begin(), result.end(), res) == result.end())
                            {                    
                                result.push_back(res);
                            }
                        }
                        else if(udres.type == 1)
                        {
                            auto new_val = udres.addr - const_val;
                            std::stringstream sstream;
                            sstream << "0x" << std::hex << new_val;
                            std::string new_str = sstream.str();
                            UDResult res {1, new_val, new_str, func};                    
                            result.push_back(res);
                            //cout<<"DESC "<<new_str<<" "<<func->getName()<<" "<<reg1<<" "<<std::hex<<instr->getAddress()<<endl;
                        }
                        else if(udres.type == 2 || udres.type == 3)
                        {
                            std::stringstream sstream;
                            sstream << udres.desc << " - " << "0x" << std::hex << const_val;
                            std::string new_str = sstream.str();
                            //auto new_desc = udres.desc + "-" + to_string(const_val) ;
                            UDResult res {3, 0, new_str, func};                    
                            result.push_back(res);
                            //cout<<"DESC "<<new_str<<" "<<func->getName()<<" "<<reg1<<" "<<std::hex<<instr->getAddress()<<endl;
                        }
                    }
                }
            }
            else if(ConstantValue::matches(def, cap)) 
            {
                auto const_val = dynamic_cast<TreeNodeConstant *>(cap.get(0))->getValue();
                std::stringstream sstream;
                sstream << "0x" << std::hex << const_val;
                std::string new_str = sstream.str();
                UDResult res {1, const_val, new_str, func};
                result.push_back(res);
                //cout<<"DESC "<<new_str<<" "<<func->getName()<<" "<<reg<<" "<<std::hex<<instr->getAddress()<<endl;
            }
            else if(RegisterValue::matches(def, cap)) 
            {
                auto reg1 = dynamic_cast<TreeNodePhysicalRegister *>(cap.get(0))->getRegister();
                bool refFlag = false;
                for(auto & rf : state->getRegRef(reg1))
                {
                    refFlag = true;
                    stop = false;
                    tuple<UDState*,int> pending_tup(rf,reg1);       
                    pending.push(pending_tup);

                }
                if(!refFlag)
                {
                    if(reg1 == 7 || reg1 == 6 || reg1 == 2 || reg1 == 1 || reg1 == 8 || reg1 == 9)    //Argument registers?
                    {
                        getArgumentsPassedToFunction(func, reg1, result);
                    }
                    else
                    {
                        UDResult res {0, 0, "unknown", func};
                        if(find(result.begin(), result.end(), res) == result.end())
                        {
                            result.push_back(res);
                        }
                    }
                }
            }
            else if(RegisterSum::matches(def,cap))
            {
                auto reg1 = dynamic_cast<TreeNodePhysicalRegister *>(cap.get(0))->getRegister();
                auto reg2 = dynamic_cast<TreeNodePhysicalRegister *>(cap.get(1))->getRegister();
                vector<UDResult> inter_res1, inter_res2;
                bool refFlag1 = false;
                for(auto & rf : state->getRegRef(reg1))
                {
                    refFlag1 = true;
                    findRegDef(func, rf, reg1, inter_res1);
                }
                bool refFlag2 = false;
                for(auto & rf : state->getRegRef(reg2))
                {
                    refFlag2 = true;
                    findRegDef(func, rf, reg2, inter_res2);
                }
                if(!refFlag1)
                {
                    if(reg1 == 7 || reg1 == 6 || reg1 == 2 || reg1 == 1 || reg1 == 8 || reg1 == 9)    //Argument registers?
                    {
                        getArgumentsPassedToFunction(func, reg1, inter_res1);
                        refFlag1 = true;
                    }
                }
                if(!refFlag2)
                {
                    if(reg2 == 7 || reg2 == 6 || reg2 == 2 || reg2 == 1 || reg2 == 8 || reg2 == 9)    //Argument registers?
                    {
                        getArgumentsPassedToFunction(func, reg2, inter_res2);
                        refFlag2 = true;
                    }
                }
                if(!refFlag1 && !refFlag2)
                {
                    //cout<<"DFD ? (NO REGREF) FUNC : "<<func->getName()<<std::hex<<" "<<instr->getAddress()<<" "<<std::dec<<reg1<<" "<<reg2<<endl;
                    UDResult res {0, 0, "unknown", func};
                    if(find(result.begin(), result.end(), res) == result.end())
                    {
                        result.push_back(res);
                    }
                    //cout<<"DESC unknown "<<func->getName()<<" "<<reg1<<" " << reg2 << " "<<std::hex<<instr->getAddress()<<endl;
                }
                else
                {
                    for(auto res1 : inter_res1)
                    {
                        for(auto res2 : inter_res2)
                        {
                            if(res1.type == 0 || res2.type == 0)
                            {
                                UDResult res {0, 0, "unknown", func};
                                //cout<<"DESC unknown "<<func->getName()<<" "<<reg1<<" "<<reg2<<" "<<std::hex<<instr->getAddress()<<endl;                    
                                if(find(result.begin(), result.end(), res) == result.end())
                                {
                                    result.push_back(res);
                                }
                            }
                            else if(res1.type == 1 && res2.type == 1)
                            {
                                auto new_val = res1.addr + res2.addr;
                                std::stringstream sstream;
                                sstream << "0x" << std::hex << new_val;
                                std::string new_str = sstream.str();
                                UDResult res {1, new_val, new_str, func};                    
                                result.push_back(res);
                                //cout<<"DESC "<<new_str<<" "<<func->getName()<<" "<<reg1<<" "<<reg2<<" "<<std::hex<<instr->getAddress()<<endl;
                            }
                            else
                            {
                             
                                auto new_str = res1.desc + " + " + res2.desc;
                                UDResult res {3, 0, new_str, func};                    
                                result.push_back(res);
                                //cout<<"DESC "<<new_str<<" "<<func->getName()<<" "<<reg1<<" "<<reg2<<" "<<std::hex<<instr->getAddress()<<endl;
                            }
                        }
                    }
                }
            }
            else if(RegisterDifference::matches(def,cap))
            {
                auto reg1 = dynamic_cast<TreeNodePhysicalRegister *>(cap.get(0))->getRegister();
                auto reg2 = dynamic_cast<TreeNodePhysicalRegister *>(cap.get(1))->getRegister();
                vector<UDResult> inter_res1, inter_res2;
                bool refFlag1 = false;
                for(auto & rf : state->getRegRef(reg1))
                {
                    refFlag1 = true;
                    findRegDef(func, rf, reg1, inter_res1);
                }
                bool refFlag2 = false;
                for(auto & rf : state->getRegRef(reg2))
                {
                    refFlag2 = true;
                    findRegDef(func, rf, reg2, inter_res2);
                }
                if(!refFlag1)
                {
                    if(reg1 == 7 || reg1 == 6 || reg1 == 2 || reg1 == 1 || reg1 == 8 || reg1 == 9)    //Argument registers?
                    {
                        getArgumentsPassedToFunction(func, reg1, inter_res1);
                        refFlag1 = true;
                    }
                }
                if(!refFlag2)
                {
                    if(reg2 == 7 || reg2 == 6 || reg2 == 2 || reg2 == 1 || reg2 == 8 || reg2 == 9)    //Argument registers?
                    {
                        getArgumentsPassedToFunction(func, reg2, inter_res2);
                        refFlag2 = true;
                    }
                }
                if(!refFlag1 && !refFlag2)
                {
                    //cout<<"DFD ? (NO REGREF) FUNC : "<<func->getName()<<std::hex<<" "<<instr->getAddress()<<" "<<std::dec<<reg1<<" "<<reg2<<endl;
                    UDResult res {0, 0, "unknown", func};
                    if(find(result.begin(), result.end(), res) == result.end())
                    {
                        result.push_back(res);
                    }
                    //cout<<"DESC unknown "<<func->getName()<<" "<<reg1<<" "<<reg2<<" "<<std::hex<<instr->getAddress()<<endl;
                }
                else
                {
                    for(auto res1 : inter_res1)
                    {
                        for(auto res2 : inter_res2)
                        {
                            if(res1.type == 0 || res2.type == 0)
                            {
                                UDResult res {0, 0, "unknown", func};
                                if(find(result.begin(), result.end(), res) == result.end())
                                {                    
                                    result.push_back(res);
                                }
                                //cout<<"DESC unknown "<<func->getName()<<" "<<reg1<<" "<<reg2<<" "<<std::hex<<instr->getAddress()<<endl;
                            }
                            else if(res1.type == 1 && res2.type == 1)
                            {
                                auto new_val = res1.addr - res2.addr;
                                std::stringstream sstream;
                                sstream << "0x" << std::hex << new_val;
                                std::string new_str = sstream.str();
                                UDResult res {1, new_val, new_str, func};                    
                                result.push_back(res);
                                //cout<<"DESC "<<new_str<<" "<<func->getName()<<" "<<reg1<<" "<<reg2<<" "<<std::hex<<instr->getAddress()<<endl;
                            }
                            else
                            {
                                auto new_str = res1.desc + " - " + res2.desc;
                                UDResult res {3, 0, new_str, func};                    
                                result.push_back(res);
                                //cout<<"DESC "<<new_str<<" "<<func->getName()<<" "<<reg1<<" "<<reg2<<" "<<std::hex<<instr->getAddress()<<endl;
                            }
                        }
                    }
                }
            }
            else if(RIPValue::matches(def, cap)) 
            {
                auto ripTree = dynamic_cast<TreeNodeRegisterRIP *>(cap.get(0));
                auto dispTree = dynamic_cast<TreeNodeConstant *>(cap.get(1));
                auto new_val = dispTree->getValue() + ripTree->getValue();
                std::stringstream sstream;
                sstream << "0x" << std::hex << new_val;
                std::string new_str = sstream.str();
                UDResult res {1, new_val, new_str, func};
                result.push_back(res);
                //cout<<"DESC "<<new_str<<" "<<func->getName()<<" "<<reg<<" "<<std::hex<<instr->getAddress()<<endl;
    
            }
            else if(RIPDerefValue::matches(def, cap)) 
            {
                TreeCapture cap1;
                if(RIPValue::matches(cap.get(0), cap1)) 
                {
                    auto ripTree = dynamic_cast<TreeNodeRegisterRIP *>(cap1.get(0));
                    auto dispTree = dynamic_cast<TreeNodeConstant *>(cap1.get(1));
                    auto new_val = dispTree->getValue() + ripTree->getValue();
                    std::stringstream sstream;
                    sstream << "val(0x" << std::hex << new_val << ")";
                    std::string new_str = sstream.str();
                    UDResult res {2, new_val, new_str, func};
                    result.push_back(res);
    
                }
            }
        }
    
        if(!defFlag) 
        {   
            //cout<<"DFD ? (NO REGDEF) FUNC : "<<func->getName()<<std::hex<<" "<<instr->getAddress()<<" "<<std::dec<<reg<<endl;
            InstrDumper instrDumper(instr->getAddress(), INT_MIN);
            instr->getSemantic()->accept(&instrDumper);
            //cout<<endl;
            UDResult res {0, 0, "unknown", func};
            if(find(result.begin(), result.end(), res) == result.end())
            {
                result.push_back(res);
            }
            //cout<<"DESC unknown "<<func->getName()<<" "<<reg<<" "<<std::hex<<instr->getAddress()<<endl;
        }

    } while(!pending.empty());
    //vs.clear();
    set<tuple<UDState*, int>>().swap(vs);
        final_result.insert(final_result.end(), result.begin(), result.end());
  
        auto it1 = find(visitedStates.begin(), visitedStates.end(), tup1);
        if(it1 != visitedStates.end())
        {
            visitedStates.erase(it1);
        }
        reg_count--;
        Intra_result ir1{func->getName(), func->getAddress(), instr->getAddress(), reg, result};
        found_state_results.push_back(ir1);
        
    
}

void SyspartUtility::printResult(UDResult res)
{
    cout<<"*************************** TYPE : "<<res.type<<" ADDR : 0x"<<std::hex<<res.addr<<" DESC "<<res.desc<<" FUNC : "<<(res.func)->getName()<<" MODULE : " <<(res.func)->getParent()->getParent()->getName()<<endl;   
}


string SyspartUtility::getFunctionName(address_t addr)
{
    string func_name="";
    for(auto module : CIter::children(program))
    {
        for(auto func : CIter::functions(module))
        {
            if(func->getAddress() == addr)
                func_name = func->getName();
        }
    }
    return func_name;
}

bool SyspartUtility::findIndirectCallTargets(IPCallGraphNode* n)
{
    auto function = n->getFunction();
    bool resolvedFlag = false;

    auto working = df.getWorkingSet(function);
    for(auto block : CIter::children(function)) {
        for(auto instruction : CIter::children(block)) {
            auto semantic = instruction->getSemantic();
            auto state = working->getState(instruction);
            
            if(auto ici = dynamic_cast<IndirectCallInstruction *>(semantic)) {
                //cout<<"Enter"<<endl;
                //int c = getchar();
                vector<IndirectCallTarget> icTargets;
                cur_function = function;
                cur_instr = instruction;
                auto continueFlag = true;
                if(iter > 1)   //Not the first iteration
                {
                    continueFlag = false;
                    vector<Function*> fp_vec;
                    FPath fp{cur_function->getName(),cur_instr->getAddress(), fp_vec};
                    auto icPath_iter = find(icPath.begin(), icPath.end(), fp);
                    if(icPath_iter != icPath.end())
                    {

                        auto path_set = (*icPath_iter).path;
                        for(auto p : path_set)
                        {
                            for(auto r : prev_resolvedFns)
                            {
                                if((r->getName() == p->getName()) && (r->getAddress() == p->getAddress()))
                                {
                                    continueFlag = true;
                                    path_set.clear();
                                    path_set.push_back(cur_function);
                                    (*icPath_iter).path = path_set;
                                    break;
                                }
                            }
                            if(continueFlag)
                                break;
                        }
                    }
                }
                else
                {
                    vector<Function*> fp_vec;
                    fp_vec.push_back(cur_function);
                    FPath fp{cur_function->getName(),cur_instr->getAddress(), fp_vec};
                    icPath.push_back(fp);
                }
                
                if(!continueFlag)
                    continue;
                if(n->isIcallResolved(instruction->getAddress()))
                    continue;
                ici->clearAllTargets();
                InstrDumper instrDumper(instruction->getAddress(), INT_MIN);
                instruction->getSemantic()->accept(&instrDumper);

                stack_depth = 0;
                if(ici->hasMemoryOperand()) 
                {
                    IndirectCallTarget target(instruction->getAddress());
                    target.setUnknown();
                    icTargets.push_back(target);
                    
                }
                else 
                {
                    auto reg = X86Register::convertToPhysical(ici->getRegister());
                    DisasmHandle handle(true);

                    vector<UDResult> results;
                    bool refFlag = false;
                    for(auto& s : state->getRegRef(reg))
                    {
                        refFlag = true;
                        findRegDef(function, s, reg, results);
                    }
                    if(!refFlag)
                    {
                        if(reg == 7 || reg == 6 || reg == 2 || reg == 1 || reg == 8 || reg == 9)    //Argument registers?
                        {
                            getArgumentsPassedToFunction(function, reg, results);
                        }
                        else
                        {
                            UDResult res {0, 0, "unknown", function};
                            if(find(results.begin(), results.end(), res) == results.end())
                            {
                                results.push_back(res);
                            }
                        }
                    }
                  
                        for(auto r : results) 
                        {
                            if(r.type == 0 | r.type == 3)
                            {
                                IndirectCallTarget target(r.addr);
                                target.setUnknown();
                                //ici->addIndirectCallTarget(target);
                                icTargets.push_back(target);
                                continue;
                            }
                            auto mod = (Module*)(r.func)->getParent()->getParent();
                            auto addr_in_egalito = r.addr + mod->getBaseAddress();
                            auto ds = mod->getDataRegionList()->findDataSectionContaining(addr_in_egalito);
                            if(ds != NULL)
                            {
                                auto dsType = ds->getType();
                               
                                
                            }
                            else
                            {
                                //cout<<"UNMATCHED_DATASECTION";
                            }
                            if(r.type == 1)
                            {
                                IndirectCallTarget target(r.addr);
                                auto func_name = getFunctionName(r.addr);
                                if(!func_name.empty())
                                {
                                    target.setName(func_name);
                                }
                                else 
                                {
                                    target.setGlobal();
                                }
                                //ici->addIndirectCallTarget(target);
                                icTargets.push_back(target);
                            }
                            else
                            {
                                IndirectCallTarget target(r.addr);
                                target.setUnknown();
                                //ici->addIndirectCallTarget(target);
                                icTargets.push_back(target);
                            }
                        }
                    
                }
                //Add resolved indirect calls
                //auto icTargets = ici->getIndirectCallTargets();
                bool resolved = true;
                set<address_t> targets;
                if(icTargets.size() == 0)
                    resolved = false;
                for(int i=0; i<icTargets.size(); i++)
                {
                    if(icTargets[i].isGlobal() || icTargets[i].isUnknown())
                    {
                        resolved = resolved & false;
                        break;
                    }
                    else
                    {
                        resolved = resolved & true;
                        targets.insert(icTargets[i].getAddress());
                    }
                }   
                if(resolved)    //only icanalysis finds all targets
                {
                    for(auto t : targets)
                    {                           
                        auto target_set = getFunctionByAddress(t, (Module*)function->getParent()->getParent());
                        for(auto func_target : target_set)
                        {
                            auto node_target = ip_callgraph->getNode(func_target);
                            if(node_target == NULL)
                            {
                                //cout<<"STRANGE NO IPNODE WITH ADDR FOUND "<<std::hex<<t<<endl;
                                continue;
                            }
                            if(find(new_resolvedFns.begin(), new_resolvedFns.end(), func_target) == new_resolvedFns.end())
                            {   
                                new_resolvedFns.push_back(func_target);
                            }
                            n->insertCallTarget(instruction->getAddress(), false, node_target);
                            node_target->insertParent(instruction->getAddress(), n, false);
                        }
                    }
                    n->setIcallResolved(instruction->getAddress(), true);
                    
                    resolvedFlag = true;
                }   
            }

        }
    }
    visitedStates.clear();
    visitedFuncRegs.clear();
    vector<tuple<Instruction*, int>>().swap(visitedStates);
    vector<tuple<string,address_t,int>>().swap(visitedFuncRegs);
    found_results.clear();
    found_state_results.clear();
    
    
    return resolvedFlag;
}

vector<Function*> SyspartUtility::getFunctionByAddress(address_t addr, Module* mod)
{
    vector<Function*> func_set;
    bool found = false;

    for(auto func : CIter::functions(mod))
    {
        if(func->getAddress() == addr)
        {
            found = true;
            bool flag = true;
            for(auto f : func_set)  //To avoid duplicate function objects that refer to the same function
            {
                if((f->getName() == func->getName()) && (f->getParent()->getParent() == mod))
                    flag = false;
            }
            if(flag)                //No duplicates found
                func_set.push_back(func);
        }
    }
    
    if(!found)
    {
        for(auto module : CIter::children(program))
        {
            if(module->getName() == mod->getName())
                continue;
            for(auto func : CIter::functions(module))
            {
                if(func->getAddress() == addr)
                {
                    bool flag = true;
                    for(auto f : func_set)  //To avoid duplicate function objects that refer to the same function
                    {
                        if((f->getName() == func->getName()) && (f->getParent()->getParent() == module))
                            flag = false;
                    }
                    if(flag)                //No duplicates found
                        func_set.push_back(func);
                }
            }
        }
    }
    return func_set;
}

void SyspartUtility::initialize()
{
    for(auto n : ip_callgraph->nodeMap)
    {
        df.addUseDefFor(n.first);
    }

}

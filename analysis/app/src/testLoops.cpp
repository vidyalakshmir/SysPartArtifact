//Main program that prints number of loops in a program
#include<iostream>
#include<string>
#include<fstream>
using namespace std;

#include "conductor/setup.h"
#include "conductor/interface.h"
#include "instr/concrete.h"
#include "log/registry.h"

#include "loopanalysis.h"

int main(int argc, char* argv[])
{
    
    if(argc <=1)
    {
        cout<<"Filename missing! \n\n Syntax : ./loops <filename>\n";
        exit(0);
    }
    char *filename=argv[1];
    Program *program = NULL;
    string fname(filename);
    
    EgalitoInterface egalito( false,  true);
    egalito.initializeParsing();
    egalito.parse(filename,true); //Second argument determines if shared library needs to be parsed
    program = egalito.getProgram();            
    LoopAnalysis loopAnalysis;
    for(auto module : CIter::children(program))
    {
        for(auto func : CIter::functions(module))
        {
            cout<<"FUNC "<<func->getName()<<" "<<std::hex<<func->getAddress()<<endl;
            loopAnalysis.detectLoops(func);
        }
    }
    loopAnalysis.printLoops();
}

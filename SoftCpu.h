#ifndef __SOFTCPU_H
#define __SOFTCPU_H


#include "asm.h"
#include "stack.h"


const size_t regs_count = 5;
const size_t ram_size   = 100;


enum REGS {
    RAX = 1,
    RBX = 2,
    RCX = 3,
    RDX = 4,
};


struct CpuData {

    int *cmd;
    
    stack_t stk;

    int reg[regs_count];
    int ram[ram_size];

    stack_t ret_links;
};


CpuData CpuCtor        (const char *file_name, BufData *sample);
int     ExecuteProgram (CpuData *cpu);
void    CpuDtor (CpuData *cpu);


#endif
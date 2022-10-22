#include <stdio.h>
#include "SoftCpu.h"
#include "asm.h"
#include "stack.h"
#include "strsort.h"
#include "config.h"


CpuData CpuCtor (const char *file_name, BufData *sample) {

    CpuData cpu = {
        .cmd       = ReadAsmBin(file_name, sample),
        .stk       = StackCtor(),
        .reg       = {},
        .ram       = {},
        .ret_links = StackCtor(),
    };

    return cpu;
}

int ExecuteProgram (CpuData *cpu) {

    RET_ON_VAL(!cpu, ERR_NULL_PTR, 1);

    size_t ip = 0;
    while (cpu->cmd[ip] != CMD_HLT) {
        int next = 1, argc = 0;

        #define DEF_CMD(name, num, arg_mod, ...)    \
                case CMD_##name:                    \
                    __VA_ARGS__                     \
                    ip += next + argc;              \
                    break;

        switch (cpu->cmd[ip] & MOD_HIDE) {

            #include "cmd.h"

            default:
                printf("Break at number: %d\n", ip);
                return 1;
                break;
        }
    }

    return 0;  
}

void CpuDtor (CpuData *cpu) {

    FreeAsmCmd(cpu->cmd);
    StackDtor(cpu->stk);
    StackDtor(cpu->ret_links);
}

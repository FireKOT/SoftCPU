#include <stdio.h>
#include "SoftCpu.h"
#include "asm.h"
#include "strsort.h"
#include "stack.h"
#include "config.h"


int main () {

    BufData sample {
        .sign = "UwUCat",
        .ver  = "v3.0",
        .size = 0,
    };

    CpuData cpu = CpuCtor("asm.bin", &sample);

    ExecuteProgram(&cpu);

    CpuDtor(&cpu);

    return 0;
}
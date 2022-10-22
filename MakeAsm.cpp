#include <stdio.h>
#include "asm.h"
#include "stack.h"
#include "strsort.h"
#include "general.h"
#include "config.h"


int main () {

    size_t size = 0;
    int *asmcode = StartCode("code.txt", &size);
    RET_ON_VAL(!asmcode, ERR_NULL_PTR, ERR_NULL_PTR);

    BufData head {
        .sign = "UwUCat",
        .ver  = "v3.0",
        .size = size,
    };
    WriteinFile(asmcode, size, &head, "asm.txt");
    WriteinFileBin(asmcode, size, &head, "asm.bin");

    FreeAsmCode(asmcode);

    return 0;
}


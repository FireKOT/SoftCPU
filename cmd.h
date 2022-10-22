#define STACK      cpu->stk
#define POP        StackPop(STACK)
#define PUSH(val)  StackPush(STACK, (val)) 
#define CMD        cpu->cmd[ip]
#define ARG(n)     cpu->cmd[ip + n]
#define REG(n)     cpu->reg[n]
#define RAM(n)     cpu->ram[n]


DEF_CMD (PUSH, 1, 1, {
    int arg = 0;

    if (CMD & MOD_NUM) arg += ARG(++argc);               
    if (CMD & MOD_REG) arg += REG(ARG(++argc));               
    if (CMD & MOD_MEM) arg  = RAM(arg);

    PUSH(arg);
})

DEF_CMD (POP, 2, 1, {
    RET_ON_VAL((CMD & MOD_NUM) && !(CMD & MOD_MEM), ERR_SEG_ERR, 1)

    if (CMD & MOD_MEM) {
        int arg = 0;

        if (CMD & MOD_NUM) arg += ARG(++argc);
        if (CMD & MOD_REG) arg += REG(ARG(++argc));

        RAM(arg) = POP;
    } 
    else {
        REG(ARG(++argc)) = POP;
    }
})

DEF_CMD (ADD, 3, 0, {
    PUSH(POP + POP);
})

DEF_CMD (SUB, 4, 0, {
    int tmp1 = POP, tmp2 = POP;
    PUSH(tmp2 - tmp1);
})

DEF_CMD (MUL, 5, 0, {
    PUSH(POP * POP);
})

DEF_CMD (DIV, 6, 0, {
    int tmp1 = POP, tmp2 = POP;
    PUSH(tmp2 / tmp1);
})

DEF_CMD (DUP, 7, 0, {
    int tmp1 = POP;
    PUSH(tmp1);
    PUSH(tmp1);
})

DEF_CMD (JMP, 8, 2, {
    argc++;
    ip = ARG(argc);
    next = 0;
    argc = 0;
})

DEF_CMD (JB, 9, 2, {
    int tmp1 = POP, tmp2 = POP;
    argc++;
    if (tmp2 < tmp1) {
        ip = ARG(argc);
        next = 0;
        argc = 0;
    }
})

DEF_CMD (JBE, 10, 2, {
    int tmp1 = POP, tmp2 = POP;
    argc++;
    if (tmp2 <= tmp1) {
        ip = ARG(argc);
        next = 0;
        argc = 0;
    }
})

DEF_CMD (JA, 11, 2, {
    int tmp1 = POP, tmp2 = POP;
    argc++;
    if (tmp2 > tmp1) {
        ip = ARG(argc);
        next = 0;
        argc = 0;
    }
})

DEF_CMD (JAE, 12, 2, {
    int tmp1 = POP, tmp2 = POP;
    argc++;
    if (tmp2 >= tmp1) {
        ip = ARG(argc);
        next = 0;
        argc = 0;
    }
})

DEF_CMD (JE, 13, 2, {
    argc++;
    if (POP == POP) {
        ip = ARG(argc);
        next = 0;
        argc = 0;
    }
})

DEF_CMD (JNE, 14, 2, {
    argc++;
    if (POP != POP) {
        ip = ARG(argc);
        next = 0;
        argc = 0;
    }
})

DEF_CMD (CALL, 15, 2, {
    argc++;
    StackPush(cpu->ret_links, ip+1);
    ip = ARG(argc);
    next = 0;
    argc = 0;
})

DEF_CMD (RET, 16, 0, {
    ip = StackPop(cpu->ret_links);
    next = 0;
})

DEF_CMD (OUT, 17, 0, {
    printf("%d\n", POP);
})

DEF_CMD (HLT, 18, 0, {
    return 0;
})
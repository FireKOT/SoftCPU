#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "asm.h"
#include "strsort.h"
#include "config.h"


const size_t BUF_SIZE    = 32;
const size_t label_count = 100;
const size_t regs_count  = 5;

static const char *registers[regs_count] = {
    "",
    "RAX",
    "RBX",
    "RCX",
    "RDX",
};

static label marks[label_count];


int *StartCode (const char *file_name, size_t *size) {

    ClearMarks();

    int *asmcode = nullptr;

    asmcode = MakeAsmCode(file_name, size);
    RET_ON_VAL(!asmcode, ERR_UNKNOWN_CMD, nullptr);

    asmcode = MakeAsmCode(file_name, size);
    RET_ON_VAL(!asmcode || !IsCorrectMarks(*size), ERR_INCRR_MARKS, nullptr);

    ClearMarks();

    return asmcode;
}

int *MakeAsmCode (const char *file_name, size_t *size) {

    RET_ON_VAL(!file_name, ERR_NULL_PTR, nullptr);

    int err = 0;

    text code = {};
    err = GetText("code.txt", &code);
    RET_ON_VAL(err == -1, ERR_NULL_PTR, nullptr);

    int *asmcode = (int*) calloc(code.lines_count * 3, sizeof(int));

    size_t ip = 0;
    for (size_t i = 0; i < code.lines_count; i++) {
        int pos = 0;
        char strcmd[str_size] = {};

        DelComm(code.content[i].start);

        if (*code.content[i].start == '\0') {
            continue;
        }

        sscanf(code.content[i].start, "%s%n", strcmd, &pos);

        if (IsLabel(strcmd)) {
            SetLabel(strcmd, ip);

            continue;
        }

        /*Processing of commands*/

        #define DEF_CMD(name, num, arg_mod, ...)                                                        \
                if (!stricmp(strcmd, #name)) {                                                          \
                    RET_ON_VAL(ProcArgs(CMD_##name, code.content[i].start + pos, asmcode, &ip, arg_mod),\
                    ERR_INVAL_ARG, nullptr);                                                            \
                }                                                                                       \
                else                                                                                                                               

        #include "cmd.h"
        /*else*/ {
            printf("SyntaxErr\n");
            return nullptr;
        }

        #undef DEF_CMD
    }

    FreeText(&code);

    *size = ip;
    return asmcode;
}

int ProcArgs (int cmd, char *code, int *asmcode, size_t *ip, int arg_mod) {

    switch (arg_mod) {
        case ARG_NO_ARGS:
            return ProcNoArgs (cmd, asmcode, ip);
            break;

        case ARG_NUM:
            return ProcNum(cmd, code, asmcode, ip);
            break;
        
        case ARG_STR:
            return ProcStr(cmd, code, asmcode, ip);
            break;

        default:
            break;
    }

    return 0;
}

int ProcNoArgs (int cmd, int *asmcode, size_t *ip) {

    asmcode[(*ip)++] = cmd;
    return 0;
}

int ProcNum (int cmd, char *code, int *asmcode, size_t *ip) {

    int mod = 0;
    int reg_num = 0;
    int cmd_ip = (*ip)++;

    char reg[str_size] = {};
    int num = 0;
    int isnum = 0, isreg = 0;

    int pos = 0;
    sscanf(code, " [%n", &pos);
    if(pos) {
        int tmp = 0;
        char buf[str_size] = {};
        sscanf(code, " [ %[ +0123456789ABCDRX] ]%n", buf, &tmp);

        RET_ON_VAL(!tmp, ERR_INVAL_ARG, 1);
        mod += MOD_MEM;
    }

    isnum = sscanf(code + pos, " %d", &num);
    if (isnum) {
        isreg = sscanf(code + pos, " %*[ +0123456789] %[^ ]]", reg);
    }
    else {
        isreg = sscanf(code + pos, " %[^ +]", reg);
        isnum = sscanf(code + pos, " %*[^+]+ %d", &num);
    }

    if(isnum) {
        mod += MOD_NUM;
        asmcode[(*ip)++] = num;
    }

    if (isreg) {
        mod += MOD_REG;

        for (size_t i = 1; i < regs_count; i++) {
            if (!stricmp(reg, registers[i])) {
                reg_num = i;
                break;
            }
        }
        RET_ON_VAL(!reg_num, ERR_INVAL_ARG, 1);

        asmcode[(*ip)++] = reg_num;
    }

    asmcode[cmd_ip] = cmd | mod;

    return 0;
}

int ProcStr (int cmd, char *code, int *asmcode, size_t *ip) {
    
    char buf[str_size] = {};
    sscanf(code, "%s", buf);

    asmcode[(*ip)++] = cmd;
    asmcode[(*ip)++] = GetLabel(buf);

    return 0;
}

void DelComm (char *str) {

    char *comm = strchr(str, ';');
    if(comm) {
        *comm = '\0';
    }
}

void ClearMarks () {

    for (size_t i = 0; i < label_count; i++) {
        strcpy(marks[i].name, "");
        marks[i].num = -1;
    }
}

int IsLabel (char *str) {

    char *mark = strchr(str, ':');

    if(mark) {
        *mark = '\0';

        if(*str != '\0') {
            return 1;
        }
    }

    return 0;
}

void SetLabel (char *name, int num) {

    for (size_t i = 0; i < label_count; i++) {
        if (!stricmp(marks[i].name, name)) {
            marks[i].num = num;
            break;
        }
        else if (!stricmp(marks[i].name, "")) {
            strcpy(marks[i].name, name);
            marks[i].num = num;
            break;
        }
    }
}

int GetLabel (char *name) {

    for (size_t i = 0; i < label_count; i++) {
        if (!stricmp(marks[i].name, name)) {
            return marks[i].num;
        }
    }

    return -1;
}

int IsCorrectMarks (size_t size) {

    for (size_t i = 0; i < label_count; i++) {
        if (stricmp(marks[i].name, "")) {
            if (marks[i].num == -1 || marks[i].num < 0 || marks[i].num >= (int) size) {
                return 0;
            }
        }
    }

    return 1;
}

void FreeAsmCode (int *asmcode) {

    free(asmcode);
}

void WriteinFile (int *asmcode, size_t size, BufData *head, const char file_name[]) {

    FILE *output = fopen(file_name, "w");
    RET_ON_VAL(!output, ERR_NULL_PTR, );

    WriteHeader(head, output);
    for (size_t i = 0; i < size; i++) {
        fprintf(output, "%d ", asmcode[i]);
    }

    fclose(output);
}

void WriteHeader (BufData *head, FILE *output) {

    fprintf(output, "%s\n", head->sign);
    fprintf(output, "%s\n", head->ver);
    fprintf(output, "%d\n", head->size);
}

void WriteinFileBin (int *asmcode, size_t size, BufData *head, const char file_name[]) {

    FILE *outputbin = fopen(file_name, "wb");
    RET_ON_VAL(!outputbin, ERR_NULL_PTR, );

    WriteHeaderBin(head, outputbin);
    fwrite(asmcode, 1, size * sizeof(int), outputbin);

    fclose(outputbin);
}

void WriteHeaderBin (BufData *head, FILE *outputbin) {

    fwrite(head, 1, sizeof(BufData), outputbin);
    fseek(outputbin, BUF_SIZE, SEEK_SET);
}

int *ReadAsmBin (const char file_name[], BufData *sample) {

    RET_ON_VAL(!file_name || !sample, ERR_NULL_PTR, nullptr);

    FILE *inputbin = fopen(file_name, "rb");
    RET_ON_VAL(!inputbin, ERR_NULL_PTR, nullptr);

    BufData head = {
        .sign = {},
        .ver  = {},
        .size = 0,
    };
    fread(&head, sizeof(BufData), 1, inputbin);
    RET_ON_VAL(strcmp(head.sign, sample->sign) != 0, ERR_WRONG_SIGN, nullptr);
    RET_ON_VAL(strcmp(head.ver, sample->ver) != 0, ERR_WRONG_VER, nullptr);
    fseek(inputbin, BUF_SIZE, SEEK_SET);


    int *cmd = (int*) calloc(head.size, sizeof(int));
    fread(cmd, sizeof(int), head.size, inputbin);

    fclose(inputbin);

    return cmd;
}

void FreeAsmCmd (int *cmd) {

    free(cmd);
}



#ifndef __ASM_H
#define __ASM_H

#include "strsort.h"

#define DEF_CMD(name, num, ...) CMD_##name = num,

enum CMD {

    #include "cmd.h"
};

#undef DEF_CMD

enum MODS {
    MOD_HIDE = 0x0000FFFF,
    MOD_NUM  = 0x10000000,
    MOD_REG  = 0x01000000,
    MOD_MEM  = 0x00100000,
};

enum ARGS {
    ARG_NO_ARGS = 0,
    ARG_NUM     = 1,
    ARG_STR     = 2,
};


const size_t sign_size = 7;
const size_t ver_size  = 5;
const size_t str_size  = 100;


struct BufData {
    
    char sign[sign_size];
    char ver[ver_size];

    size_t size;
};

struct label {
    char name[str_size];
    int num;
};


int *StartCode      (const char *file_name, size_t *size);
int *MakeAsmCode    (const char *file_name, size_t *size);
void ClearMarks     ();
void FreeAsmCode    (int *asmcode);

int  ProcArgs       (int cmd, char *code, int *asmcode, size_t *ip, int arg_mod);
int ProcNoArgs      (int cmd, int *asmcode, size_t *ip);
int  ProcNum        (int cmd, char *code, int *asmcode, size_t *ip);
int  ProcStr        (int cmd, char *code, int *asmcode, size_t *ip);

void DelComm        (char *str);
int  IsLabel        (char *str);
void SetLabel       (char *name, int num);
int  GetLabel       (char *name);
int  IsCorrectMarks (size_t size);

void WriteinFile    (int *asmcode, size_t size, BufData *head, const char file_name[]);
void WriteHeader    (BufData *head, FILE *output);

void WriteinFileBin (int *asmcode, size_t size, BufData *head, const char file_name[]);
void WriteHeaderBin (BufData *pre, FILE *output);

int *ReadAsmBin     (const char file_name[], BufData *sample);
void FreeAsmCmd     (int *cmd);


#endif
#include <stdio.h>
#include <tgmath.h>
#include <string.h>
#include "general.h"
#include "config.h"


int ArgsProcess (int argc, const char *argv[], int num, const char *possible_args[], int *args[]) {

    for (int i = 1; i < argc; i++) {
        for (int j = 0; j < num; j++) {
            if (strcmp(argv[i], possible_args[j]) == 0) {
                *args[j] = 1;
            }
        }
    }

    return 1;
}

void bitswap (void *param1, void *param2, int size) {

    char *pr1 = (char*) param1;
    char *pr2 = (char*) param2;
    for (int i = 0; i < size; i++) {
        char tmp = pr1[i];
        pr1[i] = pr2[i];
        pr2[i] = tmp; 
    }
}

int ClearStdinBuf () {

    int trash = false;
    int c = 0;
    while ((c = getchar()) != '\n') {
        if (c != ' ' && c != '\t'){
            trash = true;
        }
    }

    return trash;
}

int WantContinue () {

    printf("Do you want to continue [Y/N]?\n");

    int c = getchar();
    int isClear = ClearStdinBuf();
    while ((c != 'Y' && c != 'y' && c != 'N' && c != 'n') || isClear != 0) {
        printf("Incorrect value\n");
        printf("Do you want to continue [Y/N]?\n");

        c = getchar();
        isClear = ClearStdinBuf();
    }

    if (c == 'Y' || c =='y') {
        return 1;
    }

    return 0;
}

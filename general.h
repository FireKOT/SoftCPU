#ifndef _GENERAL_H
#define _GENERAL_H


int ArgsProcess    (int argc, const char *argv[], int num, const char *possible_args[], int *args[]);
void bitswap       (void *param1, void *param2, int size);
int ClearStdinBuf  ();
int WantContinue   ();


#endif // _GENERAL

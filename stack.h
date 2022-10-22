#ifndef __STACK_H
#define __STACK_H


typedef int elem_t;
typedef int stack_t;


enum STACK_ERR {

    ERR_INVAL_STACK_PTR      = 1,
    ERR_INVAL_DATA_PTR       = 2,
    ERR_SIZE_BIGGER_CAPACITY = 3,
    ERR_STACK_CANARY_DEAD    = 4,
    ERR_DATA_CANARY_DEAD     = 5,
    ERR_INVAL_HASH           = 6,
};


int    StackCtor (size_t base_size = 1024);
int    StackPush (stack_t vstk, elem_t value);
elem_t StackPop  (stack_t vstk);
void   StackDtor (stack_t vstk);


#endif
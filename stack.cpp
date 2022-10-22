#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <time.h>
#include <windows.h>
#include "stack.h"
#include "config.h"


static const unsigned long CANARY = 0xDEADBABE;
static const elem_t        POISON = 0xADADADAD;


struct stack_prot {

    unsigned long StartBird = 0;

    elem_t *values = nullptr;

    size_t size = 0;
    size_t capacity = 0;
    
    double factor = 0;

    int isdump = false;

    unsigned long long StackHash = 0;

    unsigned long EndBird;
};


static int  GetCode            ();

static void SetBirds           (void *mas, size_t size);
static int  ResizeUp           (stack_prot *stk);
static int  ResizeDown         (stack_prot *stk);
static void FillbyPoison       (elem_t *start, size_t size);
static char StackOK            (stack_prot *stk);
static void SetErr             (char *err, size_t num);
static void StackDump          (stack_prot *stk, char err);
static unsigned long long pow  (unsigned long long n, int k);
static unsigned long long hash (stack_prot *test);


#define PROTECTION

#ifdef PROTECTION
    #define CHECK(stk) {                                \
        char StackOK_err = StackOK(stk);                \
        if(StackOK_err) StackDump(stk, StackOK_err);    \
    }
    #define PROT(...) __VA_ARGS__
#else
    #define CHECK(...)  
    #define PROT(...)   
#endif

static const int CODE = GetCode();


static int GetCode () {

    time_t now = time(NULL);
    void *tmp = calloc(1, 1);

    return now^((int) tmp);
}

int StackCtor (size_t base_size) {

    RET_ON_VAL(base_size > sizeof(size_t) / sizeof(elem_t) - 10 * sizeof(CANARY), ERR_ARG_INVAL, -1);

    if (!base_size) {
        base_size = 1;
    }

    elem_t *values = (elem_t*) calloc(base_size*sizeof(elem_t) + 2*sizeof(CANARY), 1);
    RET_ON_VAL(IsBadReadPtr(values, _msize(values)), ERR_NULL_PTR, -1);

    SetBirds(values, base_size);
    FillbyPoison((elem_t*) ((char*)values + sizeof(CANARY)), base_size);

    stack_prot *stk = (stack_prot*) calloc(1, sizeof(stack_prot));
    RET_ON_VAL(IsBadReadPtr(stk, _msize(stk)), ERR_NULL_PTR, -1);

    stk->StartBird = CANARY;

    stk->values = (elem_t*) ((char*) values + sizeof(CANARY));

    stk->size = 0;
    stk->capacity = base_size;

    stk->factor = 2.0;

    stk->isdump = false;

    stk->StackHash = 0;

    stk->EndBird = CANARY;

    PROT(stk->StackHash = hash(stk));

    FILE *dump = fopen("dump.txt", "w");
    assert(dump);
    fclose(dump);

    int code = ((int) stk)^CODE;

    CHECK(stk);

    return code;
}

static void SetBirds (void *mas, size_t size) {

    *((unsigned long*) mas) = CANARY;
    *((unsigned long*) ((char*)mas + size*sizeof(elem_t) + sizeof(CANARY))) = CANARY;
}

int StackPush (stack_t vstk, elem_t value) {

    stack_prot *stk = (stack_prot*) (vstk^CODE);
    CHECK(stk);

    stk->values[stk->size] = value;
    stk->size++;

    PROT(stk->StackHash = hash(stk));

    if (stk->size >= stk->capacity) {
        ResizeUp(stk);
    }

    CHECK(stk);

    return 0;
}

elem_t StackPop (stack_t vstk) {

    stack_prot *stk = (stack_prot*) (vstk^CODE);
    CHECK(stk);

    stk->size--;
    CHECK(stk);
    elem_t value = stk->values[stk->size];
    stk->values[stk->size] = POISON;

    PROT(stk->StackHash = hash(stk));

    if (stk->size <= stk->capacity / (stk->factor*stk->factor)) {
        ResizeDown(stk);
    }
    
    CHECK(stk);

    return value;
}

static int ResizeUp (stack_prot *stk) {

    CHECK(stk);

    elem_t *tmp = (elem_t*) realloc((char*)stk->values - sizeof(CANARY),\
                                    (size_t) (stk->capacity*stk->factor*sizeof(elem_t)) + 2*sizeof(CANARY));

    SetBirds(tmp, (size_t) (stk->capacity * stk->factor));
    FillbyPoison((elem_t*) ((char*)tmp + sizeof(CANARY) + stk->capacity*sizeof(elem_t)), stk->capacity);

    stk->values = (elem_t*) ((char*)tmp + sizeof(CANARY));
    stk->capacity = (size_t) (stk->capacity * stk->factor);

    PROT(stk->StackHash = hash(stk));

    CHECK(stk);

    return 1;
}

static int ResizeDown (stack_prot *stk) {

    CHECK(stk);

    elem_t *tmp = (elem_t*) realloc((char*)stk->values - sizeof(CANARY),\
                                    (size_t) (stk->capacity/stk->factor*sizeof(elem_t)) + 2*sizeof(CANARY));
    SetBirds(tmp, (size_t) (stk->capacity / stk->factor));

    stk->values = (elem_t*) ((char*)tmp + sizeof(CANARY));
    stk->capacity = (size_t) (stk->capacity / stk->factor);

    PROT(stk->StackHash = hash(stk));
    CHECK(stk);

    return 1;
}

static void FillbyPoison (elem_t *start, size_t size) {

    for (size_t i = 0; i < size; i++) {
        start[i] = POISON;
    }
}

static char StackOK (stack_prot *stk) {

    char err = 0;

    if (IsBadReadPtr(stk, _msize(stk))) {
        SetErr(&err, ERR_INVAL_STACK_PTR);
        return err;
    }

    if (stk->values == 0) {
        SetErr(&err, ERR_INVAL_DATA_PTR);
    }
    if (stk->size > stk->capacity) {
        SetErr(&err, ERR_SIZE_BIGGER_CAPACITY);
    }
    if(stk->StartBird != CANARY || stk->EndBird != CANARY) {
        SetErr(&err, ERR_STACK_CANARY_DEAD);
    }
    if (*((unsigned long*) ((char*)stk->values - sizeof(CANARY))) != CANARY ||\
        *((unsigned long*) ((char*)stk->values + stk->capacity*sizeof(elem_t))) != CANARY) {
        SetErr(&err, ERR_DATA_CANARY_DEAD);
    }
    if (hash(stk) != stk->StackHash) {
        SetErr(&err, ERR_INVAL_HASH);
    }

    return err;
}

static void SetErr (char *err, size_t num) {

    *err |= (char) (1 << (num-1));
}

static void StackDump (stack_prot *stk, char err) {

    int isabort = 0;

    stk->isdump = true;

    FILE *dump = fopen("dump.txt", "a");

    fprintf(dump, "Errors:\n");
    for (size_t i = 0; i < sizeof(err) * 8; i++) {
        if(err % 2) {
            if (i <= 2) {
                isabort = 1;
            }

            fprintf(dump, "Error № %d\n", i+1);
        }

        err /= 2;
    }
    fprintf(dump, "\n");

    fprintf(dump, "Stack:\n");
    fprintf(dump, "StartBird:  %ld\n", stk->StartBird);
    fprintf(dump, "values ptr: %p\n", stk->values);
    fprintf(dump, "size:       %d\n", stk->size);
    fprintf(dump, "capacity:   %d\n", stk->capacity);
    fprintf(dump, "factor:     %lg\n", stk->factor);
    fprintf(dump, "hash:       %lu\n", (unsigned long) stk->StackHash);
    fprintf(dump, "EndBird:    %ld\n", stk->EndBird);
    fprintf(dump, "\n");

    fprintf(dump, "Values of Stack:\n");
    for (size_t i = 0; i < stk->capacity; i++) {
        if(stk->values[i] != POISON) {
            fprintf(dump, "%d ", stk->values[i]);
        }
        else {
            fprintf(dump, "POISON ");
        }
    }
    fprintf(dump, "\n");

    fprintf(dump, "<<------------------------------->>\n\n");

    fclose(dump);

    if(isabort) {
        printf("Critical stack error, check dump and be fine UwU!");
        abort();
    }
}

void StackDtor (stack_t vstk) {

    stack_prot *stk = (stack_prot*) (vstk^CODE);
    CHECK(stk);

    free((char*) stk->values - sizeof(CANARY));
    free(stk);
}

static unsigned long long pow (unsigned long long n, int k) {

    if(k <= 0) return 1;
    if(k % 2 == 0) return pow(n, k / 2) * pow(n, k / 2);

    return n * pow(n, k - 1);
}

static unsigned long long hash (stack_prot *stk) {

    RET_ON_VAL(!stk, ERR_NULL_PTR, 0);
    
    unsigned long long tmp = stk->StackHash;
    stk->StackHash = 0;

    unsigned long long n = 0;

    size_t num = 0;
    for (size_t i = 0; i < sizeof(*stk); i++) {
        num++;
        n += (*((char*)stk + i) + 128) * pow(313, num);
    }

    for (size_t i = 0; i < stk->capacity * sizeof(elem_t) + 2*sizeof(CANARY); i++) {
        num++;
        n += *((char*) stk->values - sizeof(CANARY) + i) * pow(313, num);
    }

    stk->StackHash = tmp;

    return n;
}
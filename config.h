#ifndef _CONFIG_H
#define _CONFIG_H


#define ERR_PROCESS_ON


enum Errors {
    ERR_NULL_PTR    = 1,        ///< NULL pointer
    ERR_INVAL_ARG   = 2,        ///< invaluable arg
    ERR_INCRR_TESTS = 3,        ///< incorrect tests
    ERR_DVSN_ZERO   = 4,        ///< devision on zero
    ERR_ACC_DENi    = 5,        ///< access denied
    ERR_WRONG_SIGN  = 6,
    ERR_WRONG_VER   = 7,
    ERR_UNKNOWN_CMD = 8,
    ERR_INCRR_MARKS = 9,
    ERR_SEG_ERR     = 10,
};

enum Warnings {
    WARN_FREE_NULL_PTR = 1,     ///< Free null pointer
};


#ifdef ERR_PROCESS_ON

    #define RET_ON_VAL(err_cond, err_code, ret_val) {                                                       \
        if (err_cond) {                                                                                     \
            printf("Error code: %s in file: %s in function %s in line %d\n", #err_code, __FILE__,           \
            __PRETTY_FUNCTION__, __LINE__);                                                                 \
            return ret_val;                                                                                 \
        }                                                                                                   \
    }

    #define ISWARN(warn_cond, warn_code) {                                                                  \
        if (warn_cond) {                                                                                    \
            printf("Warning code: %s in file: %s in function %s in line %d\n", #warn_code, __FILE__,        \
            __PRETTY_FUNCTION__, __LINE__);                                                                 \
        }                                                                                                   \
    }

#else

    #define ISERR(err_cond, err_code, ret_val) 
    #define ISWARN(warn_code, warn_code)  

#endif


#endif // _CONFIG

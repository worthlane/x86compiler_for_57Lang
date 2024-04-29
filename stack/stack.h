#ifndef __STACK_H_
#define __STACK_H_

#include <stdio.h>

#include "common/errors.h"
#include "common/logs.h"
#include "nametable.h"
#include "types.h"

/*! \file
* \brief Contains hash functions
*/

#ifndef CANARY_PROTECT
/************************************************************//**
 * @brief Canary protection (if canary gets changed, program returns error)
 *
 * 1 for ON
 * 0 for OFF
 ************************************************************/
#define CANARY_PROTECT 0

#endif

#ifndef HASH_PROTECT
/************************************************************//**
 * @brief Hash protection (if hash is not equal to hash, that we expected, program returns error)
 *
 * 1 for ON
 * 0 for OFF
 ************************************************************/
#define HASH_PROTECT 0

#endif

#if CANARY_PROTECT
#define ON_CANARY(...) __VA_ARGS__
#define OFF_CANARY(...) ;

#else
#define ON_CANARY(...) ;
#define OFF_CANARY(...) __VA_ARGS__
#endif

#if HASH_PROTECT
#define ON_HASH(...) __VA_ARGS__

#else
#define ON_HASH(...) ;
#endif

#ifdef STACK_DUMP
#undef STACK_DUMP

#endif
#define STACK_DUMP(stk)     LogDump(StackDump, stk, __func__, __FILE__, __LINE__)

typedef nametable_t* elem_t;

static const size_t MIN_CAPACITY = 16;
static const elem_t STK_POISON   = nullptr;

/// stack type
typedef struct Stack Stack_t;

/// @brief Stack structure
struct Stack
{
    ON_CANARY
    (
        /// stack prefix canary
        canary_t stack_prefix;
    )

    /// stack data
    elem_t* data;
    /// stack size
    size_t size;
    /// stack capacity
    size_t capacity;
    /// stack status (0 if everything is fine)
    int status;

    ON_HASH
    (
        /// hash function
        hash_f hash_func;
        /// data hash
        hash_t data_hash;
        /// stack hash
        hash_t stack_hash;
    )

    ON_CANARY
    (
        /// stack postfix canary
        canary_t stack_postfix;
    )
};

/// @brief list of stack conditions
enum StackCondition
{
    /// everything is fine
    OK                   = 0,

    /// stack has invalid capacity (<= 0)
    INVALID_CAPACITY     = 1 << 1,
    /// stack size is 0
    EMPTY_STACK          = 1 << 2,
    /// stack has invalid size (> capacity)
    INVALID_SIZE         = 1 << 3,
    /// stack contains invalid data
    INVALID_DATA         = 1 << 4,
    /// access to poisoned element
    POISON_ACCESS        = 1 << 5,

    /// data canary triggered (canary protection mode)
    DATA_CANARY_TRIGGER  = 1 << 6,
    /// stack canary triggered (canary protection mode)
    STACK_CANARY_TRIGGER = 1 << 7,

    /// invalid hash function (hash protect mode)
    INVALID_HASH_FUNC    = 1 << 8,
    /// not expected data hash (hash protect mode)
    INCORRECT_DATA_HASH  = 1 << 9,
    /// not expected stack hash (hash protect mode)
    INCORRECT_STACK_HASH = 1 << 10
};

/************************************************************//**
 * @brief Creates stack
 *
 * @param[in] stk stack pointer
 * @param[in] capacity stack capacity
 * @return int error code
 *************************************************************/
int StackCtor(Stack_t* stk, size_t capacity = MIN_CAPACITY);

/************************************************************//**
 * @brief Destroys stack
 *
 * @param[in] stk stack pointer
 * @return int error code
 ************************************************************/
int StackDtor(Stack_t* stk);

/************************************************************//**
 * @brief Pushes element in stack
 *
 * @param[in] stk stack pointer
 * @param[in] value element
 * @return int error code
 ************************************************************/
int StackPush(Stack_t* stk, elem_t value);

/************************************************************//**
 * @brief Pops element from stack
 *
 * @param[in] stk stack pointer
 * @param[out] ret_value popped element
 * @return int error code
 ************************************************************/
int StackPop(Stack_t* stk, elem_t* ret_value = nullptr);

/************************************************************//**
 * @brief Prints info about stack in output stream
 *
 * @param[in] fp output stream
 * @param[in] stk stack pointer
 * @param[in] func function, where print called
 * @param[in] file file, where print called
 * @param[in] line line, where print caled
 * @return int error code
 ************************************************************/
int StackDump(FILE* fp, const void* stk, const char* func, const char* file, const int line);

/************************************************************//**
 * @brief Verifies stack
 *
 * @param[in] stk stack pointer
 * @return int stack condition code
 ************************************************************/
int StackOk(const Stack_t* stk);

bool IsStackValid(Stack* stack, const char* func, const char* file, const int line);

#ifdef CHECK_STACK
#undef CHECK_STACK

#endif
#define CHECK_STACK(stk)    do                                                          \
                            {                                                           \
                                if (!IsStackValid(stk, __func__, __FILE__, __LINE__))   \
                                    return (int) ERRORS::INVALID_STACK;                 \
                            } while(0)

#endif

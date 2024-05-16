#include <stdlib.h>
#include <assert.h>

#include "stack.h"
#include "common/logs.h"
#include "hash.h"

// ============= STATIC FUNCS ===============
static inline bool EmptyStackCheck(Stack_t* stk);

static void InitCanary(canary_t* prefix_canary, canary_t* postfix_canary);
static bool VerifyCanary(const canary_t* prefix_canary, const canary_t* postfix_canary);
static canary_t* GetPostfixDataCanary(const Stack_t* stk);
static canary_t* GetPrefixDataCanary(const Stack_t* stk);

static size_t CountDataSize(const size_t capacity);

static hash_t GetDataHash(const Stack_t* stk);
static hash_t GetStackHash(const Stack_t* stk);
static bool VerifyDataHash(const Stack_t* stk);
static bool VerifyStackHash(const Stack_t* stk);
static inline void ReInitAllHashes(Stack_t* stk);

static int StackRealloc(Stack_t* stk, size_t new_capacity);

static void PrintStackCondition(const Stack_t* stk);
static int PrintStackData(FILE* fp, const Stack_t* stk);

static void PoisonData(elem_t* left_border, elem_t* right_border);
static bool PoisonVerify(Stack_t* stk);

static bool Equal(const elem_t a, const elem_t b);
//============================================


// =============CONSTS============
static const canary_t canary_val = 0xD07ADEAD;
// ===============================

int StackCtor(Stack_t* stk, size_t capacity)
{
    assert(stk);

    elem_t* data       = nullptr;
    size_t data_size   = CountDataSize(capacity);

    data = (elem_t*) calloc(data_size, 1);

    if (data == nullptr)
        return (int) ERRORS::ALLOCATE_MEMORY;

    elem_t* first_elem = data;

    ON_CANARY
    (
        first_elem               = (elem_t*)((char*) data + sizeof(canary_t));
        canary_t* prefix_canary  = (canary_t*) data;
        canary_t* postfix_canary = (canary_t*)((char*) data + capacity * sizeof(elem_t) + sizeof(canary_t));

        InitCanary(prefix_canary, postfix_canary);

        InitCanary(&stk->stack_prefix, &stk->stack_postfix)
    );

    stk->data     = first_elem;
    stk->size     = 0;
    stk->capacity = capacity;
    stk->status   = OK;

    PoisonData(stk->data, (elem_t*)((char*)stk->data + stk->capacity * sizeof(elem_t)));

    ON_HASH
    (
        if (stk->hash_func == nullptr)
            stk->hash_func = MurmurHash
    );

    ReInitAllHashes(stk);

    CHECK_STACK(stk);

    return (int) ERRORS::NONE;
}

//-----------------------------------------------------------------------------------------------------

int StackDtor(Stack_t* stk)
{
    assert(stk);

    CHECK_STACK(stk);

    OFF_CANARY(elem_t* data = stk->data);

    ON_CANARY(elem_t* data = (elem_t*)((char*) stk->data - sizeof(canary_t)));

    // TODO mem leak

    free(data);

    stk->data     = nullptr;
    stk->size     = 0;
    stk->capacity = 0;
    stk->status   = OK;

    ON_CANARY
    (
        stk->stack_prefix  = 0;
        stk->stack_postfix = 0
    );

    ON_HASH
    (
        stk->hash_func  = nullptr;
        stk->stack_hash = 0;
        stk->data_hash  = 0
    );

    return (int) ERRORS::NONE;
}

//-----------------------------------------------------------------------------------------------------

int StackPush(Stack_t* stk, elem_t value)
{
    assert(stk);
    assert(stk->data);

    CHECK_STACK(stk);

    if (stk->capacity == stk->size)
    {
        if (StackRealloc(stk, stk->capacity << 1) != (int) ERRORS::NONE)
            return (int) ERRORS::ALLOCATE_MEMORY;
    }

    (stk->data)[(stk->size)++] = value;

    ReInitAllHashes(stk);

    CHECK_STACK(stk);

    return (int) ERRORS::NONE;
}

//-----------------------------------------------------------------------------------------------------

static int StackRealloc(Stack_t* stk, size_t new_capacity)
{
    assert(stk);

    CHECK_STACK(stk);

    if (new_capacity < MIN_CAPACITY)
        new_capacity = MIN_CAPACITY;

    elem_t* data       = stk->data;
    elem_t* first_elem = data;
    size_t new_size    = CountDataSize(new_capacity);

    ON_CANARY
    (
        data = (elem_t*)((char*) data - sizeof(canary_t))
    );

    elem_t* temp = (elem_t*) realloc(data, new_size);

    if (temp == nullptr)
    {
        StackDtor(stk);

        return (int) ERRORS::ALLOCATE_MEMORY;
    }
    else
        data = temp;

    ON_CANARY
    (
        first_elem               = (elem_t*)((char*) data + sizeof(canary_t));
        canary_t* postfix_canary = (canary_t*)((char*) first_elem + new_capacity * sizeof(elem_t));

        *(postfix_canary) = canary_val
    );

    stk->data     = first_elem;
    stk->capacity = new_capacity;

    PoisonData((elem_t*)((char*)stk->data + stk->size * sizeof(elem_t)),
               (elem_t*)((char*)stk->data + stk->capacity * sizeof(elem_t)));

    ReInitAllHashes(stk);

    CHECK_STACK(stk);

    return (int) ERRORS::NONE;
}

//-----------------------------------------------------------------------------------------------------

int StackPop(Stack_t* stk, elem_t* ret_value)
{
    assert(stk);
    assert(stk->data);

    if (EmptyStackCheck(stk))
    {
        stk->status |= EMPTY_STACK;
        STACK_DUMP(stk);
        return (int) ERRORS::INVALID_STACK;
    }

    CHECK_STACK(stk);

    if (ret_value)
    {
        *(ret_value) = (stk->data)[--(stk->size)];
        NametableDtor((nametable_t*) ret_value);
    }
    else
    {
        stk->size--;
    }

    (stk->data)[(stk->size)] = STK_POISON;

    if (stk->size <= stk->capacity >> 2)
    {
        ReInitAllHashes(stk);
        int realloc_error  = StackRealloc(stk, stk->capacity >> 1);
        if (realloc_error != (int) ERRORS::NONE)
            return realloc_error;
    }

    ReInitAllHashes(stk);

    CHECK_STACK(stk);

    return (int) ERRORS::NONE;
}

//-----------------------------------------------------------------------------------------------------

int StackOk(const Stack_t* stack)
{
    assert(stack);

#pragma GCC diagnostic ignored "-Wcast-qual"
    Stack_t* stk = (Stack_t*) stack;
#pragma GCC diagnostic warning "-Wcast-qual"

    ON_CANARY
    (
        canary_t* prefix_canary  = GetPrefixDataCanary(stk);
        canary_t* postfix_canary = GetPostfixDataCanary(stk);

        if (!VerifyCanary(prefix_canary, postfix_canary))           stk->status |= DATA_CANARY_TRIGGER;
        if (!VerifyCanary(&stk->stack_prefix, &stk->stack_postfix)) stk->status |= DATA_CANARY_TRIGGER
    );

    if (stk->capacity <= 0)                                         stk->status |= INVALID_CAPACITY;
    if (stk->size > stk->capacity)                                  stk->status |= INVALID_SIZE;
    if (stk->data == nullptr && stk->capacity != 0)                 stk->status |= INVALID_DATA;
#pragma GCC diagnostic ignored "-Wcast-qual"
    if (!PoisonVerify((Stack_t*) stk))                              stk->status |= POISON_ACCESS;
#pragma GCC diagnostic warning "-Wcast-qual"

    ON_HASH
    (
        if (!stk->hash_func)                                        stk->status |= INVALID_HASH_FUNC;

        if (!VerifyDataHash(stk))                                   stk->status |= INCORRECT_DATA_HASH;
        if (!VerifyStackHash(stk))                                  stk->status |= INCORRECT_STACK_HASH
    );

    return stk->status;
}

//-----------------------------------------------------------------------------------------------------

static void InitCanary(canary_t* prefix_canary, canary_t* postfix_canary)
{
    assert(prefix_canary);
    assert(postfix_canary);
    assert(prefix_canary != postfix_canary);

    *prefix_canary  = canary_val;
    *postfix_canary = canary_val;
}

//-----------------------------------------------------------------------------------------------------

static bool VerifyCanary(const canary_t* prefix_canary, const canary_t* postfix_canary)
{
    assert(prefix_canary);
    assert(postfix_canary);
    assert(prefix_canary != postfix_canary);

    if (*prefix_canary  != canary_val)
        return false;

    if (*postfix_canary != canary_val)
        return false;

    return true;
}

//-----------------------------------------------------------------------------------------------------

static bool VerifyStackHash(const Stack_t* stk)
{
    assert(stk);

    ON_HASH
    (
        hash_t current_hash = stk->stack_hash;

        if (current_hash != GetStackHash(stk))
            return false;

        return true
    );

    return false;
}

//-----------------------------------------------------------------------------------------------------

static bool VerifyDataHash(const Stack_t* stk)
{
    assert(stk);

    ON_HASH
    (
        hash_t current_hash = stk->data_hash;

        if (current_hash != GetDataHash(stk))
            return false;

        return true
    );

    return false;
}

//-----------------------------------------------------------------------------------------------------

static hash_t GetDataHash(const Stack_t* stk)
{
    assert(stk);

    hash_t new_hash = 0;

    ON_HASH
    (
        elem_t* data = stk->data;
        size_t data_size = stk->capacity * sizeof(elem_t);

        new_hash = stk->hash_func(data, data_size);
    );

    return new_hash;
}

//-----------------------------------------------------------------------------------------------------

static hash_t GetStackHash(const Stack_t* stk)
{
    assert(stk);

    hash_t new_hash = 0;

    size_t stk_size    = sizeof(Stack_t);
    const Stack_t* stack_ptr = stk;

#pragma GCC diagnostic ignored "-Wcast-qual"
    ON_CANARY
    (
        stk_size -= 2 * sizeof(canary_t);
        stack_ptr = (Stack_t*) ((char*) stk + sizeof(canary_t));
    )

    ON_HASH
    (
        hash_t stack_hash = stk->stack_hash;
        int    status     = stk->status;
        ((Stack_t*) stk)->status     = 0;
        ((Stack_t*) stk)->stack_hash = 0;

        new_hash = stk->hash_func(stack_ptr, stk_size);

        ((Stack_t*) stk)->stack_hash = stack_hash;
        ((Stack_t*) stk)->status     = status;
    )
#pragma GCC diagnostic warning "-Wcast-qual"

    return new_hash;;
}

//-----------------------------------------------------------------------------------------------------

int StackDump(FILE* fp, const void* stack, const char* func, const char* file, const int line)
{
    assert(stack);
    assert(func);
    assert(file);

    const Stack_t* stk = (const Stack_t*) stack;

    LOG_START_DUMP(func, file, line);

    fprintf(fp, "Stack                > [%p]\n"
                "size                 > %zu\n"
                "capacity             > %zu\n"
                "data place           > [%p]\n",
                stk, stk->size, stk->capacity, stk->data);

    ON_CANARY
    (
        fprintf(fp, "STACK PREFIX CANARY  > %llX\n"
                    "STACK POSTFIX CANARY > %llX\n",
                    stk->stack_prefix, stk->stack_postfix)
    );

    ON_HASH
    (
        fprintf(fp, "HASH FUNCTION        > [%p]\n"
                    "::::::EXPECTED HASH::::::\n"
                    "STACK HASH           > %u\n"
                    "DATA HASH            > %u\n"
                    "::::::CURRENT HASH::::::\n"
                    "STACK CURRENT        > %u\n"
                    "DATA CURRENT         > %u\n",
                    stk->hash_func, stk->stack_hash, stk->data_hash,
                    GetStackHash(stk), GetDataHash(stk))
    );

    fprintf(fp, "ELEMENTS: \n\n");

    PrintStackData(fp, stk);

    ON_CANARY
    (
        canary_t* prefix_canary  = GetPrefixDataCanary(stk);
        canary_t* postfix_canary = GetPostfixDataCanary(stk);

        fprintf(fp, "PREFIX DATA CANARY  > %llX\n"
                    "POSTFIX DATA CANARY > %llX\n", *prefix_canary, *postfix_canary)
    );

    StackOk(stk);
    if (stk->status != OK)
        PrintStackCondition(stk);

    LOG_END();

    return (int) ERRORS::NONE;
}

//-----------------------------------------------------------------------------------------------------

static inline void ReInitAllHashes(Stack_t* stk)
{
    ON_HASH
    (
        stk->data_hash  = GetDataHash(stk);
        stk->stack_hash = GetStackHash(stk)
    );
}

//-----------------------------------------------------------------------------------------------------

static size_t CountDataSize(const size_t capacity)
{
    size_t size = capacity * sizeof(elem_t);

    ON_CANARY
    (
        size += 2 * sizeof(canary_t);
        size += size % 8;
    );

    return size;
}

//-----------------------------------------------------------------------------------------------------

static inline bool EmptyStackCheck(Stack_t* stk)
{
    if (stk->size == 0)
        return true;

    return false;
}

//-----------------------------------------------------------------------------------------------------

static canary_t* GetPostfixDataCanary(const Stack_t* stk)
{
    canary_t* postfix_canary = (canary_t*)((char*) stk->data +
                                            CountDataSize(stk->capacity) - 2 * sizeof(canary_t));
    return postfix_canary;
}

//-----------------------------------------------------------------------------------------------------

static canary_t* GetPrefixDataCanary(const Stack_t* stk)
{
    canary_t* prefix_canary  = (canary_t*)((char*) stk->data - sizeof(canary_t));

    return prefix_canary;
}

//-----------------------------------------------------------------------------------------------------

static void PrintStackCondition(const Stack_t* stk)
{
    PrintLog("\n>>>>>>>>>>STACK CONDITIONS<<<<<<<<<\n");

    if ((stk->status & INVALID_CAPACITY) != 0)
        PrintLog("INVALID STACK CAPACITY\n"
                    "SIZE:     %zu\n"
                    "CAPACITY: %zu\n",
                    stk->size, stk->capacity);

    if ((stk->status & INVALID_SIZE) != 0)
        PrintLog("INVALID STACK SIZE\n"
                    "SIZE:     %zu\n",
                    stk->size);

    if ((stk->status & INVALID_DATA) != 0)
        PrintLog("INVALID STACK DATA\n"
                    "DATA:     [%p]\n",
                    stk->data);

    if ((stk->status & EMPTY_STACK) != 0)
        PrintLog("CAN NOT POP ELEMENT FROM EMPTY STACK\n");

    if ((stk->status & POISON_ACCESS) != 0)
        PrintLog("CAN NOT ACCESS TO POISONED ELEMENT\n");

    #if CANARY_PROTECT
    canary_t* prefix_canary  = GetPrefixDataCanary(stk);
    canary_t* postfix_canary = GetPostfixDataCanary(stk);

    if ((stk->status & DATA_CANARY_TRIGGER) != 0)
        PrintLog("DATA CANARY TRIGGERED\n"
                    "LEFT CANARY:     %llu\n"
                    "RIGHT CANARY:    %llu\n",
                    *prefix_canary, *postfix_canary);

    if ((stk->status & STACK_CANARY_TRIGGER) != 0)
        PrintLog("STACK CANARY TRIGGERED\n"
                    "LEFT CANARY:     %llu\n"
                    "RIGHT CANARY:    %llu\n",
                    stk->stack_prefix, stk->stack_postfix);
    #endif

    #if HASH_PROTECT

    if ((stk->status & INVALID_HASH_FUNC) != 0)
        PrintLog("INVALID HASH FUNCTION\n"
                    "FUNC:     [%p]\n",
                    stk->hash_func);

    if ((stk->status & INCORRECT_DATA_HASH) != 0)
        PrintLog("INCORRECT DATA HASH\n"
                    "EXPECTED:     %u\n"
                    "CURRENT:      %u\n",
                    stk->data_hash,
                    GetDataHash(stk));

    if ((stk->status & INCORRECT_STACK_HASH) != 0)
        PrintLog("INCORRECT STACK HASH\n"
                    "EXPECTED:     %u\n"
                    "CURRENT:      %u\n",
                    stk->stack_hash,
                    GetStackHash(stk));
    #endif

    PrintLog(">>>>>>>>STACK CONDITIONS END<<<<<<<\n\n");
}

//-----------------------------------------------------------------------------------------------------

bool IsStackValid(Stack* stack, const char* func, const char* file, const int line)
{
    StackOk(stack);
    if (stack->status != OK)
    {
        const void* stk = (const void*) stack;
        LogDump(StackDump, stk, func, file, line);
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------------------------------

static int PrintStackData(FILE* fp, const Stack_t* stk)
{
    for (size_t i = 0; i < stk->size; i++)
    {
        fprintf(fp, "*[%zu] > " PRINT_ELEM_T "\n", i, stk->data[i]);
    }

    fprintf(fp, "clear elements\n");

    for (size_t i = stk->size; i < stk->capacity; i++)
    {
        fprintf(fp, "*[%zu] > " PRINT_ELEM_T, i, stk->data[i]);
        if (Equal((stk->data[i]), STK_POISON))
            fprintf(fp, " (POISONED)");
        fprintf(fp, "\n");
    }

    return (int) ERRORS::NONE;
}

//-----------------------------------------------------------------------------------------------------

static void PoisonData(elem_t* left_border, elem_t* right_border)
{
    assert(left_border);
    assert(right_border);

    for (elem_t* iterator = left_border; iterator < right_border; iterator++)
    {
        *iterator = STK_POISON;
    }
}

//-----------------------------------------------------------------------------------------------------

static bool PoisonVerify(Stack_t* stk)
{
    elem_t* left_border  = (elem_t*)((char*)stk->data + stk->size * sizeof(elem_t));
    elem_t* right_border = (elem_t*)((char*)stk->data + stk->capacity * sizeof(elem_t));

    for (elem_t* iterator = left_border; iterator < right_border - 1; iterator++)
    {
        if (!Equal(STK_POISON, *iterator))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------------------------------

bool Equal(const elem_t a, const elem_t b)
{

    if (a == b)
        return true;

    // const elem_t EPSILON = 1e-5;

    return a == b;
}

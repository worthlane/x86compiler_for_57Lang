#ifndef _NAMETABLE_H_
#define _NAMETABLE_H_

#include <stdio.h>

static const size_t DEFAULT_NAMES_AMT  = 64;
static const size_t MAX_NAME_LEN       = 64;

// ======================================================================
// OPERATORS
// ======================================================================

#define DEF_OP(name, ...)  name,

enum class Operators
{
    #include "common/operations.h"

    // FICTIVE OPERATORS:

    FUNC_CALL,

    TYPE,
    NEW_FUNC,
    FUNC,

    UNK
};

#undef DEF_OP

// ======================================================================
// TOKENS
// ======================================================================

enum class TokenType
{
    NUM,
    TOKEN,
    NAME,

    FUNC_NAME,
    VAR_NAME,
};

union TokenValue
{
    Operators       opt;
    double          val;
    int             name_id;
};

struct token_t
{
    TokenType  type;
    TokenValue info;
    size_t     line;
};

struct name_t
{
    char*     name;

    TokenType type;

    int       ram_id;

    bool      is_arg;
};

struct nametable_t
{
    name_t* list;

    size_t size;
    size_t capacity;            // TODO пока не используется, переделать с реаллоком
};

nametable_t* MakeNametable();
void         NametableCtor(nametable_t* nametable);
void         NametableDtor(nametable_t* nametable);

void DumpNametable(FILE* fp, const nametable_t* nametable);

void CopyNametable(const nametable_t* nametable, nametable_t* dest);

int  InsertNameInTable(nametable_t* nametable, const char* name, const TokenType type = TokenType::NAME);
int  InsertPairInTable(nametable_t* nametable, const char* name, const int index);
bool FindNameInTable(const nametable_t* nametable, const char* name, bool* exists, bool* is_func);
int  GetValueFromNameTable(nametable_t* nametable, const char* name);

static const int UNKNOWN_VAL = 1337;

// ======================================================================
// KEYWORDS
// ======================================================================

int  InsertKeywordInTable(nametable_t* nametable, const char* name);
void FillNametableWithKeywords(nametable_t* nametable);
void GlobalNametableCtor(nametable_t* nametable);

Operators TranslateKeywordToOperator(const char* keyword);
Operators GetOperator(const char* word);
bool      IsType(const Operators type);
void      PrintOperator(FILE* fp, const Operators sign);

// ======================================================================
// WORD DIGITS FORMAT
// ======================================================================

enum Digits
{
    ZERO  = 0,
    ONE   = 1,
    TWO   = 2,
    THREE = 3,
    FOUR  = 4,
    FIVE  = 5,
    SIX   = 6,
    SEVEN = 7,
    EIGHT = 8,
    NINE  = 9,

    UNK = -1        // TODO unknown
};

#endif

#include <assert.h>
#include <string.h>

#include "nametable.h"

//-----------------------------------------------------------------------------------------------------

void GlobalNametableCtor(nametable_t* nametable)
{
    assert(nametable);

    NametableCtor(nametable);

    FillNametableWithKeywords(nametable);
}

//-----------------------------------------------------------------------------------------------------

int InsertKeywordInTable(nametable_t* nametable, const char* name)
{
    int id = InsertNameInTable(nametable, name);

    nametable->list[id].type = TokenType::TOKEN;

    return id;
}

//-----------------------------------------------------------------------------------------------------

#define DEF_OP(name, is_char, symb, ...)             \
        if (!is_char)                           \
        {                                       \
            InsertKeywordInTable(nametable, symb);  \
        }                                               \

void FillNametableWithKeywords(nametable_t* nametable)
{
    assert(nametable);

    #include "common/operations.h"
}

#undef DEF_OP

//-----------------------------------------------------------------------------------------------------

nametable_t* MakeNametable()
{
    nametable_t* names = (nametable_t*) calloc(1, sizeof(nametable_t));

    assert(names);

    NametableCtor(names);

    return names;
}

//-----------------------------------------------------------------------------------------------------

void NametableCtor(nametable_t* nametable)
{
    assert(nametable);

    name_t* list = (name_t*) calloc(DEFAULT_NAMES_AMT, sizeof(name_t));

    assert(list);

    nametable->list     = list;
    nametable->size     = 0;
    nametable->capacity = DEFAULT_NAMES_AMT;
}

//-----------------------------------------------------------------------------------------------------

void NametableDtor(nametable_t* nametable)
{
    assert(nametable);
    assert(nametable->list);

    for (int i = 0; i < nametable->size; i++)
    {
        if (nametable->list[i].name != nullptr)
            free(nametable->list[i].name);
    }

    free(nametable->list);

    nametable->size     = 0;
    nametable->capacity = 0;
}

//-----------------------------------------------------------------------------------------------------

void DumpNametable(FILE* fp, const nametable_t* nametable)
{
    assert(nametable);
    assert(nametable->list);

    fprintf(fp, "NAMETABLE SIZE > %d\n", nametable->size);

    for (int i = 0; i < nametable->size; i++)
    {
        if (nametable->list[i].name != nullptr)
            fprintf(fp, "\"%s\"[%d]\n", nametable->list[i].name, i);
    }
}

//-----------------------------------------------------------------------------------------------------

void CopyNametable(const nametable_t* nametable, nametable_t* dest)
{
    assert(nametable);
    assert(dest);

    dest->size     = nametable->size;
    dest->capacity = nametable->capacity;

    for (int i = 0; i < nametable->size; i++)
    {
        dest->list[i].type = nametable->list[i].type;
        dest->list[i].name = strndup(nametable->list[i].name, MAX_NAME_LEN);
    }
}

//-----------------------------------------------------------------------------------------------------

int InsertNameInTable(nametable_t* nametable, const char* name, const TokenType type)
{
    assert(nametable);
    assert(nametable->list);
    assert(name);

    for (int i = 0; i < nametable->size; i++)
    {
        nametable->list[i].is_arg = true; // TODO fix

        if (!strncmp(name, nametable->list[i].name, MAX_NAME_LEN))
            return i;
    }
    char* inserted_name = strndup(name, MAX_NAME_LEN);
    assert(inserted_name);

    nametable->list[nametable->size].name = inserted_name;
    nametable->list[nametable->size].type = type;

    return nametable->size++;
}

//-----------------------------------------------------------------------------------------------------

#define DEF_OP(name, ...)        \
    case (Operators::name):        \
        fprintf(fp, #name);   \
        break;                    \

void PrintOperator(FILE* fp, const Operators sign)
{
    switch (sign)
    {
        #include "common/operations.h"

        case (Operators::TYPE):
            fprintf(fp, "TYPE");
            break;

        case (Operators::FUNC):
            fprintf(fp, "FUNC");
            break;

        case (Operators::NEW_FUNC):
            fprintf(fp, "NEW_FUNC");
            break;

        case (Operators::FUNC_CALL):
            fprintf(fp, "FUNC_CALL");
            break;

        default:
            fprintf(fp, " undefined_operator ");
    }
}

#undef DEF_OP

//-----------------------------------------------------------------------------------------------------

#define DEF_OP(name, is_char, symb, ...)                     \
    if (!strncmp(keyword, symb, MAX_NAME_LEN))   \
    {                                           \
        return Operators::name;                       \
    }                                           \
    else


Operators TranslateKeywordToOperator(const char* keyword)
{
    if (!keyword) return Operators::UNK;

    #include "common/operations.h"

    /* else */ return Operators::UNK;
}

#undef DEF_OP

//-----------------------------------------------------------------------------------------------------

#define DEF_OP(op, ...)                     \
    if (!strncmp(word, #op, MAX_NAME_LEN))   \
    {                                           \
        return Operators::op;                       \
    }                                           \
    else


Operators GetOperator(const char* word)
{
    if (!word) return Operators::UNK;

    #include "common/operations.h"

    if (!strncmp(word, "FUNC_CALL", MAX_NAME_LEN))
    {
        return Operators::FUNC_CALL;
    }
    else if (!strncmp(word, "TYPE", MAX_NAME_LEN))
    {
        return Operators::TYPE;
    }
    else if (!strncmp(word, "NEW_FUNC", MAX_NAME_LEN))
    {
        return Operators::NEW_FUNC;
    }
    else if (!strncmp(word, "FUNC", MAX_NAME_LEN))
    {
        return Operators::FUNC;
    }
    else
        return Operators::UNK;

}

#undef DEF_OP

//-----------------------------------------------------------------------------------------------------

bool IsType(const Operators type)
{
    switch (type)
    {
        case (Operators::TYPE_INT):
            return true;
        default:
            return false;
    }

    return false;
}

//-----------------------------------------------------------------------------------------------------

bool FindNameInTable(const nametable_t* nametable, const char* name, bool* exists, bool* is_func)
{
    assert(nametable);
    assert(name);
    assert(is_func);
    assert(exists);

    for (int i = 0; i < nametable->size; i++)
    {
        if (!strncmp(name, nametable->list[i].name, MAX_NAME_LEN))
        {
            if (nametable->list[i].type == TokenType::FUNC_NAME)
                *is_func = true;
            *exists = true;
            return true;
        }
    }

    return false;
}




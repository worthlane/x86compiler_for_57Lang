#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "analysis.h"
#include "common/file_read.h"
#include "common/errors.h"
#include "tree/tree_output.h"

// ======================================================================
// INPUT PARSE
// ======================================================================

enum class CharType
{
    DIGIT,
    ALPHA,
    ADDITIONAL,
    OPT,

    UNK,
};

static CharType CheckBufCharType(LinesStorage* text);
static CharType GetCharType(const int ch);

static void     SkipSymbols(LinesStorage* text);
static bool     SkipComments(LinesStorage* text);
static bool     SkipSpaces(LinesStorage* text);

static bool     CanCharBeInName(const int ch);

static void     ReadName(LinesStorage* text, char* buffer);
static Digits   ReadDigit(LinesStorage* text, char* buffer);

// ======================================================================
// TOKENIZATOR
// ======================================================================

static FrontendErrors TokenizeOperator(LinesStorage* text, Tokens* storage, error_t* error);
static FrontendErrors TokenizeWord(LinesStorage* text, Tokens* storage, error_t* error);
static FrontendErrors TokenizeNumber(LinesStorage* text, Tokens* storage, error_t* error);

//-----------------------------------------------------------------------------------------------------

FrontendErrors Tokenize(LinesStorage* text, Tokens* storage, error_t* error)
{
    assert(text);
    assert(storage);
    assert(error);

    while(text->ptr < text->text_len)
    {
        SkipSymbols(text);

        CharType type = CheckBufCharType(text);

        if (type == CharType::DIGIT || type == CharType::ADDITIONAL)
        {
            TokenizeWord(text, storage, error);
            RETURN_IF_FRONTEND_ERROR;
        }
        else if (type == CharType::OPT)
        {
            TokenizeOperator(text, storage, error);
            RETURN_IF_FRONTEND_ERROR;
        }
        else if (type == CharType::ALPHA)
        {
            TokenizeNumber(text, storage, error);
            RETURN_IF_FRONTEND_ERROR;
        }
        else
        {
            SET_FRONTEND_ERROR(INVALID_SYNTAX, "UNKNOWN SYMBOL IN LINE %d", text->curr_line);
        }
    }

    return (FrontendErrors) error->code;
}

//-----------------------------------------------------------------------------------------------------

static FrontendErrors TokenizeWord(LinesStorage* text, Tokens* storage, error_t* error)
{
    assert(text);
    assert(storage);
    assert(error);

    size_t line = text->curr_line;

    token_t* token = &storage->array[storage->size];

    char buffer[MAX_NAME_LEN] = "";
    ReadName(text, buffer);

    int id = InsertNameInTable(&storage->all_names, buffer);

    if (storage->all_names.list[id].type == TokenType::TOKEN)
    {
        Operators op    = TranslateKeywordToOperator(storage->all_names.list[id].name);
        token->type     = TokenType::TOKEN;
        token->info.opt = op;
    }
    else
    {
        token->type         = storage->all_names.list[id].type;
        token->info.name_id = id;
    }

    token->line = line;
    storage->size++;

    return (FrontendErrors) error->code;
}

//-----------------------------------------------------------------------------------------------------

static FrontendErrors TokenizeNumber(LinesStorage* text, Tokens* storage, error_t* error)
{
    assert(text);
    assert(storage);
    assert(error);

    size_t   line  = text->curr_line;
    token_t* token = &storage->array[storage->size];

    char buffer[MAX_NAME_LEN] = "";

    int val = ReadDigit(text, buffer);
    if (val == Digits::UNK)
        SET_FRONTEND_ERROR(INVALID_SYNTAX, "CAN NOT READ NUMBER IN LINE %d", text->curr_line);

    int ch = Bufgetc(text);
    while (ch == '_')
    {
        int val_2 = ReadDigit(text, buffer);
        if (val_2 == Digits::UNK)
            SET_FRONTEND_ERROR(INVALID_SYNTAX, "CAN NOT READ NUMBER IN LINE %d", text->curr_line);

        val = val * 10 + val_2;

        ch = Bufgetc(text);
    }
    Bufungetc(text);

    token->type     = TokenType::NUM;
    token->info.val = val;
    token->line     = line;
    storage->size++;

    return (FrontendErrors) error->code;
}

//-----------------------------------------------------------------------------------------------------

#define DEF_OP(name, is_char, symb, ...)    \
        if (ch == symb[0] && is_char)                  \
        {                                   \
            op = Operators::name;            \
        }                                    \
        else                                \

static FrontendErrors TokenizeOperator(LinesStorage* text, Tokens* storage, error_t* error)
{
    assert(text);
    assert(storage);
    assert(error);

    size_t line = text->curr_line;
    int    ch   = Bufgetc(text);

    Operators op = Operators::UNK;

    #include "common/operations.h"

    /* else */ SET_FRONTEND_ERROR(UNKNOWN_OPERATOR, "%c IN LINE %d", ch, text->curr_line);

    token_t* token = &storage->array[storage->size];

    token->info.opt = op;
    token->line     = line;
    token->type     = TokenType::TOKEN;

    storage->size++;

    return (FrontendErrors) error->code;
}

#undef DEF_OP

//-----------------------------------------------------------------------------------------------------

#ifdef IS_DIGIT
#undef IS_DIGIT
#endif
#define IS_DIGIT(digit)                                                \
        if (strncasecmp(buffer, #digit, MAX_NAME_LEN) == 0)            \
        {                                                       \
            return Digits::digit;                                        \
        }

static Digits ReadDigit(LinesStorage* text, char* buffer)
{
    assert(text);
    assert(buffer);

    int i = 0;

    int ch = Bufgetc(text);

    while ((isalpha(ch)) && i < MAX_NAME_LEN)
    {
        buffer[i++] = ch;
        ch = Bufgetc(text);
    }
    buffer[i] = '\0';

    Bufungetc(text);

    IS_DIGIT(ZERO);
    IS_DIGIT(ONE);
    IS_DIGIT(TWO);
    IS_DIGIT(THREE);
    IS_DIGIT(FOUR);
    IS_DIGIT(FIVE);
    IS_DIGIT(SIX);
    IS_DIGIT(SEVEN);
    IS_DIGIT(EIGHT);
    IS_DIGIT(NINE);

    return Digits::UNK;
}

#undef IS_DIGIT

//-----------------------------------------------------------------------------------------------------

static void ReadName(LinesStorage* text, char* buffer)
{
    assert(text);
    assert(buffer);

    int i = 0;

    int ch = Bufgetc(text);

    while ((CanCharBeInName(ch)) && i < MAX_NAME_LEN)
    {
        buffer[i++] = ch;
        ch = Bufgetc(text);
    }
    buffer[i] = '\0';

    Bufungetc(text);
}

//-----------------------------------------------------------------------------------------------------

static bool CanCharBeInName(const int ch)
{
    CharType type = GetCharType(ch);

    if (type == CharType::DIGIT || type == CharType::ADDITIONAL)
        return true;
    else
        return false;
}

//-----------------------------------------------------------------------------------------------------

static void SkipSymbols(LinesStorage* text)
{
    assert(text);

    bool need_to_skip = true;

    while (need_to_skip)
        need_to_skip = SkipSpaces(text) || SkipComments(text);
}

//-----------------------------------------------------------------------------------------------------

static bool SkipSpaces(LinesStorage* info)
{
    int ch       = 0;
    bool skipped = false;

    ch = Bufgetc(info);

    while (ch == ' ' || ch == '\t')
    {
        ch = Bufgetc(info);
        skipped = true;
    }

    Bufungetc(info);

    return skipped;
}

//-----------------------------------------------------------------------------------------------------

static bool SkipComments(LinesStorage* text)
{
    int ch_1 = Bufgetc(text);
    int ch_2 = Bufgetc(text);
    bool skipped = false;

    if (ch_1 == '/' && ch_2 == '/')
    {
        int ch = Bufgetc(text);

        while (ch != '\0' && ch != '\n')
            ch = Bufgetc(text);

        skipped = true;
    }
    else
    {
        Bufungetc(text);
        Bufungetc(text);
    }

    return skipped;
}

//-----------------------------------------------------------------------------------------------------

static CharType CheckBufCharType(LinesStorage* text)
{
    assert(text);

    int ch = Bufgetc(text);
    Bufungetc(text);

    return GetCharType(ch);
}

//-----------------------------------------------------------------------------------------------------

static CharType GetCharType(const int ch)
{
    if (isalpha(ch))
        return CharType::ALPHA;

    if ('0' <= ch && ch <= '9')
        return CharType::DIGIT;

    switch (ch)
    {
        case '+':
        case '-':
        case '*':
        case '/':
        case '^':
        case '(':
        case ')':
        case '\n':
        case ',':
            return CharType::OPT;
        case '$':
        case '#':
        case '=':
        case '.':
        case '?':
        case '@':
        case '_':
        case '~':
        case ':':
        case '&':
        case '!':
        case '|':
        case '<':
        case '>':
            return CharType::ADDITIONAL;
        default:
            return CharType::UNK;
    }
}

//-----------------------------------------------------------------------------------------------------

void FillToken(token_t* token, const TokenType type, const TokenValue info, const size_t line)
{
    assert(token);

    token->type = type;
    token->info = info;
    token->line = line;
}

//-----------------------------------------------------------------------------------------------------

void DumpToken(FILE* fp, token_t* token)
{
    assert(token);

    fprintf(fp, "---------------\n");

    switch (token->type)
    {
        case TokenType::NUM:
            fprintf(fp, "TYPE > NUMBER\n"
                        "VAL  > %d\n"
                        "LINE > %d\n"
                        "---------------\n", token->info.val, token->line);
            return;
        case TokenType::TOKEN:
            fprintf(fp, "TYPE > TOKEN ");
            PrintOperator(fp, token->info.opt);
            fprintf(fp, "\nLINE > %d\n"
                        "---------------\n", token->line);
            return;
        case TokenType::NAME:
            fprintf(fp, "TYPE > NAME\n"
                        "ID   > %d\n"
                        "LINE > %d\n"
                        "---------------\n", token->info.name_id, token->line);
            return;
        case TokenType::FUNC_NAME:
            fprintf(fp, "TYPE > FUNC NAME\n"
                        "ID   > %d\n"
                        "LINE > %d\n"
                        "---------------\n", token->info.name_id, token->line);
            return;
        case TokenType::VAR_NAME:
            fprintf(fp, "TYPE > VAR NAME\n"
                        "ID   > %d\n"
                        "LINE > %d\n"
                        "---------------\n", token->info.name_id, token->line);
            return;
        default:
            fprintf(fp, "POISONED TOKEN\n"
                        "---------------\n");
            return;
    }
}

//-----------------------------------------------------------------------------------------------------

void TokensStorageCtor(Tokens* storage)
{
    assert(storage);

    token_t* tokens = (token_t*) calloc(DEFAULT_TOKENS_AMT, sizeof(token_t));

    StackCtor(&(storage->names_stk));

    nametable_t* global_table = MakeNametable();
    GlobalNametableCtor(global_table);

    StackPush(&(storage->names_stk), global_table);

    assert(tokens);

    storage->array = tokens;
    storage->size   = 0;

    GlobalNametableCtor(&storage->all_names);
}

//-----------------------------------------------------------------------------------------------------

void TokensStorageDtor(Tokens* storage)
{
    assert(storage);

    StackDtor(&(storage->names_stk));

    free(storage->array);

    storage->size   = 0;

    NametableDtor(&storage->all_names);
}

//-----------------------------------------------------------------------------------------------------

void DumpTokensStorage(FILE* fp, Tokens* storage)
{
    assert(storage);

    for (int i = 0; i < storage->size; i++)
    {
        fprintf(fp, "[%d]{\n", i);
        DumpToken(fp, &storage->array[i]);
        fprintf(fp, "}\n");
    }

    DumpNametable(fp, &storage->all_names);
}


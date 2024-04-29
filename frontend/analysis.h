#ifndef __ANALYSIS_H_
#define __ANALYSIS_H_

#include <stdio.h>

#include "tree/tree.h"
#include "common/file_read.h"
#include "frontend.h"
#include "stack/stack.h"

static const size_t DEFAULT_TOKENS_AMT = 500;

// ======================================================================
// TOKEN
// ======================================================================

void FillToken(token_t* token, const TokenType type, const TokenValue info, const size_t line);

void DumpToken(FILE* fp, token_t* token);

// ======================================================================
// SYNTAX STORAGE
// ======================================================================

struct Tokens
{
    token_t*    array;
    size_t      size;

    nametable_t all_names;

    Stack_t     names_stk;
};

void TokensStorageCtor(Tokens* storage);
void TokensStorageDtor(Tokens* storage);

void DumpTokensStorage(FILE* fp, Tokens* storage);

FrontendErrors Tokenize(LinesStorage* text, Tokens* storage, error_t* error);

#endif

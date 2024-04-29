#ifndef __SYNTAX_PARSER_H_
#define __SYNTAX_PARSER_H_

#include "analysis.h"

struct ParserState
{
    Tokens* tokens;
    size_t  ptr;
};

void GetTreeFromTokens(Tokens* tokens, tree_t* tree, error_t* error);  // TODO make tokens const

#endif

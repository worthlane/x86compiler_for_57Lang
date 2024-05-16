#include <stdio.h>
#include <string.h>

#include "tree_input.h"

static const size_t MAX_STRING_LEN = 100;

static char CheckOpeningBracketInInput(LinesStorage* info);
static inline void DeleteClosingBracketFromWord(LinesStorage* info, char* read);
static void GetNodeInfo(tree_t* tree, char* read, NodeType* type, NodeValue* val);
static Node* ReadNewNode(LinesStorage* info, tree_t* tree, error_t* error);
static Node* NodesPrefixRead(LinesStorage* storage, tree_t* tree, error_t* error);

//-----------------------------------------------------------------------------------------------------

void TreePrefixRead(LinesStorage* storage, tree_t* tree, error_t* error)
{
    assert(storage);
    assert(tree);
    assert(error);

    tree->root = NodesPrefixRead(storage, tree, error);
}

//-----------------------------------------------------------------------------------------------------

static char CheckOpeningBracketInInput(LinesStorage* info)
{
    SkipBufSpaces(info);
    char opening_bracket_check = Bufgetc(info);
    SkipBufSpaces(info);

    return opening_bracket_check;
}

//-----------------------------------------------------------------------------------------------------

static inline void DeleteClosingBracketFromWord(LinesStorage* info, char* read)
{
    assert(read);

    size_t bracket_pos = strlen(NIL);
    if (read[bracket_pos] == ')')
    {
        read[bracket_pos] = '\0';
        Bufungetc(info);
    }
}

//-----------------------------------------------------------------------------------------------------

static void GetNodeInfo(tree_t* tree, char* read, NodeType* type, NodeValue* val)
{
    int len = strlen(read);

    Operators op = GetOperator(read);
    if (op != Operators::UNK)
    {
        *type = NodeType::OP;
        *val  = {.opt = op};
        return;
    }

    char* end = nullptr;
    double num = strtod(read, &end);
    if (!(num == 0 && end != read + len))
    {
        *type = NodeType::NUM;
        *val = {.val = num};
        return;
    }

    if (read[0] == '_' && read[len - 1] == '_')
    {
        read[len - 1] = '\0';
        char* word = read + 1;

        int id = InsertKeywordInTable(&tree->names, word);

        *type = NodeType::VAR;
        *val  = {.var = id};
        return;
    }
}

//-----------------------------------------------------------------------------------------------------

static Node* ReadNewNode(LinesStorage* info, tree_t* tree, error_t* error)
{
    NodeType type = NodeType::POISON;
    NodeValue val = {};

    char read[MAX_STRING_LEN] = "";
    BufScanfWord(info, read);

    if (!strncmp(read, NIL, MAX_STRING_LEN))
        return nullptr;

    GetNodeInfo(tree, read, &type, &val);

    if (type == NodeType::POISON)
    {
        error->code = (int) TreeErrors::INVALID_SYNTAX;
        PrintLog("UNKNOWN NODE TYPE<br>\n");
        return nullptr;
    }

    Node* node  = MakeNode(type, val);

    Node* left  = NodesPrefixRead(info, tree, error);
    Node* right = NodesPrefixRead(info, tree, error);

    ConnectNodes(node, left, right);

    SkipBufSpaces(info);

    return node;
}

//-----------------------------------------------------------------------------------------------------

static Node* NodesPrefixRead(LinesStorage* storage, tree_t* tree, error_t* error)
{
    assert(storage);
    assert(tree);
    assert(error);

    char opening_bracket_check = CheckOpeningBracketInInput(storage);

    if (opening_bracket_check == '(')
    {
        Node* new_node = ReadNewNode(storage, tree, error);

        SkipBufSpaces(storage);

        char closing_bracket_check = Bufgetc(storage);
        if (closing_bracket_check != ')')
        {
            error->code = (int) TreeErrors::INVALID_SYNTAX;
            return nullptr;
        }

        return new_node;
    }
    else
    {
        Bufungetc(storage);

        char read[MAX_STRING_LEN] = "";
        BufScanfWord(storage, read);

        DeleteClosingBracketFromWord(storage, read);

        if (strncmp(read, "nil", MAX_STRING_LEN))
            error->code = (int) TreeErrors::INVALID_SYNTAX;
    }

    return nullptr;
}

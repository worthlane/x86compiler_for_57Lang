#include <stdio.h>
#include <string.h>

#include "common/logs.h"
#include "common/input_and_output.h"
#include "common/file_read.h"
#include "tree/tree.h"
#include "tree/tree_output.h"
#include "frontend/analysis.h"
#include "frontend/syntax_parser.h"
#include "frontend/frontend.h"
#include "stack/stack.h"

int main(const int argc, const char* argv[])
{
    OpenLogFile(argc, argv);

    error_t error = {};
    tree_t tree = {};

    const char* data_file = GetFileName(argc, argv, 1, "INPUT", &error);
    EXIT_IF_ERROR(&error);
    FILE* fp = OpenInputFile(data_file, &error);
    EXIT_IF_ERROR(&error);

    FILE* out_stream = OpenFile(TREE_FILE, "w", &error);
    EXIT_IF_ERROR(&error);

    LinesStorage info = {};
    CreateTextStorage(&info, &error, data_file);

    Tokens storage = {};
    TokensStorageCtor(&storage);

    Tokenize(&info, &storage, &error);
    EXIT_IF_FRONTEND_ERROR;

    TreeCtor(&tree, &error);

    GetTreeFromTokens(&storage, &tree, &error);
    EXIT_IF_FRONTEND_ERROR;

    PrintSignature(out_stream, data_file);
    PrintPrefixTree(out_stream, &tree);

    DUMP_TREE(&tree);

    TokensStorageDtor(&storage);
    DestructTextStorage(&info);
    TreeDtor(&tree);

    fclose(out_stream);
    fclose(fp);
}

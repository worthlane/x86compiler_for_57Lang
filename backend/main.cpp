#include <stdio.h>

#include "common/errors.h"
#include "tree/tree.h"
#include "tree/tree_input.h"
#include "tree/tree_output.h"
#include "common/file_read.h"
#include "common/input_and_output.h"
#include "backend.h"
#include "ir.h"

static const size_t IR_SIZE = 1012;

static const char* BYTES_FILE = "spu/assets/asm_code.txt";

int main(const int argc, const char* argv[])
{
    OpenLogFile(argv[0]);

    error_t error = {};

    FILE* fp = OpenInputFile(TREE_FILE, &error);
    EXIT_IF_ERROR(&error);

    LinesStorage info = {};
    CreateTextStorage(&info, &error, TREE_FILE);

    tree_t tree = {};
    TreeCtor(&tree, &error);

    TreePrefixRead(&info, &tree, &error);
    EXIT_IF_TREE_ERROR(&error);

    DUMP_TREE(&tree);

    FILE* out = fopen(BYTES_FILE, "w");
    if (!out)
    {
        error.code = (int) ERRORS::OPEN_FILE;
        SetErrorData(&error, "CAN NOT OPEN FILE \"%s\"\n", BYTES_FILE);
    }
    EXIT_IF_ERROR(&error);

    ir_t* intermediate_rep = IRCtor(IR_SIZE, &error);
    EXIT_IF_ERROR(&error);

    //MoveTreeToIR(&tree, intermediate_rep, &error);

    TranslateToAsm(&tree, out, &error);

    fclose(out);
    TreeDtor(&tree);
    IRDtor(intermediate_rep);
}

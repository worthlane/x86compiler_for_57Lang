#include <stdio.h>

#include "common/errors.h"
#include "tree/tree.h"
#include "tree/tree_input.h"
#include "tree/tree_output.h"
#include "common/file_read.h"
#include "common/input_and_output.h"
#include "backend.h"

const char* BYTES_FILE = "spu/assets/asm_code.txt";

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

    TranslateToAsm(&tree, out, &error);
}

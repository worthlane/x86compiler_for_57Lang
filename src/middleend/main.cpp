#include <stdio.h>

#include "common/errors.h"
#include "tree/tree.h"
#include "tree/tree_input.h"
#include "tree/tree_output.h"
#include "common/file_read.h"
#include "common/input_and_output.h"
#include "common/logs.h"
#include "middleend.h"

int main(const int argc, const char* argv[])
{
    OpenLogFile(argc, argv);

    error_t error = {};

    /*FILE* fp = OpenInputFile(TREE_FILE, &error);
    EXIT_IF_ERROR(&error);*/

    //const char* sign = GetSignature(fp);
    //printf("%s\n", sign);

    LinesStorage info = {};
    CreateTextStorage(&info, &error, TREE_FILE);

    tree_t tree = {};
    TreeCtor(&tree, &error);

    char signature[MAX_SIGNATURE_LEN] = "";
    BufScanfWord(&info, signature);

    TreePrefixRead(&info, &tree, &error);
    EXIT_IF_TREE_ERROR(&error);
    DUMP_TREE(&tree);

    OptimizeTree(&tree, &error);
    DUMP_TREE(&tree);

    FILE* out_stream = fopen(TREE_FILE, "w");

    fprintf(out_stream, "%s ", signature);
    PrintPrefixTree(out_stream, &tree);

    fclose(out_stream);
}

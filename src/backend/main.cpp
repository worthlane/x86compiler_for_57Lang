#include <stdio.h>
#include <string.h>

#include "common/errors.h"
#include "tree/tree.h"
#include "tree/tree_input.h"
#include "tree/tree_output.h"
#include "common/file_read.h"
#include "common/input_and_output.h"
#include "backend.h"
#include "compiler/ir.h"
#include "compiler/x86-64/x86_asm.h"
#include "compiler/x86-64/x86_elf.h"

static const size_t IR_SIZE = 1012;

static const char*  DUMP_FLAG    = "-s";
static const size_t MAX_FLAG_LEN = 10;

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

    char signature[MAX_SIGNATURE_LEN] = "";
    BufScanfWord(&info, signature);

    TreePrefixRead(&info, &tree, &error);
    EXIT_IF_TREE_ERROR(&error);

    DUMP_TREE(&tree);

    nametable_t* labels = MakeNametable();

    FillLabelsTable(&tree, labels, &error);

    ir_t* intermediate_rep = IRCtor(IR_SIZE, &error);
    EXIT_IF_ERROR(&error);

    MoveTreeToIR(&tree, intermediate_rep, labels, &error);

    char asm_file[MAX_SIGNATURE_LEN] = "";
    sprintf(asm_file, "%s.s", signature);

    bool need_dump = false;
    if (argc > 1)
        need_dump = !strncmp(DUMP_FLAG, argv[1], MAX_FLAG_LEN);

    TranslateIrToX86(asm_file, intermediate_rep, need_dump, &error);
    EXIT_IF_ERROR(&error);

    TranslateIrToElf(signature, intermediate_rep, &error);
    EXIT_IF_ERROR(&error);

    TreeDtor(&tree);
    IRDtor(intermediate_rep);
}

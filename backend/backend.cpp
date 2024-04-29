#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "backend.h"
#include "stack/stack.h"

#ifdef  NODE_NAME
#undef  NODE_NAME
#endif
#define NODE_NAME(token) (tree->names.list[token->value.var].name)

#ifdef  LOCAL_TABLE
#undef  LOCAL_TABLE
#endif
#define LOCAL_TABLE(stk) (stk->data[stk->size - 1])

#define LOTS_TABS "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"

static const int NO_RAM = -1;

static const int TABS_AMT = 7;

static int GetNameRamIdFromStack(const Stack_t* stk, const char* name);
static int GetNameRamIdFromTable(const nametable_t* nametable, const char* name);

static void TranslateNodeToAsm(const tree_t* tree, const Node* node, Stack_t* tables, FILE* out_stream, error_t* error);

static void TranslateCompareToAsm(const tree_t* tree, const Node* node, Stack_t* tables, int* label_spot,
                                  const char* comparator, FILE* out_stream, error_t* error);

static void TranslateAndToAsm(const tree_t* tree, const Node* node, Stack_t* tables,
                              int* label_spot, FILE* out_stream, error_t* error);
static void TranslateOrToAsm(const tree_t* tree, const Node* node, Stack_t* tables,
                              int* label_spot, FILE* out_stream, error_t* error);

static void TranslateAssignToAsm(const tree_t* tree, const Node* node, Stack_t* tables,
                                 int* ram_spot, FILE* out_stream, error_t* error);

static void GetParams(const tree_t* tree, const Node* node, Stack_t* tables, int* ram_spot, FILE* fp);

static inline int PrintWithTabs(FILE* fp, const size_t tabs_amt, const char *format, ...)
{
    for (size_t i = 0; i < tabs_amt; i++)
    {
        fprintf(fp, "\t");
    }

    va_list arg;
    int done;

    va_start (arg, format);
    done = vfprintf(fp, format, arg);
    va_end (arg);

    return done;
}

//-----------------------------------------------------------------------------------------------------

void TranslateToAsm(const tree_t* tree, FILE* out_stream, error_t* error)
{
    assert(tree);

    Stack_t tables = {};
    StackCtor(&tables);

    nametable_t* global = MakeNametable();
    StackPush(&tables, global);

    fprintf(out_stream, "jmp :_0_\n");

    TranslateNodeToAsm(tree, tree->root, &tables, out_stream, error);

    fprintf(out_stream, "hlt\n");

    StackDtor(&tables);
}

//-----------------------------------------------------------------------------------------------------

#define DEF_OP(name, is_char, symb, action, is_calc, asm)   \
                case Operators::name:                       \
                    asm

static void TranslateNodeToAsm(const tree_t* tree, const Node* node, Stack_t* tables, FILE* out_stream, error_t* error)
{
    assert(tree);

    static int ram_spot   = 0;
    static int label_spot = 0;

    if (node == nullptr)
        return;

    if (node->type == NodeType::NUM)
    {
        PrintWithTabs(out_stream, TABS_AMT, "push %d\n", (int) node->value.val);
        return;
    }

    if (node->type == NodeType::VAR)
    {
        int ram_id = GetNameRamIdFromStack(tables, NODE_NAME(node));
        if (ram_id == NO_RAM)
            error->code = (int) BackendErrors::NON_INIT_VAR;

        PrintWithTabs(out_stream, TABS_AMT, "push [%d]\n", ram_id);
        return;
    }

    assert(node->type == NodeType::OP);

    switch (node->value.opt)
    {
        #include "common/operations.h"

        case (Operators::NEW_FUNC):
        {
            TranslateNodeToAsm(tree, node->left, tables, out_stream, error);
            TranslateNodeToAsm(tree, node->right, tables, out_stream, error);
            break;
        }
        case (Operators::TYPE):
        {
            TranslateNodeToAsm(tree, node->right, tables, out_stream, error);
            break;
        }
        case (Operators::FUNC):
        {
            fprintf(out_stream, "\n:%s" LOTS_TABS "%% FUNC START\n", tree->names.list[node->left->value.var].name);

            nametable_t* local = MakeNametable();
            StackPush(tables, local);

            GetParams(tree, node->left->left, tables, &ram_spot, out_stream);

            TranslateNodeToAsm(tree, node->left->right, tables, out_stream, error);
            PrintWithTabs(out_stream, TABS_AMT, "ret" LOTS_TABS "%% FUNC END\n\n");

            StackPop(tables);

            break;
        }
        case (Operators::FUNC_CALL):
        {
            fprintf(out_stream, "\n" LOTS_TABS "%% START OF CALL\n");

            int name_amt = ((nametable_t*) LOCAL_TABLE(tables))->size;

            for (int i = 0; i < name_amt; i++)
            {
                int id = LOCAL_TABLE(tables)->list[i].ram_id;
                PrintWithTabs(out_stream, TABS_AMT, "push [%d]\n", id);
            }

            TranslateNodeToAsm(tree, node->left->left, tables, out_stream, error);
            PrintWithTabs(out_stream, TABS_AMT, "call :%s\n", tree->names.list[node->left->value.var].name);

            PrintWithTabs(out_stream, TABS_AMT, "pop rax\n");

            for (int i = 0; i < name_amt; i++)
            {
                int id = LOCAL_TABLE(tables)->list[i].ram_id;
                PrintWithTabs(out_stream, TABS_AMT, "pop [%d]\n", id);
            }

            PrintWithTabs(out_stream, TABS_AMT, "push rax\n" LOTS_TABS "%% END OF CALL\n");
            break;
        }
    }
}

#undef DEF_OP

//-----------------------------------------------------------------------------------------------------

static void GetParams(const tree_t* tree, const Node* node, Stack_t* tables, int* ram_spot, FILE* fp)
{
    assert(tree);
    assert(tables);

    if (!node)
        return;

    if (node->type == NodeType::OP && node->value.opt == Operators::TYPE)
    {
        GetParams(tree, node->right, tables, ram_spot, fp);
        return;
    }

    if (node->type == NodeType::OP && node->value.opt == Operators::COMMA)
    {
        GetParams(tree, node->left, tables, ram_spot, fp);
        GetParams(tree, node->right, tables, ram_spot, fp);

        return;
    }

    if (node->type == NodeType::VAR)
    {
        int id = InsertNameInTable(LOCAL_TABLE(tables), NODE_NAME(node));
        int ram_id = (*ram_spot)++;

        LOCAL_TABLE(tables)->list[id].ram_id = ram_id;
        LOCAL_TABLE(tables)->list[id].is_arg = true;

        PrintWithTabs(fp, TABS_AMT, "pop [%d]\n", ram_id);
        return;
    }

    PrintLog("UNKNOWN BEHAVIOUR IN FUNCTION ARGUMENTS\n");
}

//-----------------------------------------------------------------------------------------------------

static int GetNameRamIdFromTable(const nametable_t* nametable, const char* name)
{
    assert(nametable);
    assert(name);

    for (int i = 0; i < nametable->size; i++)
    {
        if (!strncmp(name, nametable->list[i].name, MAX_NAME_LEN))
        {
            return nametable->list[i].ram_id;
        }
    }

    return NO_RAM;
}

//-----------------------------------------------------------------------------------------------------

static int GetNameRamIdFromStack(const Stack_t* stk, const char* name)
{
    assert(stk);
    assert(name);

    for (int i = 0; i < stk->size; i++)
    {
        int id = GetNameRamIdFromTable(stk->data[i], name);
        if (id != NO_RAM)
            return id;
    }

    return NO_RAM;
}

//-----------------------------------------------------------------------------------------------------

static void TranslateCompareToAsm(const tree_t* tree, const Node* node, Stack_t* tables, int* label_spot,
                                  const char* comparator, FILE* out_stream, error_t* error)
{
    assert(tables);
    assert(tree);
    assert(node);
    assert(label_spot);
    assert(comparator);

    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);
    TranslateNodeToAsm(tree, node->right, tables, out_stream, error);

    PrintWithTabs(out_stream, TABS_AMT, "%s :TRUE_%d" LOTS_TABS "%% COMPARISON\n", comparator, *label_spot);
    PrintWithTabs(out_stream, TABS_AMT, "push 0\n");
    PrintWithTabs(out_stream, TABS_AMT, "jmp :FALSE_%d\n", *label_spot);
    fprintf(out_stream, ":TRUE_%d\n", *label_spot);
    PrintWithTabs(out_stream, TABS_AMT, "push 1\n");
    fprintf(out_stream, ":FALSE_%d" LOTS_TABS "%% COMPARISON END\n", *label_spot);


    *label_spot++;
}

//-----------------------------------------------------------------------------------------------------

static void TranslateAndToAsm(const tree_t* tree, const Node* node, Stack_t* tables,
                              int* label_spot, FILE* out_stream, error_t* error)
{
    assert(tables);
    assert(tree);
    assert(node);
    assert(label_spot);

    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);
    TranslateNodeToAsm(tree, node->right, tables, out_stream, error);

    fprintf(out_stream, "\n");

    PrintWithTabs(out_stream, TABS_AMT, "mul" LOTS_TABS "%% AND OPERATION\n");
    PrintWithTabs(out_stream, TABS_AMT, "push 0\n");
    PrintWithTabs(out_stream, TABS_AMT, "je :AND_TRUE_%d\n", *label_spot);
    PrintWithTabs(out_stream, TABS_AMT, "push 1\n");
    PrintWithTabs(out_stream, TABS_AMT, "jmp :AND_FALSE_%d\n", *label_spot);
    fprintf(out_stream, ":AND_TRUE_%d\n", *label_spot);
    PrintWithTabs(out_stream, TABS_AMT, "push 0\n");
    fprintf(out_stream, ":AND_FALSE_%d" LOTS_TABS "%% AND OPERATION END\n\n", *label_spot);
}

//-----------------------------------------------------------------------------------------------------

static void TranslateOrToAsm(const tree_t* tree, const Node* node, Stack_t* tables,
                              int* label_spot, FILE* out_stream, error_t* error)
{
    assert(tables);
    assert(tree);
    assert(node);
    assert(label_spot);

    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);

    fprintf(out_stream, "\n");

    PrintWithTabs(out_stream, TABS_AMT, "push 0" LOTS_TABS "%% FIRST OR OPERAND\n");
    PrintWithTabs(out_stream, TABS_AMT, "je :FIRST_ZERO_%d\n", *label_spot);
    PrintWithTabs(out_stream, TABS_AMT, "push 1\n");
    PrintWithTabs(out_stream, TABS_AMT, "jmp :FIRST_NOT_ZERO_%d\n", *label_spot);
    fprintf(out_stream, ":FIRST_ZERO_%d\n", *label_spot);
    PrintWithTabs(out_stream, TABS_AMT, "push 0\n");
    fprintf(out_stream, ":FIRST_NOT_ZERO_%d" LOTS_TABS "%% END OF FIRST OR OPERAND\n\n", *label_spot);

    *label_spot++;

    TranslateNodeToAsm(tree, node->right, tables, out_stream, error);

    fprintf(out_stream, "\n");

    PrintWithTabs(out_stream, TABS_AMT, "push 0" LOTS_TABS "%% SECOND OR OPERAND\n");
    PrintWithTabs(out_stream, TABS_AMT, "je :SECOND_ZERO_%d", *label_spot);
    PrintWithTabs(out_stream, TABS_AMT, "push 1\n");
    PrintWithTabs(out_stream, TABS_AMT, "jmp :SECOND_NOT_ZERO_%d\n", *label_spot);
    fprintf(out_stream, ":SECOND_ZERO_%d\n", *label_spot);
    PrintWithTabs(out_stream, TABS_AMT, "push 0\n");
    fprintf(out_stream, ":SECOND_NOT_ZERO_%d" LOTS_TABS "%% END OF SECOND OR OPERAND\n\n", *label_spot);

    *label_spot++;

    fprintf(out_stream, "\n");

    PrintWithTabs(out_stream, TABS_AMT, "add" LOTS_TABS "%% OR OPERATION\n");
    PrintWithTabs(out_stream, TABS_AMT, "push 0\n");
    PrintWithTabs(out_stream, TABS_AMT, "je :OR_TRUE_%d\n", *label_spot);
    PrintWithTabs(out_stream, TABS_AMT, "push 1\n");
    PrintWithTabs(out_stream, TABS_AMT, "jmp :OR_FALSE_%d\n", *label_spot);
    fprintf(out_stream, ":OR_TRUE_%d\n", *label_spot);
    PrintWithTabs(out_stream, TABS_AMT, "push 0\n");
    fprintf(out_stream, ":OR_FALSE_%d" LOTS_TABS "%% END OF OR OPERATION\n\n", *label_spot);

    *label_spot++;

}

//-----------------------------------------------------------------------------------------------------

static void TranslateAssignToAsm(const tree_t* tree, const Node* node, Stack_t* tables,
                                 int* ram_spot, FILE* out_stream, error_t* error)
{
    assert(ram_spot);
    assert(tree);
    assert(node);
    assert(tables);
    assert(error);

    TranslateNodeToAsm(tree, node->right, tables, out_stream, error);

    int ram_id = GetNameRamIdFromStack(tables, NODE_NAME(node->left));

    if (ram_id == NO_RAM)
    {
        int id = InsertNameInTable(LOCAL_TABLE(tables), NODE_NAME(node->left));

        ram_id = *ram_spot;

        *ram_spot += 1;

        LOCAL_TABLE(tables)->list[id].ram_id = ram_id;
    }

    PrintWithTabs(out_stream, TABS_AMT, "pop [%d]\n\n", ram_id);
}

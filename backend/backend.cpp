#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "backend.h"
#include "stack/stack.h"
#include "ir.h"

#ifdef  NODE_NAME
#undef  NODE_NAME
#endif
#define NODE_NAME(token) (tree->names.list[token->value.var].name)

#ifdef  LOCAL_TABLE
#undef  LOCAL_TABLE
#endif
#define LOCAL_TABLE(stk) (stk->data[stk->size - 1])

static const int NO_RAM = 1337;

static const int INIT_RAM = -16;
static const int RAM_STEP = -16;

static const int ARG_RAM_STEP = 16;
static const int ARG_INIT_RAM = 16;

static int GetNameRamIdFromStack(const Stack_t* stk, const char* name);
static int GetNameRamIdFromTable(const nametable_t* nametable, const char* name);

// =====================================================

void MoveTreeToIR(const tree_t* tree, ir_t* ir, error_t* error)
{
    assert(tree);
    assert(ir);
    assert(error);

    Stack_t tables = {};
    StackCtor(&tables);

    nametable_t* global = MakeNametable();
    StackPush(&tables, global);

    fprintf(out_stream, "jmp :_0_\n");

    int ram_spot = INIT_RAM;

    TranslateNodeToIR(ir, tree, tree->root, &tables, &ram_spot, error);

    instruction_t hlt = {.code = InstructionCode::ID_HLT};
    IRInsert(ir, &hlt, error);

    StackDtor(&tables);


}

//-----------------------------------------------------------------------------------------------------

#define DEF_OP(name, is_char, symb, action, is_calc, asm)   \
                case Operators::name:                       \
                    asm

static void TranslateNodeToIR(ir_t* ir, const tree_t* tree, const Node* node,
                              Stack_t* tables, int* ram_spot, error_t* error)
{
    assert(tree);
    assert(ir);

    static int label_spot = 0;

    if (node == nullptr)
        return;

    if (node->type == NodeType::NUM)
    {
        instruction_t instr = {.code  = InstructionCode::ID_PUSH_XMM,
                               .type1 = ArgumentType::VALUE,
                               .arg1  = (int) node->value.val};

        IRInsert(ir, &instr, error);
        return;
    }

    if (node->type == NodeType::VAR)
    {
        int ram_id = GetNameRamIdFromStack(tables, NODE_NAME(node));
        if (ram_id == NO_RAM)
            error->code = (int) BackendErrors::NON_INIT_VAR;

        instruction_t push_ram = {  .code  = InstructionCode::ID_PUSH_XMM,
                                    .type1 = ArgumentType::RAM,
                                    .arg1  = (int) node->value.val};

        IRInsert(ir, &push_ram, error);
        return;
    }

    assert(node->type == NodeType::OP);

    switch (node->value.opt)
    {
        #include "common/operations.h"

        case (Operators::NEW_FUNC):
        {
            TranslateNodeToIR(ir, tree, node->left, tables, ram_spot, error);
            TranslateNodeToIR(ir, tree, node->right, tables, ram_spot, error);
            break;
        }
        case (Operators::TYPE):
        {
            TranslateNodeToIR(ir, tree, node->right, tables, ram_spot, error);
            break;
        }
        case (Operators::FUNC):
        {
            fprintf(out_stream, "\n:%s" LOTS_TABS "%% FUNC START\n", tree->names.list[node->left->value.var].name);

            instruction_t stk_frame_in  = { .code  = InstructionCode::ID_PUSH,
                                            .type1 = ArgumentType::REGISTER,
                                            .arg1  = RBP};
            IRInsert(ir, &stk_frame_in, error); // push rbp

            instruction_t rbp_upd       = { .code  = InstructionCode::ID_MOV,
                                            .type1 = ArgumentType::REGISTER,
                                            .arg1  = RBP,
                                            .type2 = ArgumentType::REGISTER,
                                            .arg2  = RSP};
            IRInsert(ir, &rbp_upd, error);     // mov rbp, rsp

            nametable_t* local = MakeNametable();
            StackPush(tables, local);
            int local_ram_spot = ARG_INIT_RAM;

            GetParams(ir, tree, node->left->left, tables, &local_ram_spot);

            local_ram_spot = INIT_RAM;
            TranslateNodeToIR(ir, tree, node->left->right, tables, &local_ram_spot, error);

            StackPop(tables);

            break;
        }
        case (Operators::FUNC_CALL):
        {
            int name_amt = ((nametable_t*) LOCAL_TABLE(tables))->size;

            /*for (int i = 0; i < name_amt; i++)
            {
                int id = LOCAL_TABLE(tables)->list[i].ram_id;

                instruction_t push_ram = {   .code  = InstructionCode::ID_PUSH_XMM,
                                             .type1 = ArgumentType::RAM,
                                             .arg1  = id};
                IRInsert(ir, &push_ram, error);
            }*/

            TranslateNodeToIR(ir, tree, node->left->left, tables, ram_spot, error);
            PrintWithTabs(out_stream, TABS_AMT, "call :%s\n", tree->names.list[node->left->value.var].name);

            // pop

            /*instruction_t pop_reg = {   .code  = InstructionCode::ID_POP_XMM,
                                        .type1 = ArgumentType::REGISTER,
                                        .arg1  = XMM0};
            IRInsert(ir, &pop_reg, error);*/

            /*for (int i = 0; i < name_amt; i++)
            {
                int id = LOCAL_TABLE(tables)->list[i].ram_id;

                instruction_t pop_ram = {   .code  = InstructionCode::ID_POP_XMM,
                                            .type1 = ArgumentType::RAM,
                                            .arg1  = id};
                IRInsert(ir, &pop_ram, error);
            }*/

            /*instruction_t push_reg = {  .code  = InstructionCode::ID_PUSH_XMM,
                                        .type1 = ArgumentType::REGISTER,
                                        .arg1  = XMM0};
            IRInsert(ir, &push_reg, error);*/
            break;
        }
    }
}

#undef DEF_OP

//-----------------------------------------------------------------------------------------------------

static void TranslateAssignToIR(ir_t* ir, const tree_t* tree, const Node* node,
                                Stack_t* tables, int* ram_spot, error_t* error)
{
    assert(ram_spot);
    assert(tree);
    assert(node);
    assert(tables);
    assert(error);

    TranslateNodeToIR(ir, tree, node->right, tables, ram_spot, error);

    int ram_id = GetNameRamIdFromStack(tables, NODE_NAME(node->left));

    if (ram_id == NO_RAM)
    {
        int id = InsertNameInTable(LOCAL_TABLE(tables), NODE_NAME(node->left));

        ram_id = *ram_spot;
        *ram_spot += RAM_STEP;

        LOCAL_TABLE(tables)->list[id].ram_id = ram_id;
    }

    instruction_t pop_ram = {   .code  = InstructionCode::ID_POP_XMM,
                                .type1 = ArgumentType::REGISTER,
                                .arg1  = XMM3};
    IRInsert(ir, &pop_ram, error);

    instruction_t mov_ram = {   .code  = InstructionCode::ID_MOV_XMM,
                                .type1 = ArgumentType::RAM,
                                .arg1  = ram_id,
                                .type1 = ArgumentType::REGISTER,
                                .arg1  = XMM3};
    IRInsert(ir, &mov_ram, error);
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

static void GetParams(ir_t* ir, const tree_t* tree, const Node* node, Stack_t* tables, int* ram_spot)
{
    assert(tree);
    assert(tables);

    if (!node)
        return;

    if (node->type == NodeType::OP && node->value.opt == Operators::TYPE)
    {
        GetParams(ir, tree, node->right, tables, ram_spot);
        return;
    }

    if (node->type == NodeType::OP && node->value.opt == Operators::COMMA)
    {
        GetParams(ir, tree, node->left, tables, ram_spot);
        GetParams(ir, tree, node->right, tables, ram_spot);

        return;
    }

    if (node->type == NodeType::VAR)
    {
        int id = InsertNameInTable(LOCAL_TABLE(tables), NODE_NAME(node));
        int ram_id = *ram_spot;
        *ram_spot += ARG_RAM_STEP;

        LOCAL_TABLE(tables)->list[id].ram_id = ram_id;
        LOCAL_TABLE(tables)->list[id].is_arg = true;

        instruction_t pop_ram = {   .code  = InstructionCode::ID_POP_XMM,
                                    .type1 = ArgumentType::RAM,
                                    .arg1  = ram_id};
        IRInsert(ir, &pop_ram, error);                                          // TODO ?
        return;
    }

    PrintLog("UNKNOWN BEHAVIOUR IN FUNCTION ARGUMENTS\n");
}

//-----------------------------------------------------------------------------------------------------

static void TranslateCompareToIR(ir_t* ir, const tree_t* tree, const Node* node, Stack_t* tables, int* label_spot, int* ram_spot,
                                  const InstructionCode comparator, error_t* error)
{
    assert(tables);
    assert(tree);
    assert(node);
    assert(label_spot);

    instruction_t pop_val1 = {.code = InstructionCode::ID_POP_XMM,
                              .type1 = ArgumentType::REGISTER,
                              .arg1  = XMM1};

    instruction_t pop_val2 = {.code = InstructionCode::ID_POP_XMM,
                              .type1 = ArgumentType::REGISTER,
                              .arg1  = XMM2};

    instruction_t cmp = {.code = InstructionCode::ID_CMP,
                         .type1 = ArgumentType::REGISTER,
                         .arg1  = XMM1,
                         .type2 = ArgumentType::REGISTER,
                         .arg2  = XMM2};

    instruction_t true_res = {  .code = InstructionCode::ID_PUSH_XMM,
                                .type1 = ArgumentType::NUM,
                                .arg1  = 1};

    instruction_t false_res = { .code = InstructionCode::ID_PUSH_XMM,
                                .type1 = ArgumentType::NUM,
                                .arg1  = 0};

    TranslateNodeToIR(ir, tree, node->left, tables, ram_spot, error);
    TranslateNodeToIR(ir, tree, node->right, tables, ram_spot, error);

    IRInsert(ir, &pop_val1, error);
    IRInsert(ir, &pop_val2, error);
    IRInsert(ir, &cmp, error);
    PrintWithTabs(out_stream, TABS_AMT, "%s :TRUE_%d" LOTS_TABS "%% COMPARISON\n", comparator, *label_spot);
    IRInsert(ir, &false_res, error);
    PrintWithTabs(out_stream, TABS_AMT, "jmp :FALSE_%d\n", *label_spot);
    fprintf(out_stream, ":TRUE_%d\n", *label_spot);
    IRInsert(ir, &true_res, error);
    fprintf(out_stream, ":FALSE_%d" LOTS_TABS "%% COMPARISON END\n", *label_spot);


    *label_spot++;
}

//-----------------------------------------------------------------------------------------------------

static void TranslateAndToIR(ir_t* ir, const tree_t* tree, const Node* node, Stack_t* tables,
                              int* label_spot, int* ram_spot, error_t* error)
{
    assert(tables);
    assert(tree);
    assert(node);
    assert(label_spot);

    instruction_t pop_val1 = {.code = InstructionCode::ID_POP_XMM,
                              .type1 = ArgumentType::REGISTER,
                              .arg1  = XMM1};

    instruction_t pop_val2 = {.code = InstructionCode::ID_POP_XMM,
                              .type1 = ArgumentType::REGISTER,
                              .arg1  = XMM2};

    instruction_t mul = {.code = InstructionCode::ID_MUL,
                         .type1 = ArgumentType::REGISTER,
                         .arg1  = XMM1,
                         .type2 = ArgumentType::REGISTER,
                         .arg2  = XMM2};

    instruction_t cmp = {.code = InstructionCode::ID_CMP,
                         .type1 = ArgumentType::REGISTER,
                         .arg1  = XMM1,
                         .type2 = ArgumentType::NUM,
                         .arg2  = 0};

    instruction_t true_res = {  .code = InstructionCode::ID_PUSH_XMM,
                                .type1 = ArgumentType::NUM,
                                .arg1  = 1};

    instruction_t false_res = { .code = InstructionCode::ID_PUSH_XMM,
                                .type1 = ArgumentType::NUM,
                                .arg1  = 0};

    TranslateNodeToIR(ir, tree, node->left, tables, ram_spot, error);
    TranslateNodeToIR(ir, tree, node->right, tables, ram_spot, error);

    IRInsert(ir, &pop_val1, error);
    IRInsert(ir, &pop_val2, error);
    IRInsert(ir, &mul, error);
    IRInsert(ir, &cmp, error);
    PrintWithTabs(out_stream, TABS_AMT, "je :AND_TRUE_%d\n", *label_spot);
    IRInsert(ir, &true_res, error);
    PrintWithTabs(out_stream, TABS_AMT, "jmp :AND_FALSE_%d\n", *label_spot);
    fprintf(out_stream, ":AND_TRUE_%d\n", *label_spot);
    IRInsert(ir, &false_res, error);
    fprintf(out_stream, ":AND_FALSE_%d" LOTS_TABS "%% AND OPERATION END\n\n", *label_spot);
}

//-----------------------------------------------------------------------------------------------------

static void TranslateOrToAsm(ir_t* ir, const tree_t* tree, const Node* node, Stack_t* tables,
                              int* label_spot, int* ram_spot, error_t* error)
{
    assert(tables);
    assert(tree);
    assert(node);
    assert(label_spot);

    instruction_t pop_op = {.code = InstructionCode::ID_POP_XMM,
                              .type1 = ArgumentType::REGISTER,
                              .arg1  = XMM1};

    instruction_t cmp_op = {.code = InstructionCode::ID_CMP,
                            .type1 = ArgumentType::REGISTER,
                            .arg1  = XMM1,
                            .type2 = ArgumentType::NUM,
                            .arg2  = 0};

    instruction_t true_res = {  .code = InstructionCode::ID_PUSH_XMM,
                                .type1 = ArgumentType::NUM,
                                .arg1  = 1};

    instruction_t false_res = { .code = InstructionCode::ID_PUSH_XMM,
                                .type1 = ArgumentType::NUM,
                                .arg1  = 0};

    TranslateNodeToIR(ir, tree, node->left, tables, ram_spot, error);

    IRInsert(ir, &pop_op, error);
    IRInsert(ir, &cmp_op, error);
    PrintWithTabs(out_stream, TABS_AMT, "je :FIRST_ZERO_%d\n", *label_spot);
    IRInsert(ir, &true_res, error);
    PrintWithTabs(out_stream, TABS_AMT, "jmp :FIRST_NOT_ZERO_%d\n", *label_spot);
    fprintf(out_stream, ":FIRST_ZERO_%d\n", *label_spot);
    IRInsert(ir, &false_res, error);
    fprintf(out_stream, ":FIRST_NOT_ZERO_%d" LOTS_TABS "%% END OF FIRST OR OPERAND\n\n", *label_spot);

    *label_spot++;

    TranslateNodeToIR(ir, tree, node->right, tables, ram_spot, error);

    IRInsert(ir, &pop_op, error);
    IRInsert(ir, &cmp_op, error);
    PrintWithTabs(out_stream, TABS_AMT, "je :SECOND_ZERO_%d", *label_spot);
    IRInsert(ir, &true_res, error);
    PrintWithTabs(out_stream, TABS_AMT, "jmp :SECOND_NOT_ZERO_%d\n", *label_spot);
    fprintf(out_stream, ":SECOND_ZERO_%d\n", *label_spot);
    IRInsert(ir, &false_res, error);
    fprintf(out_stream, ":SECOND_NOT_ZERO_%d" LOTS_TABS "%% END OF SECOND OR OPERAND\n\n", *label_spot);

    *label_spot++;

    instruction_t res1 = {.code = InstructionCode::ID_POP_XMM,
                              .type1 = ArgumentType::REGISTER,
                              .arg1  = XMM1};

    instruction_t res2 = {.code = InstructionCode::ID_POP_XMM,
                              .type1 = ArgumentType::REGISTER,
                              .arg1  = XMM2};

    instruction_t add = {.code = InstructionCode::ID_ADD,
                         .type1 = ArgumentType::REGISTER,
                         .arg1  = XMM1,
                         .type2 = ArgumentType::REGISTER,
                         .arg2  = XMM2};

    IRInsert(ir, &res1, error);
    IRInsert(ir, &res2, error);
    IRInsert(ir, &add, error);
    IRInsert(ir, &cmp_op, error);
    PrintWithTabs(out_stream, TABS_AMT, "je :OR_TRUE_%d\n", *label_spot);
    IRInsert(ir, &true_res, error);
    PrintWithTabs(out_stream, TABS_AMT, "jmp :OR_FALSE_%d\n", *label_spot);
    fprintf(out_stream, ":OR_TRUE_%d\n", *label_spot);
    IRInsert(ir, &false_res, error);
    fprintf(out_stream, ":OR_FALSE_%d" LOTS_TABS "%% END OF OR OPERATION\n\n", *label_spot);

    *label_spot++;

}





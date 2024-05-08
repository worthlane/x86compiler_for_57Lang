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

static const int INIT_RAM = -16;
static const int RAM_STEP = -16;

static const int ARG_RAM_STEP = 16;
static const int ARG_INIT_RAM = 24;

static int GetNameRamIdFromStack(const Stack_t* stk, const char* name);

static void TranslateNodeToIR(ir_t* ir, const tree_t* tree, const Node* node,
                              Stack_t* tables, nametable_t* labels, int* ram_spot, error_t* error);
static void TranslateAssignToIR(ir_t* ir, const tree_t* tree, const Node* node,
                                Stack_t* tables, nametable_t* labels, int* ram_spot, error_t* error);
static void GetParams(ir_t* ir, const tree_t* tree, const Node* node,
                      Stack_t* tables, nametable_t* labels, int* ram_spot);
static void TranslateCompareToIR(ir_t* ir, const tree_t* tree, const Node* node, Stack_t* tables,
                                nametable_t* labels, int* ram_spot,  const InstructionCode comparator, error_t* error);
static void TranslateAndToIR(ir_t* ir, const tree_t* tree, const Node* node, Stack_t* tables,
                            nametable_t* labels, int* ram_spot, error_t* error);
static void TranslateOrToIR(ir_t* ir, const tree_t* tree, const Node* node, Stack_t* tables,
                             nametable_t* labels, int* ram_spot, error_t* error);

// =====================================================

void MoveTreeToIR(const tree_t* tree, ir_t* ir, nametable_t* labels, error_t* error)
{
    assert(tree);
    assert(ir);
    assert(error);

    Stack_t tables = {};
    StackCtor(&tables);

    nametable_t* global = MakeNametable();
    StackPush(&tables, global);

    instruction_t call = {  .code  = InstructionCode::ID_CALL,
                            .need_patch = true,
                            .refer_to   = GetValueFromNameTable(labels, "_0_")};
    IRInsert(ir, &call, error);

    instruction_t hlt = {.code = InstructionCode::ID_HLT};
    IRInsert(ir, &hlt, error);

    int ram_spot = INIT_RAM;

    TranslateNodeToIR(ir, tree, tree->root, &tables, labels, &ram_spot, error);

    StackDtor(&tables);
}

//-----------------------------------------------------------------------------------------------------

#define DEF_OP(name, is_char, symb, action, is_calc, asm)   \
                case Operators::name:                       \
                    asm                                     \
                    break;

static void TranslateNodeToIR(ir_t* ir, const tree_t* tree, const Node* node,
                              Stack_t* tables, nametable_t* labels, int* ram_spot, error_t* error)
{
    assert(tree);
    assert(ir);

    if (node == nullptr)
    {
        return;
    }

    if (node->type == NodeType::NUM)
    {

        instruction_t instr = {.code  = InstructionCode::ID_PUSH_XMM,
                               .type1 = ArgumentType::NUM,
                               .arg1  = (int) node->value.val};

        IRInsert(ir, &instr, error);
        return;
    }

    if (node->type == NodeType::VAR)
    {
        int ram_id = GetNameRamIdFromStack(tables, NODE_NAME(node));

        instruction_t push_ram = {  .code  = InstructionCode::ID_PUSH_XMM,
                                    .type1 = ArgumentType::RAM,
                                    .arg1  = ram_id};

        IRInsert(ir, &push_ram, error);
        return;
    }

    assert(node->type == NodeType::OP);

    switch (node->value.opt)
    {
        #define DEF_CONSTS
        #include "common/operations.h"
        #undef DEF_CONSTS

        case (Operators::NEW_FUNC):
        {
            TranslateNodeToIR(ir, tree, node->left, tables, labels, ram_spot, error);
            TranslateNodeToIR(ir, tree, node->right, tables, labels, ram_spot, error);
            break;
        }
        case (Operators::TYPE):
        {
            TranslateNodeToIR(ir, tree, node->right, tables, labels, ram_spot, error);
            break;
        }
        case (Operators::FUNC):
        {
            InsertPairInTable(labels, NODE_NAME(node->left), ir->size); // func start label

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

            GetParams(ir, tree, node->left->left, tables, labels, &local_ram_spot);

            local_ram_spot = INIT_RAM;
            TranslateNodeToIR(ir, tree, node->left->right, tables, labels, &local_ram_spot, error);

            StackPop(tables);

            break;
        }
        case (Operators::FUNC_CALL):
        {
            instruction_t stk_save       = { .code  = InstructionCode::ID_MOV,
                                            .type1 = ArgumentType::REGISTER,
                                            .arg1  = RDI,
                                            .type2 = ArgumentType::REGISTER,
                                            .arg2  = RSP};
            IRInsert(ir, &stk_save, error);     // mov rdi, rsp

            TranslateNodeToIR(ir, tree, node->left->left, tables, labels, ram_spot, error);

            instruction_t rdi_save  = { .code  = InstructionCode::ID_PUSH,
                                            .type1 = ArgumentType::REGISTER,
                                            .arg1  = RDI};
            IRInsert(ir, &rdi_save, error); // push rdi

            instruction_t call = {  .code  = InstructionCode::ID_CALL,
                                    .need_patch = true,
                                    .refer_to   = GetValueFromNameTable(labels, NODE_NAME(node->left))};
            IRInsert(ir, &call, error);

            instruction_t ret_rdi  = { .code  = InstructionCode::ID_POP,
                                            .type1 = ArgumentType::REGISTER,
                                            .arg1  = RDI};
            IRInsert(ir, &ret_rdi, error); // pop rdi

            instruction_t pop_arg       = { .code  = InstructionCode::ID_MOV,
                                            .type1 = ArgumentType::REGISTER,
                                            .arg1  = RSP,
                                            .type2 = ArgumentType::REGISTER,
                                            .arg2  = RDI};
            IRInsert(ir, &pop_arg, error); // mov rsp, rdi

            instruction_t push_reg = {  .code  = InstructionCode::ID_PUSH_XMM,
                                        .type1 = ArgumentType::REGISTER,
                                        .arg1  = XMM0};     // returned value
            IRInsert(ir, &push_reg, error);
            break;
        }
    }
}

#undef DEF_OP

//-----------------------------------------------------------------------------------------------------

static void TranslateAssignToIR(ir_t* ir, const tree_t* tree, const Node* node,
                                Stack_t* tables, nametable_t* labels, int* ram_spot, error_t* error)
{
    assert(ram_spot);
    assert(tree);
    assert(node);
    assert(tables);
    assert(error);

    TranslateNodeToIR(ir, tree, node->right, tables, labels, ram_spot, error);

    int ram_id = GetNameRamIdFromStack(tables, NODE_NAME(node->left));

    if (ram_id != UNKNOWN_VAL)
    {
        instruction_t pop_ram = {   .code  = InstructionCode::ID_POP_XMM,
                                    .type1 = ArgumentType::RAM,
                                    .arg1  = ram_id};
        IRInsert(ir, &pop_ram, error);
    }
    if (ram_id == UNKNOWN_VAL)
    {
        InsertPairInTable(LOCAL_TABLE(tables), NODE_NAME(node->left), *ram_spot);

        ram_id = *ram_spot;
        *ram_spot += RAM_STEP;

        //LOCAL_TABLE(tables)->list[id].ram_id = ram_id;
    }
}

//-----------------------------------------------------------------------------------------------------

static int GetNameRamIdFromStack(const Stack_t* stk, const char* name)
{
    assert(stk);
    assert(name);

    for (int i = 0; i < stk->size; i++)
    {
        int id = GetValueFromNameTable(stk->data[i], name);
        if (id != UNKNOWN_VAL)
            return id;
    }

    return UNKNOWN_VAL;
}

//-----------------------------------------------------------------------------------------------------

static void GetParams(ir_t* ir, const tree_t* tree, const Node* node,
                      Stack_t* tables, nametable_t* labels, int* ram_spot)
{
    assert(tree);
    assert(tables);

    if (!node)
        return;

    if (node->type == NodeType::OP && node->value.opt == Operators::TYPE)
    {
        GetParams(ir, tree, node->right, tables, labels, ram_spot);
        return;
    }

    if (node->type == NodeType::OP && node->value.opt == Operators::COMMA)
    {
        GetParams(ir, tree, node->left, tables, labels, ram_spot);
        GetParams(ir, tree, node->right, tables, labels, ram_spot);

        return;
    }

    if (node->type == NodeType::VAR)
    {
        InsertPairInTable(LOCAL_TABLE(tables), NODE_NAME(node), *ram_spot);
        int ram_id = *ram_spot;
        *ram_spot += ARG_RAM_STEP;

        /*LOCAL_TABLE(tables)->list[id].ram_id = ram_id;
        LOCAL_TABLE(tables)->list[id].is_arg = true;*/

        /*instruction_t pop_ram = {   .code  = InstructionCode::ID_POP_XMM,
                                    .type1 = ArgumentType::RAM,
                                    .arg1  = ram_id};
        IRInsert(ir, &pop_ram, error);                                          // TODO ?*/
        return;
    }

    PrintLog("UNKNOWN BEHAVIOUR IN FUNCTION ARGUMENTS\n");
}

//-----------------------------------------------------------------------------------------------------

static void TranslateCompareToIR(ir_t* ir, const tree_t* tree, const Node* node, Stack_t* tables,
                                nametable_t* labels, int* ram_spot,  const InstructionCode comparator, error_t* error)
{
    assert(tables);
    assert(tree);
    assert(node);

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

    TranslateNodeToIR(ir, tree, node->left, tables, labels, ram_spot, error);
    TranslateNodeToIR(ir, tree, node->right, tables, labels, ram_spot, error);

    IRInsert(ir, &pop_val1, error);
    IRInsert(ir, &pop_val2, error);
    IRInsert(ir, &cmp, error);

    instruction_t jmp_true = {  .code       = comparator,
                                .need_patch = true,
                                .refer_to   = ir->size + 3};

    IRInsert(ir, &jmp_true, error); // comparator :TRUE

    IRInsert(ir, &false_res, error);

    instruction_t jmp_false = {  .code      = InstructionCode::ID_JMP,
                                .need_patch = true,
                                .refer_to   = ir->size + 2};

    IRInsert(ir, &jmp_false, error); // jmp :FALSE
    // :TRUE
    IRInsert(ir, &true_res, error);
    // :FALSE
}

//-----------------------------------------------------------------------------------------------------

static void TranslateAndToIR(ir_t* ir, const tree_t* tree, const Node* node, Stack_t* tables,
                            nametable_t* labels, int* ram_spot, error_t* error)
{
    assert(tables);
    assert(tree);
    assert(node);

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

    TranslateNodeToIR(ir, tree, node->left, tables, labels, ram_spot, error);
    TranslateNodeToIR(ir, tree, node->right, tables, labels, ram_spot, error);

    IRInsert(ir, &pop_val1, error);
    IRInsert(ir, &pop_val2, error);
    IRInsert(ir, &mul, error);
    IRInsert(ir, &cmp, error);

    instruction_t jmp_false = { .code = InstructionCode::ID_JE,
                                .need_patch = true,
                                .refer_to   = ir->size + 3};

    IRInsert(ir, &jmp_false, error); // je :AND_FALSE
    IRInsert(ir, &true_res, error);

    instruction_t jmp_true = {  .code = InstructionCode::ID_JMP,
                                .need_patch = true,
                                .refer_to   = ir->size + 2};

    IRInsert(ir, &jmp_true, error); // jmp :AND_TRUE
    // :AND_FALSE
    IRInsert(ir, &false_res, error);
    // :AND_TRUE
}

//-----------------------------------------------------------------------------------------------------

static void TranslateOrToIR(ir_t* ir, const tree_t* tree, const Node* node, Stack_t* tables,
                             nametable_t* labels, int* ram_spot, error_t* error)
{
    assert(tables);
    assert(tree);
    assert(node);

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

    TranslateNodeToIR(ir, tree, node->left, tables, labels, ram_spot, error);

    IRInsert(ir, &pop_op, error);
    IRInsert(ir, &cmp_op, error);

    instruction_t jmp_zero = {  .code = InstructionCode::ID_JE,
                                .need_patch = true,
                                .refer_to   = ir->size + 3};
    IRInsert(ir, &jmp_zero, error); // je :FIRST_ZERO
    IRInsert(ir, &true_res, error);
    instruction_t jmp_not_zero = {  .code = InstructionCode::ID_JMP,
                                    .need_patch = true,
                                    .refer_to   = ir->size + 2};
    IRInsert(ir, &jmp_not_zero, error); // jmp :FIRST__NOT_ZERO
    // :FIRST_ZERO
    IRInsert(ir, &false_res, error);
    // :FIRST_NOT_ZERO

    TranslateNodeToIR(ir, tree, node->right, tables, labels, ram_spot, error);

    IRInsert(ir, &pop_op, error);
    IRInsert(ir, &cmp_op, error);

    jmp_zero.refer_to = ir->size + 3;
    IRInsert(ir, &jmp_zero, error); // je :SECOND_ZERO
    IRInsert(ir, &true_res, error);

    jmp_not_zero.refer_to = ir->size + 2;
    IRInsert(ir, &jmp_not_zero, error); // jmp :SECOND__NOT_ZERO
    // :SECOND_ZERO
    IRInsert(ir, &false_res, error);
    // :SECOND_NOT_ZERO

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

    instruction_t or_false = {  .code = InstructionCode::ID_JE,
                                .need_patch = true,
                                .refer_to   = ir->size + 3};
    IRInsert(ir, &or_false, error); // je :OR_FALSE

    IRInsert(ir, &true_res, error);

    instruction_t or_true  = {  .code = InstructionCode::ID_JMP,
                                .need_patch = true,
                                .refer_to   = ir->size + 2};
    IRInsert(ir, &or_true, error); // jmp :OR_TRUE
    // :OR_FALSE
    IRInsert(ir, &false_res, error);
    // :OR_TRUE
}

// -----------------------------------------------------------

void FillLabelsTable(const tree_t* tree, nametable_t* labels, error_t* error)
{
    assert(tree);
    assert(error);

    ir_t* ir = IRCtor(FAKE_IR_CAP, error);

    Stack_t tables = {};
    StackCtor(&tables);

    nametable_t* global = MakeNametable();
    StackPush(&tables, global);

    ir->size += 2; // call :_0_, hlt

    int ram_spot = INIT_RAM;

    TranslateNodeToIR(ir, tree, tree->root, &tables, labels, &ram_spot, error);

    IRDtor(ir);
    StackDtor(&tables);
}




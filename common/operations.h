#ifndef DEF_OP
#define DEF_OP(...) ;
#endif

// ======================================================================
// FORMAT
// ======================================================================

// DEF_OP(NAME, IS_CHAR, SYMBOLS, ACTION, IS_CALC, ASM)

// ======================================================================
// OPERATIONS
// ======================================================================

DEF_OP(ADD, true, "+", (NUMBER_1 + NUMBER_2), true,
{
    TranslateNodeToIR(ir, tree, node->left, tables, ram_spot, error);
    TranslateNodeToIR(ir, tree, node->right, tables, ram_spot, error);

    instruction_t pop_val1 = {.code = InstructionCode::ID_POP_XMM,
                              .type1 = ArgumentType::REGISTER,
                              .arg1  = XMM1};

    instruction_t pop_val2 = {.code = InstructionCode::ID_POP_XMM,
                              .type1 = ArgumentType::REGISTER,
                              .arg1  = XMM2};

    instruction_t add = {.code = InstructionCode::ID_ADD,
                         .type1 = ArgumentType::REGISTER,
                         .arg1  = XMM1,
                         .type2 = ArgumentType::REGISTER,
                         .arg2  = XMM2};

    instruction_t push = {  .code = InstructionCode::ID_PUSH_XMM,
                            .type1 = ArgumentType::REGISTER,
                            .arg1  = XMM1};

    IRInsert(ir, &pop_val1, error);
    IRInsert(ir, &pop_val2, error);
    IRInsert(ir, &add, error);
    IRInsert(ir, &push, error);

    PrintWithTabs(out_stream, TABS_AMT, "add\n");
    break;
})

DEF_OP(SUB, true, "-", (NUMBER_1 - NUMBER_2), true,
{
    TranslateNodeToIR(ir, tree, node->left, tables, ram_spot, error);
    TranslateNodeToIR(ir, tree, node->right, tables, ram_spot, error);

    instruction_t pop_val1 = {.code = InstructionCode::ID_POP_XMM,
                              .type1 = ArgumentType::REGISTER,
                              .arg1  = XMM1};

    instruction_t pop_val2 = {.code = InstructionCode::ID_POP_XMM,
                              .type1 = ArgumentType::REGISTER,
                              .arg1  = XMM2};

    instruction_t sub = {.code = InstructionCode::ID_SUB,
                         .type1 = ArgumentType::REGISTER,
                         .arg1  = XMM1,
                         .type2 = ArgumentType::REGISTER,
                         .arg2  = XMM2};

    instruction_t push = {  .code = InstructionCode::ID_PUSH_XMM,
                            .type1 = ArgumentType::REGISTER,
                            .arg1  = XMM1};

    IRInsert(ir, &pop_val1, error);
    IRInsert(ir, &pop_val2, error);
    IRInsert(ir, &sub, error);
    IRInsert(ir, &push, error);

    break;
})

DEF_OP(MUL, true, "*", (NUMBER_1 * NUMBER_2), true,
{
    TranslateNodeToIR(ir, tree, node->left, tables, ram_spot, error);
    TranslateNodeToIR(ir, tree, node->right, tables, ram_spot, error);

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

    instruction_t push = {  .code = InstructionCode::ID_PUSH_XMM,
                            .type1 = ArgumentType::REGISTER,
                            .arg1  = XMM1};

    IRInsert(ir, &pop_val1, error);
    IRInsert(ir, &pop_val2, error);
    IRInsert(ir, &mul, error);
    IRInsert(ir, &push, error);

    break;
})

DEF_OP(DIV, true, "/", (NUMBER_1 / NUMBER_2), true,
{
    TranslateNodeToIR(ir, tree, node->left, tables, ram_spot, error);
    TranslateNodeToIR(ir, tree, node->right, tables, ram_spot, error);

    instruction_t pop_val1 = {.code = InstructionCode::ID_POP_XMM,
                              .type1 = ArgumentType::REGISTER,
                              .arg1  = XMM1};

    instruction_t pop_val2 = {.code = InstructionCode::ID_POP_XMM,
                              .type1 = ArgumentType::REGISTER,
                              .arg1  = XMM2};

    instruction_t div = {.code = InstructionCode::ID_DIV,
                         .type1 = ArgumentType::REGISTER,
                         .arg1  = XMM1,
                         .type2 = ArgumentType::REGISTER,
                         .arg2  = XMM2};

    instruction_t push = {  .code = InstructionCode::ID_PUSH_XMM,
                            .type1 = ArgumentType::REGISTER,
                            .arg1  = XMM1};

    IRInsert(ir, &pop_val1, error);
    IRInsert(ir, &pop_val2, error);
    IRInsert(ir, &div, error);
    IRInsert(ir, &push, error);

    break;
})

DEF_OP(DEG, true, "^", ( pow(NUMBER_1, NUMBER_2) ), true,
{
    assert(false && "THIS COMMANT DOES NOT SUPPORT NOW");
})

DEF_OP(ASSIGN, false, ":=", (0), false,
{
    TranslateAssignToIR(ir, tree, node, tables, ram_spot, error);
    break;
})

DEF_OP(SIN, false, "$1#", ( sin( NUMBER_1 )), true,
{
    assert(false && "THIS COMMANT DOES NOT SUPPORT NOW");
})

DEF_OP(SQRT, false, "57#", ( sqrt( NUMBER_1 )), true,
{
    assert(false && "THIS COMMANT DOES NOT SUPPORT NOW");
})

DEF_OP(COS, false, "<0$", ( cos( NUMBER_1 )), true,
{
    assert(false && "THIS COMMANT DOES NOT SUPPORT NOW");
})

DEF_OP(IF, false, "57?", (0), false,
{
    int free_label = label_spot++;

    instruction_t pop_op = {.code = InstructionCode::ID_POP_XMM,
                              .type1 = ArgumentType::REGISTER,
                              .arg1  = XMM1};

    instruction_t cmp_op = {.code = InstructionCode::ID_CMP,
                            .type1 = ArgumentType::REGISTER,
                            .arg1  = XMM1,
                            .type2 = ArgumentType::NUM,
                            .arg2  = 0};

    TranslateNodeToIR(ir, tree, node->left, tables, ram_spot, error);

    IRInsert(ir, &pop_op, error);
    IRInsert(ir, &cmp_op, error);

    PrintWithTabs(out_stream, TABS_AMT, "je :FALSE_COND_%d\n", (int) free_label);

    nametable_t* local = MakeNametable();
    StackPush(tables, local);

    TranslateNodeToIR(ir, tree, node->right, tables, ram_spot, error);

    StackPop(tables);

    fprintf(out_stream, ":FALSE_COND_%d" LOTS_TABS "%% IF END\n\n", (int) free_label);

    break;
})

DEF_OP(WHILE, false, "1000_7", (0), false,
{
    int free_label_cycle = label_spot++;
    int free_label_cond  = label_spot++;

    instruction_t pop_op = {.code = InstructionCode::ID_POP_XMM,
                              .type1 = ArgumentType::REGISTER,
                              .arg1  = XMM1};

    instruction_t cmp_op = {.code = InstructionCode::ID_CMP,
                            .type1 = ArgumentType::REGISTER,
                            .arg1  = XMM1,
                            .type2 = ArgumentType::NUM,
                            .arg2  = 0};

    fprintf(out_stream, "\n:WHILE_CYCLE_%d" LOTS_TABS "%% WHILE START\n", free_label_cycle);

    TranslateNodeToIR(ir, tree, node->left, tables, ram_spot, error);

    IRInsert(ir, &pop_op, error);
    IRInsert(ir, &cmp_op, error);
    PrintWithTabs(out_stream, TABS_AMT, "je :QUIT_CYCLE_%d\n", free_label_cond);

    nametable_t* local = MakeNametable();
    StackPush(tables, local);

    TranslateNodeToIR(ir, tree, node->right, tables, ram_spot, error);

    StackPop(tables);

    PrintWithTabs(out_stream, TABS_AMT, "jmp :WHILE_CYCLE_%d\n", free_label_cycle);
    fprintf(out_stream, ":QUIT_CYCLE_%d" LOTS_TABS "%% WHILE END\n\n", free_label_cond);

    break;
})

DEF_OP(GREATER, false,">", (NUMBER_1 > NUMBER_2), true,
{
    TranslateCompareToIR(ir, tree, node, tables, &label_spot, ram_spot, ID_JB, error);
    break;
})
DEF_OP(GREATER_EQ, false, ">=", (NUMBER_1 >= NUMBER_2), true,
{
    TranslateCompareToIR(ir, tree, node, tables, &label_spot, ram_spot, ID_JBE, error);
    break;
})
DEF_OP(LESS, false, "<", (NUMBER_1 < NUMBER_2), true,
{
    TranslateCompareToIR(ir, tree, node, tables, &label_spot, ram_spot, ID_JA, error);
    break;
})
DEF_OP(LESS_EQ, false, "<=", (NUMBER_1 <= NUMBER_2), true,
{
    TranslateCompareToIR(ir, tree, node, tables, &label_spot, ram_spot, ID_JAE, error);
    break;
})
DEF_OP(EQ,false, "==", (NUMBER_1 == NUMBER_2), true,
{
    TranslateCompareToIR(ir, tree, node, tables, &label_spot, ram_spot, ID_JE, error);
    break;
})
DEF_OP(NOT_EQ, false, "!=", (NUMBER_1 != NUMBER_2), true,
{
    TranslateCompareToIR(ir, tree, node, tables, &label_spot, ram_spot, ID_JNE, error);
    break;
})
DEF_OP(AND, false, "&&", (NUMBER_1 && NUMBER_2), true,
{
    TranslateAndToIR(ir, tree, node, tables, &label_spot, ram_spot, error);
    break;
})
DEF_OP(OR, false, "||", (NUMBER_1 || NUMBER_2), true,
{
    TranslateOrToIR(ir, tree, node, tables, &label_spot, ram_spot, error);
    break;
})

DEF_OP(READ, false, "57>>", (0), false,
{
    instruction_t in = {.code = InstructionCode::ID_IN};
    IRInsert(ir, &in, error);

    int ram_id = GetNameRamIdFromStack(tables, NODE_NAME(node->left));

    instruction_t pop_ram = {   .code  = InstructionCode::ID_POP_XMM,
                                .type1 = ArgumentType::RAM,
                                .arg1  = ram_id};
    IRInsert(ir, &pop_ram, error);
    break;
})

DEF_OP(PRINT, false, "57<<", (0), false,
{
    TranslateNodeToIR(ir, tree, node->left, tables, ram_spot, error);

    // что то с out сделать

    instruction_t out = {.code = InstructionCode::ID_OUT};
    IRInsert(ir, &out, error);
    break;
})

DEF_OP(TYPE_INT, false, "57::", (0), false, {})

DEF_OP(L_BRACKET, true, "(", (0), false, {})
DEF_OP(R_BRACKET, true, ")", (0), false, {})
DEF_OP(COMMA, true, ",", (0), false,
{
    TranslateNodeToIR(ir, tree, node->right, tables, ram_spot, error);      // TODO notice right -> left
    TranslateNodeToIR(ir, tree, node->left, tables, ram_spot, error);
    break;
})
DEF_OP(LINE_END, true, "\n", (0), false,
{
    TranslateNodeToIR(ir, tree, node->left, tables, ram_spot, error);
    TranslateNodeToIR(ir, tree, node->right, tables, ram_spot, error);
    break;
})
DEF_OP(BLOCK_END, false, ".57", (0), false, {})
DEF_OP(FUNC_WALL, false, "##################################################", (0), false, {})

DEF_OP(RETURN, false, "~57", (0), false,
{
    TranslateNodeToIR(ir, tree, node->left, tables, ram_spot, error)

    instruction_t stk_frame_ret = { .code  = InstructionCode::ID_POP,
                                    .type1 = ArgumentType::REGISTER,
                                    .arg1  = RBP};
    IRInsert(ir, &stk_frame_ret, error); // pop rbp

    instruction_t ret = {.code = InstructionCode::ID_RET};
    IRInsert(ir, &ret, error);
    break;
})

DEF_OP(END, false, "@57@", (0), false, {})





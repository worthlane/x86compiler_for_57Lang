#ifndef DEF_OP
#define DEF_OP(...) ;
#endif

// ======================================================================
// OPERATIONS
// ======================================================================

#ifdef DEF_CONSTS

static const instruction_t POP_VAL0 = { .code = InstructionCode::ID_POP_XMM,
                                        .type1 = ArgumentType::REGISTER,
                                        .arg1  = XMM0};

static const instruction_t PUSH_VAL0 = { .code = InstructionCode::ID_PUSH_XMM,
                                        .type1 = ArgumentType::REGISTER,
                                        .arg1  = XMM0};

static const instruction_t POP_VAL1 = { .code = InstructionCode::ID_POP_XMM,
                                        .type1 = ArgumentType::REGISTER,
                                        .arg1  = XMM1};

static const instruction_t POP_VAL2 = { .code = InstructionCode::ID_POP_XMM,
                                        .type1 = ArgumentType::REGISTER,
                                        .arg1  = XMM2};

static const instruction_t ADD_XMM = {  .code = InstructionCode::ID_ADD,
                                        .type1 = ArgumentType::REGISTER,
                                        .arg1  = XMM1,
                                        .type2 = ArgumentType::REGISTER,
                                        .arg2  = XMM2};

static const instruction_t SUB_XMM = {  .code = InstructionCode::ID_SUB,
                                        .type1 = ArgumentType::REGISTER,
                                        .arg1  = XMM1,
                                        .type2 = ArgumentType::REGISTER,
                                        .arg2  = XMM2};

static const instruction_t MUL_XMM = {  .code = InstructionCode::ID_MUL,
                                        .type1 = ArgumentType::REGISTER,
                                        .arg1  = XMM1,
                                        .type2 = ArgumentType::REGISTER,
                                        .arg2  = XMM2};

static const instruction_t DIV_XMM = {  .code = InstructionCode::ID_DIV,
                                        .type1 = ArgumentType::REGISTER,
                                        .arg1  = XMM1,
                                        .type2 = ArgumentType::REGISTER,
                                        .arg2  = XMM2};

static const instruction_t PUSH_XMM = { .code = InstructionCode::ID_PUSH_XMM,
                                        .type1 = ArgumentType::REGISTER,
                                        .arg1  = XMM1};

static const instruction_t RETURN = {.code = InstructionCode::ID_RET};

static const instruction_t STK_FRAME_RET = {    .code  = InstructionCode::ID_POP,
                                                .type1 = ArgumentType::REGISTER,
                                                .arg1  = RBP};

static const instruction_t OUT_INSTR = {.code = InstructionCode::ID_OUT};

static const instruction_t IN_INSTR = {.code = InstructionCode::ID_IN};

static const instruction_t POP_OP = {   .code = InstructionCode::ID_POP_XMM,
                                        .type1 = ArgumentType::REGISTER,
                                        .arg1  = XMM1};

static const instruction_t CMP_OP = {   .code = InstructionCode::ID_CMP,
                                        .type1 = ArgumentType::REGISTER,
                                        .arg1  = XMM1,
                                        .type2 = ArgumentType::NUM,
                                        .arg2  = 0};

static const instruction_t RSP_RET       = { .code  = InstructionCode::ID_MOV,
                                            .type1 = ArgumentType::REGISTER,
                                            .arg1  = RSP,
                                            .type2 = ArgumentType::REGISTER,
                                            .arg2  = RBP};

static const instruction_t RET_VAL = { .code = InstructionCode::ID_POP_XMM,
                                        .type1 = ArgumentType::REGISTER,
                                        .arg1  = XMM0};

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
    TranslateNodeToIR(ir, tree, node->left, tables, labels, ram_spot, error);
    TranslateNodeToIR(ir, tree, node->right, tables,  labels, ram_spot, error);

    IRInsert(ir, &POP_VAL1, error);
    IRInsert(ir, &POP_VAL2, error);
    IRInsert(ir, &ADD_XMM, error);
    IRInsert(ir, &PUSH_XMM, error);
    break;
})

DEF_OP(SUB, true, "-", (NUMBER_1 - NUMBER_2), true,
{
    TranslateNodeToIR(ir, tree, node->left, tables, labels, ram_spot, error);
    TranslateNodeToIR(ir, tree, node->right, tables, labels, ram_spot, error);

    IRInsert(ir, &POP_VAL1, error);
    IRInsert(ir, &POP_VAL2, error);
    IRInsert(ir, &SUB_XMM, error);
    IRInsert(ir, &PUSH_XMM, error);
    break;
})

DEF_OP(MUL, true, "*", (NUMBER_1 * NUMBER_2), true,
{
    TranslateNodeToIR(ir, tree, node->left, tables, labels, ram_spot, error);
    TranslateNodeToIR(ir, tree, node->right, tables, labels, ram_spot, error);

    IRInsert(ir, &POP_VAL1, error);
    IRInsert(ir, &POP_VAL2, error);
    IRInsert(ir, &MUL_XMM, error);
    IRInsert(ir, &PUSH_XMM, error);
    break;
})

DEF_OP(DIV, true, "/", (NUMBER_1 / NUMBER_2), true,
{
    TranslateNodeToIR(ir, tree, node->left, tables, labels, ram_spot, error);
    TranslateNodeToIR(ir, tree, node->right, tables, labels, ram_spot, error);

    IRInsert(ir, &POP_VAL1, error);
    IRInsert(ir, &POP_VAL2, error);
    IRInsert(ir, &DIV_XMM, error);
    IRInsert(ir, &PUSH_XMM, error);
    break;
})

DEF_OP(DEG, true, "^", ( pow(NUMBER_1, NUMBER_2) ), true,
{
    assert(false && "THIS COMMAND DOES NOT SUPPORT NOW");
})

DEF_OP(ASSIGN, false, ":=", (0), false,
{
    TranslateAssignToIR(ir, tree, node, tables, labels, ram_spot, error);
    break;
})

DEF_OP(SIN, false, "$1#", ( sin( NUMBER_1 )), true,
{
    assert(false && "THIS COMMAND DOES NOT SUPPORT NOW");
})

DEF_OP(SQRT, false, "57#", ( sqrt( NUMBER_1 )), true,
{
    assert(false && "THIS COMMAND DOES NOT SUPPORT NOW");
})

DEF_OP(COS, false, "<0$", ( cos( NUMBER_1 )), true,
{
    assert(false && "THIS COMMAND DOES NOT SUPPORT NOW");
})

DEF_OP(IF, false, "57?", (0), false,
{
    TranslateNodeToIR(ir, tree, node->left, tables, labels, ram_spot, error);

    IRInsert(ir, &POP_OP, error);
    IRInsert(ir, &CMP_OP, error);

    size_t jmp_false_pos = ir->size;
    instruction_t jmp_false = {};
    jmp_false.code       = InstructionCode::ID_JE;
    jmp_false.need_patch = true;
    jmp_false.refer_to   = 0;

    IRInsert(ir, &jmp_false, error); // je :FALSE_COND

    nametable_t* local = MakeNametable();
    StackPush(tables, local);
    TranslateNodeToIR(ir, tree, node->right, tables, labels, ram_spot, error);
    StackPop(tables);

    ir->array[jmp_false_pos].refer_to = ir->size;
    // :FALSE_COND

    break;
})

DEF_OP(WHILE, false, "1000_7", (0), false,
{
    size_t while_start = ir->size;
    // :WHILE START

    TranslateNodeToIR(ir, tree, node->left, tables, labels, ram_spot, error);

    IRInsert(ir, &POP_OP, error);
    IRInsert(ir, &CMP_OP, error);

    size_t break_jmp_pos = ir->size;

    instruction_t jmp_break = {};
    jmp_break.code       = InstructionCode::ID_JE;
    jmp_break.need_patch = true;
    jmp_break.refer_to   = 0;

    IRInsert(ir, &jmp_break, error); // je :QUIT_CYCLE

    nametable_t* local = MakeNametable();
    StackPush(tables, local);
    TranslateNodeToIR(ir, tree, node->right, tables, labels, ram_spot, error);
    StackPop(tables);

    instruction_t jmp_start = {};
    jmp_start.code       = InstructionCode::ID_JMP;
    jmp_start.need_patch = true;
    jmp_start.refer_to   = while_start;

    IRInsert(ir, &jmp_start, error); // jmp :WHILE_START

    ir->array[break_jmp_pos].refer_to = ir->size;
    // :QUIT_CYCLE

    break;
})

DEF_OP(GREATER, false,">", (NUMBER_1 > NUMBER_2), true,
{
    TranslateCompareToIR(ir, tree, node, tables, labels, ram_spot, InstructionCode::ID_JB, error);
    break;
})
DEF_OP(GREATER_EQ, false, ">=", (NUMBER_1 >= NUMBER_2), true,
{
    TranslateCompareToIR(ir, tree, node, tables, labels, ram_spot, InstructionCode::ID_JBE, error);
    break;
})
DEF_OP(LESS, false, "<", (NUMBER_1 < NUMBER_2), true,
{
    TranslateCompareToIR(ir, tree, node, tables, labels, ram_spot, InstructionCode::ID_JA, error);
    break;
})
DEF_OP(LESS_EQ, false, "<=", (NUMBER_1 <= NUMBER_2), true,
{
    TranslateCompareToIR(ir, tree, node, tables, labels, ram_spot, InstructionCode::ID_JAE, error);
    break;
})
DEF_OP(EQ,false, "==", (NUMBER_1 == NUMBER_2), true,
{
    TranslateCompareToIR(ir, tree, node, tables, labels, ram_spot, InstructionCode::ID_JE, error);
    break;
})
DEF_OP(NOT_EQ, false, "!=", (NUMBER_1 != NUMBER_2), true,
{
    TranslateCompareToIR(ir, tree, node, tables, labels, ram_spot, InstructionCode::ID_JNE, error);
    break;
})
DEF_OP(AND, false, "&&", (NUMBER_1 && NUMBER_2), true,
{
    TranslateAndToIR(ir, tree, node, tables, labels, ram_spot, error);
    break;
})
DEF_OP(OR, false, "||", (NUMBER_1 || NUMBER_2), true,
{
    TranslateOrToIR(ir, tree, node, tables, labels, ram_spot, error);
    break;
})

DEF_OP(READ, false, "57>>", (0), false,
{
    IRInsert(ir, &IN_INSTR, error);

    int ram_id = GetNameRamIdFromStack(tables, NODE_NAME(node->left));

    IRInsert(ir, &PUSH_VAL0, error);

    instruction_t pop_ram = {};

    pop_ram.code  = InstructionCode::ID_POP_XMM;
    pop_ram.type1 = ArgumentType::RAM;
    pop_ram.arg1  = ram_id;

    IRInsert(ir, &pop_ram, error);
    break;
})

DEF_OP(PRINT, false, "57<<", (0), false,
{
    TranslateNodeToIR(ir, tree, node->left, tables, labels, ram_spot, error);

    IRInsert(ir, &POP_VAL0, error);
    IRInsert(ir, &OUT_INSTR, error);
    break;
})

DEF_OP(TYPE_INT, false, "57::", (0), false, {})

DEF_OP(L_BRACKET, true, "(", (0), false, {})
DEF_OP(R_BRACKET, true, ")", (0), false, {})
DEF_OP(COMMA, true, ",", (0), false,
{
    TranslateNodeToIR(ir, tree, node->right, tables, labels, ram_spot, error);      // TODO notice right -> left
    TranslateNodeToIR(ir, tree, node->left, tables, labels, ram_spot, error);
    break;
})
DEF_OP(LINE_END, true, "\n", (0), false,
{
    TranslateNodeToIR(ir, tree, node->left, tables, labels, ram_spot, error);
    TranslateNodeToIR(ir, tree, node->right, tables, labels, ram_spot, error);
    break;
})
DEF_OP(BLOCK_END, false, ".57", (0), false, {})
DEF_OP(FUNC_WALL, false, "##################################################", (0), false, {})

DEF_OP(RETURN, false, "~57", (0), false,
{
    TranslateNodeToIR(ir, tree, node->left, tables, labels, ram_spot, error);

    IRInsert(ir, &RET_VAL, error);        // pop xmm0
    IRInsert(ir, &RSP_RET, error);       // mov rsp, rbp
    IRInsert(ir, &STK_FRAME_RET, error); // pop rbp
    IRInsert(ir, &RETURN, error);
    break;
})

DEF_OP(END, false, "@57@", (0), false, {})





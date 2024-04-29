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
    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);
    TranslateNodeToAsm(tree, node->right, tables, out_stream, error);
    PrintWithTabs(out_stream, TABS_AMT, "add\n");
    break;
})

DEF_OP(SUB, true, "-", (NUMBER_1 - NUMBER_2), true,
{
    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);
    TranslateNodeToAsm(tree, node->right, tables, out_stream, error);
    PrintWithTabs(out_stream, TABS_AMT, "sub\n");
    break;
})

DEF_OP(MUL, true, "*", (NUMBER_1 * NUMBER_2), true,
{
    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);
    TranslateNodeToAsm(tree, node->right, tables, out_stream, error);
    PrintWithTabs(out_stream, TABS_AMT, "mul\n");
    break;
})

DEF_OP(DIV, true, "/", (NUMBER_1 / NUMBER_2), true,
{
    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);
    TranslateNodeToAsm(tree, node->right, tables, out_stream, error);
    PrintWithTabs(out_stream, TABS_AMT, "div\n");
    break;
})

DEF_OP(DEG, true, "^", ( pow(NUMBER_1, NUMBER_2) ), true,
{
    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);
    TranslateNodeToAsm(tree, node->right, tables, out_stream, error);
    PrintWithTabs(out_stream, TABS_AMT, "pow\n");
    break;
})

DEF_OP(ASSIGN, false, ":=", (0), false,
{
    TranslateAssignToAsm(tree, node, tables, &ram_spot, out_stream, error);
    break;
})

DEF_OP(SIN, false, "$1#", ( sin( NUMBER_1 )), true,
{
    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);
    PrintWithTabs(out_stream, TABS_AMT, "sin\n");
    break;
})

DEF_OP(SQRT, false, "57#", ( sqrt( NUMBER_1 )), true,
{
    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);
    PrintWithTabs(out_stream, TABS_AMT, "sqrt\n");
    break;
})

DEF_OP(COS, false, "<0$", ( cos( NUMBER_1 )), true,
{
    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);
    PrintWithTabs(out_stream, TABS_AMT, "cos\n");
    break;
})

DEF_OP(IF, false, "57?", (0), false,
{
    int free_label = label_spot++;

    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);

    PrintWithTabs(out_stream, TABS_AMT, "push 0" LOTS_TABS "%% IF START\n");
    PrintWithTabs(out_stream, TABS_AMT, "je :FALSE_COND_%d\n", (int) free_label);

    nametable_t* local = MakeNametable();
    StackPush(tables, local);

    TranslateNodeToAsm(tree, node->right, tables, out_stream, error);

    StackPop(tables);

    fprintf(out_stream, ":FALSE_COND_%d" LOTS_TABS "%% IF END\n\n", (int) free_label);

    break;
})

DEF_OP(WHILE, false, "1000_7", (0), false,
{
    int free_label_cycle = label_spot++;
    int free_label_cond  = label_spot++;

    fprintf(out_stream, "\n:WHILE_CYCLE_%d" LOTS_TABS "%% WHILE START\n", free_label_cycle);

    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);

    PrintWithTabs(out_stream, TABS_AMT, "push 0\n");
    PrintWithTabs(out_stream, TABS_AMT, "je :QUIT_CYCLE_%d\n", free_label_cond);

    nametable_t* local = MakeNametable();
    StackPush(tables, local);

    TranslateNodeToAsm(tree, node->right, tables, out_stream, error);

    StackPop(tables);

    PrintWithTabs(out_stream, TABS_AMT, "jmp :WHILE_CYCLE_%d\n", free_label_cycle);
    fprintf(out_stream, ":QUIT_CYCLE_%d" LOTS_TABS "%% WHILE END\n\n", free_label_cond);

    break;
})

DEF_OP(GREATER, false,">", (NUMBER_1 > NUMBER_2), true,
{
    TranslateCompareToAsm(tree, node, tables, &label_spot, "jb", out_stream, error);
    break;
})
DEF_OP(GREATER_EQ, false, ">=", (NUMBER_1 >= NUMBER_2), true,
{
    TranslateCompareToAsm(tree, node, tables, &label_spot, "jbe", out_stream, error);
    break;
})
DEF_OP(LESS, false, "<", (NUMBER_1 < NUMBER_2), true,
{
    TranslateCompareToAsm(tree, node, tables, &label_spot, "ja", out_stream, error);
    break;
})
DEF_OP(LESS_EQ, false, "<=", (NUMBER_1 <= NUMBER_2), true,
{
    TranslateCompareToAsm(tree, node, tables, &label_spot, "jae", out_stream, error);
    break;
})
DEF_OP(EQ,false, "==", (NUMBER_1 == NUMBER_2), true,
{
    TranslateCompareToAsm(tree, node, tables, &label_spot, "je", out_stream, error);
    break;
})
DEF_OP(NOT_EQ, false, "!=", (NUMBER_1 != NUMBER_2), true,
{
    TranslateCompareToAsm(tree, node, tables, &label_spot, "jne", out_stream, error);
    break;
})
DEF_OP(AND, false, "&&", (NUMBER_1 && NUMBER_2), true,
{
    TranslateAndToAsm(tree, node, tables, &label_spot, out_stream, error);
    break;
})
DEF_OP(OR, false, "||", (NUMBER_1 || NUMBER_2), true,
{
    TranslateOrToAsm(tree, node, tables, &label_spot, out_stream, error);
    break;
})

DEF_OP(READ, false, "57>>", (0), false,
{
    PrintWithTabs(out_stream, TABS_AMT, "in\n");

    int ram_id = GetNameRamIdFromStack(tables, NODE_NAME(node->left));

    PrintWithTabs(out_stream, TABS_AMT, "pop [%d]\n", ram_id);
    break;
})

DEF_OP(PRINT, false, "57<<", (0), false,
{
    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);
    PrintWithTabs(out_stream, TABS_AMT, "out\n");
    break;
})

DEF_OP(TYPE_INT, false, "57::", (0), false, {})

DEF_OP(L_BRACKET, true, "(", (0), false, {})
DEF_OP(R_BRACKET, true, ")", (0), false, {})
DEF_OP(COMMA, true, ",", (0), false,
{
    TranslateNodeToAsm(tree, node->right, tables, out_stream, error);
    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);
    break;
})
DEF_OP(LINE_END, true, "\n", (0), false,
{
    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);
    TranslateNodeToAsm(tree, node->right, tables, out_stream, error);
    break;
})
DEF_OP(BLOCK_END, false, ".57", (0), false, {})
DEF_OP(FUNC_WALL, false, "##################################################", (0), false, {})

DEF_OP(RETURN, false, "~57", (0), false,
{
    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);
    fprintf(out_stream, "ret\n");
    break;
})

DEF_OP(END, false, "@57@", (0), false, {})





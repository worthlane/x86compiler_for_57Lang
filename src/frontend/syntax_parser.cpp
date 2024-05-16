#include "string.h"

#include "syntax_parser.h"
#include "analysis.h"
#include "tree/tree.h"
#include "common/errors.h"

#ifdef  CUR_PTR
#undef  CUR_PTR
#endif
#define CUR_PTR storage->ptr

#ifdef  NAMES_STK
#undef  NAMES_STK
#endif
#define NAMES_STK (storage->tokens->names_stk)

#ifdef  TOKEN_NAME
#undef  TOKEN_NAME
#endif
#define TOKEN_NAME(token) storage->tokens->all_names.list[token.info.name_id].name

#ifdef  INCREASE_PTR
#undef  INCREASE_PTR
#endif
#define INCREASE_PTR    PrintLog("GOT TOKEN[%d], READING NEXT ON LINE [%d]<br>\n",       \
                                        storage->ptr++,                __LINE__)

#ifdef  DECREASE_PTR
#undef  DECREASE_PTR
#endif
#define DECREASE_PTR    PrintLog("RETURN FROM TOKEN[%d], READING PREVIOUS ON LINE [%d]<br>\n",       \
                                                    storage->ptr++,                __LINE__)

#ifdef  CUR_TOKEN
#undef  CUR_TOKEN
#endif
#define CUR_TOKEN       storage->tokens->array[CUR_PTR]

#ifdef  NEXT_TOKEN
#undef  NEXT_TOKEN
#endif
#define NEXT_TOKEN       storage->tokens->array[CUR_PTR + 1]

#ifdef  SKIP_BREAKS
#undef  SKIP_BREAKS
#endif
#define SKIP_BREAKS     do                                                                                      \
                        {                                                                                       \
                            while(CUR_TOKEN.info.opt == Operators::LINE_END && CUR_TOKEN.type == TokenType::TOKEN)       \
                            {                                                   \
                                INCREASE_PTR;                                   \
                            }                                                   \
                        } while (false);

#ifdef  SYN_ASSERT
#undef  SYN_ASSERT
#endif
#define SYN_ASSERT(stat)                                                                            \
        do                                                                                              \
        {                                                                                               \
            if (!(stat))                                                                                \
            {                                                                                           \
                LOG_START(__func__, __FILE__, __LINE__);                                                \
                error->code = (int) FrontendErrors::INVALID_SYNTAX;                                     \
                SetErrorData(error, "SYNTAX ASSERT\"" #stat "\"<br>\n"                                  \
                                    "IN FUNCTION %s FROM FILE \"%s\"(%d)<br>\n"                         \
                                    "IN LINE %d<br>\n",                                                 \
                                    __func__, __FILE__, __LINE__, CUR_TOKEN.line);                         \
                LOG_END();                                                                              \
                return nullptr;                                                                         \
            }                                                                                           \
        } while (false)                                                                                 \


#ifdef  CONSUME
#undef  CONSUME
#endif
#define CONSUME(stat)                           \
        do                                      \
        {                                       \
            SYN_ASSERT(stat);                   \
            INCREASE_PTR;                       \
        } while (false)


#ifdef  PEEK
#undef  PEEK
#endif
#define PEEK(op, type)   CUR_TOKEN.info.opt == op && CUR_TOKEN.type == type

#ifdef  BREAK_ASSERT
#undef  BREAK_ASSERT
#endif
#define BREAK_ASSERT(stat)                                                                            \
        do                                                                                          \
        {                                                                                           \
            if (!(stat))                                                                                \
            {                                                                                           \
                LOG_START(__func__, __FILE__, __LINE__);                                                \
                PrintLog("LEAVING FUNC BECAUSE OF\"" #stat "\"<br>\n"                                  \
                        "IN FUNCTION %s, FILE \"%s\"(%d)<br>\n",                                         \
                                    __func__, __FILE__, __LINE__);                                      \
                LOG_END();                                                                              \
                return nullptr;                                                                         \
            }                                                                                           \
        } while (false)

#ifdef  NULL_IF_ERR
#undef  NULL_IF_ERR
#endif
#define NULL_IF_ERR                             \
        do                                      \
        {                                       \
            if (error->code != (int) FrontendErrors::NONE)      \
                return nullptr;                                 \
        } while (false)

static void TryInsertNameInStack(const Stack_t* stk, const char* str,
                                 bool* exists, bool* is_func, const TokenType type);

// Print            ::= PRINT Name
// Read             ::= READ Name

// Program          ::= DefFunc Break {DefFunc Break}* END
// DefFunc          ::= Type NewFuncName L_BRACKET FuncVars R_BRACKET {LINE_END}* FUNC_WALL SubProgram FUNC_WALL
// CallVars         ::= L_BRACKET Expr { [,] Expr }* R_BRACKET
// CallFunc         ::= Name CallVars
// FuncVars         ::= OneFuncVar { [,] OneFuncVar }*
// OneFuncVar       ::= Type L_BRACKET Var R_BRACKET
// SubProgram       ::= Block
// WhileSection     ::= WHILE L_BRACKET Expression R_BRACKET {LINE_END}* Block BLOCK_END
// IfSection        ::= IF    L_BRACKET Expression R_BRACKET {LINE_END}* Block BLOCK_END
// Block            ::= {Line Break}*
// Line             ::= {IfSection | WhileSection | Return | Assignment | Init }*
// Init             ::= TYPE L_BRACKET InitialAssignment R_BRACKET
// Assignment       ::= Name ASSIGN Expression
// Return           ::= RETURN Expression
// Expression       ::= AndOperand   { [OR] AndOperand }*
// AndOperand       ::= Comparison   { [AND] Comparison }*
// Comparison       ::= Summ         { [<=>] Summ }*
// Summ             ::= Term         { [ADD SUB] Term }*
// Term             ::= Degree       { [MUL DIV] Degree }*
// Degree           ::= Trigonometry { [^] Trigonometry }*
// Trigonometry     ::=              { [SIN COS] Brackets }*
// Brackets         ::= L_BRACKET Expression R_BRACKET | Component
// Component        ::= [+ -] (Name | Num | CallFunc)
// Type             ::= TYPE
// Break            ::= LINE_END
// Name             ::= NAME
// Num              ::= NUM

static Node* GetPrint(ParserState* storage, error_t* error);
static Node* GetRead(ParserState* storage, error_t* error);

static Node* GetNewVar(ParserState* storage, error_t* error);
static Node* GetOldVar(ParserState* storage, error_t* error);
static Node* GetNewFuncName(ParserState* storage, error_t* error);
static Node* GetOldFuncName(ParserState* storage, error_t* error);

static Node* CallFunc(ParserState* storage, error_t* error);
static Node* CallVars(ParserState* storage, error_t* error);
static Node* GetProgram(ParserState* storage, error_t* error);
static Node* DefFunc(ParserState* storage, error_t* error);
static Node* GetSubProgram(ParserState* storage, error_t* error);
static Node* GetOneFuncVar(ParserState* storage, error_t* error);
static Node* GetFuncVars(ParserState* storage, error_t* error);
static Node* GetName(ParserState* storage, error_t* error);
static Node* GetNum(ParserState* storage, error_t* error);
static Node* GetBreak(ParserState* storage, error_t* error);
static Node* GetType(ParserState* storage, error_t* error);
static Node* GetInit(ParserState* storage, error_t* error);
static Node* GetBlock(ParserState* storage, error_t* error);
static Node* GetLine(ParserState* storage, error_t* error);
static Node* GetReturn(ParserState* storage, error_t* error);
static Node* GetWhileSection(ParserState* storage, error_t* error);
static Node* GetIfSection(ParserState* storage, error_t* error);
static Node* GetAssignment(ParserState* storage, error_t* error);
static Node* GetInitialAssignment(ParserState* storage, error_t* error);
static Node* GetExpression(ParserState* storage, error_t* error);
static Node* GetSumm(ParserState* storage, error_t* error);
static Node* GetComparison(ParserState* storage, error_t* error);
static Node* GetAndOperand(ParserState* storage, error_t* error);
static Node* GetTerm(ParserState* storage, error_t* error);
static Node* GetDegree(ParserState* storage, error_t* error);
static Node* GetTrigonometry(ParserState* storage, error_t* error);
static Node* GetBrackets(ParserState* storage, error_t* error);
static Node* GetComponent(ParserState* storage, error_t* error);

// -------------------------------------------------------------

void GetTreeFromTokens(Tokens* tokens, tree_t* tree, error_t* error)
{
    assert(tokens);
    assert(tree);
    assert(error);

    ParserState syn = {};
    syn.ptr = 0;
    syn.tokens = tokens;

    tree->root  = GetProgram(&syn, error);

    CopyNametable(&(tokens->all_names), &(tree->names));
}

// -------------------------------------------------------------

static Node* GetProgram(ParserState* storage, error_t* error)
{
    assert(storage);
    assert(error);

    Node* funcs = nullptr;

    SYN_ASSERT(IsType(CUR_TOKEN.info.opt) && CUR_TOKEN.type == TokenType::TOKEN);

    funcs = DefFunc(storage, error);
    NULL_IF_ERR;

    GetBreak(storage, error);
    NULL_IF_ERR;

    SKIP_BREAKS;

    while (IsType(CUR_TOKEN.info.opt) && CUR_TOKEN.type == TokenType::TOKEN)
    {
        Node* func = DefFunc(storage, error);
        NULL_IF_ERR;

        GetBreak(storage, error);
        NULL_IF_ERR;

        SKIP_BREAKS;

        funcs = MakeNode(NodeType::OP, {.opt = Operators::NEW_FUNC}, funcs, func);
    }

    SYN_ASSERT(CUR_TOKEN.info.opt == Operators::END && CUR_TOKEN.type == TokenType::TOKEN);

    return funcs;
}

// -------------------------------------------------------------

static Node* GetNewVar(ParserState* storage, error_t* error)
{
    assert(storage);
    assert(error);

    SYN_ASSERT(CUR_TOKEN.type == TokenType::NAME);

    Node* var = MakeNode(NodeType::VAR, {.var = CUR_TOKEN.info.name_id});

    bool exists = false;
    bool is_func = false;

    TryInsertNameInStack(&NAMES_STK, TOKEN_NAME(CUR_TOKEN), &exists, &is_func, TokenType::VAR_NAME);

    if (exists)
    {
        error->code = (int) FrontendErrors::INVALID_SYNTAX;
        if (!is_func)
            SetErrorData(error, "VARIABLE ON LINE %d ALREADY EXISTS<br>\n", CUR_TOKEN.line);
        else
            SetErrorData(error, "CAN NOT NAME VARIABLE ON LINE %d AS FUNCTION<br>\n", CUR_TOKEN.line);
        return nullptr;
    }

    INCREASE_PTR;

    return var;
}

// -------------------------------------------------------------

static Node* GetOldVar(ParserState* storage, error_t* error)
{
    assert(storage);
    assert(error);

    SYN_ASSERT(CUR_TOKEN.type == TokenType::NAME);

    Node* var = MakeNode(NodeType::VAR, {.var = CUR_TOKEN.info.name_id});

    if (storage->tokens->all_names.list[CUR_TOKEN.info.name_id].name[0] == '\"')
        return var;

    bool exists = false;
    bool is_func = false;

    TryInsertNameInStack(&NAMES_STK, TOKEN_NAME(CUR_TOKEN), &exists, &is_func, TokenType::VAR_NAME);

    if (is_func)
    {
        error->code = (int) FrontendErrors::INVALID_SYNTAX;
        SetErrorData(error, "CAN NOT GET FUNCTION AS VARIABLE ON LINE %d<br>\n", CUR_TOKEN.line);
        return nullptr;
    }
    if (!exists)
    {
        error->code = (int) FrontendErrors::INVALID_SYNTAX;
        SetErrorData(error, "UNKNOWN VARIABLE ON LINE %d<br>\n", CUR_TOKEN.line);
        return nullptr;
    }

    INCREASE_PTR;

    return var;
}

// -------------------------------------------------------------

static Node* GetOldFuncName(ParserState* storage, error_t* error)
{
    assert(storage);
    assert(error);

    SYN_ASSERT(CUR_TOKEN.type == TokenType::NAME);

    Node* var = MakeNode(NodeType::VAR, {.var = CUR_TOKEN.info.name_id});

    bool exists = false;
    bool is_func = false;

    TryInsertNameInStack(&NAMES_STK, TOKEN_NAME(CUR_TOKEN), &exists, &is_func, TokenType::FUNC_NAME);

    if (!is_func)
    {
        error->code = (int) FrontendErrors::INVALID_SYNTAX;
        SetErrorData(error, "CAN NOT GET VARIABLE AS FUNCTION ON LINE %d<br>\n", CUR_TOKEN.line);
        return nullptr;
    }
    if (!exists)
    {
        error->code = (int) FrontendErrors::INVALID_SYNTAX;
        SetErrorData(error, "UNKNOWN FUNCTION ON LINE %d<br>\n", CUR_TOKEN.line);
        return nullptr;
    }

    INCREASE_PTR;

    return var;
}

// -------------------------------------------------------------

static Node* GetNewFuncName(ParserState* storage, error_t* error)
{
    assert(storage);
    assert(error);

    SYN_ASSERT(CUR_TOKEN.type == TokenType::NAME);

    Node* var = MakeNode(NodeType::VAR, {.var = CUR_TOKEN.info.name_id});

    bool exists = false;
    bool is_func = false;

    TryInsertNameInStack(&NAMES_STK, TOKEN_NAME(CUR_TOKEN), &exists, &is_func, TokenType::FUNC_NAME);

    if (exists)
    {
        error->code = (int) FrontendErrors::INVALID_SYNTAX;
        if (is_func)
            SetErrorData(error, "FUNCTION ON LINE %d ALREADY EXISTS<br>\n", CUR_TOKEN.line);
        else
            SetErrorData(error, "CAN NOT NAME FUNCTION ON LINE %d AS VARIABLE<br>\n", CUR_TOKEN.line);
        return nullptr;
    }

    INCREASE_PTR;

    return var;
}

// -------------------------------------------------------------

static Node* DefFunc(ParserState* storage, error_t* error)
{
    assert(storage);
    assert(error);

    Node* type = GetType(storage, error);
    NULL_IF_ERR;

    CONSUME(CUR_TOKEN.type == TokenType::TOKEN && CUR_TOKEN.info.opt == Operators::L_BRACKET);

    Node* name = GetNewFuncName(storage, error);
    NULL_IF_ERR;

    CONSUME(CUR_TOKEN.type == TokenType::TOKEN && CUR_TOKEN.info.opt == Operators::L_BRACKET);

    nametable_t* local_table = MakeNametable();
    StackPush(&NAMES_STK, local_table);

    Node* vars = GetFuncVars(storage, error);
    NULL_IF_ERR;

    CONSUME(CUR_TOKEN.type == TokenType::TOKEN && CUR_TOKEN.info.opt == Operators::R_BRACKET);
    CONSUME(CUR_TOKEN.type == TokenType::TOKEN && CUR_TOKEN.info.opt == Operators::R_BRACKET);

    SKIP_BREAKS;

    CONSUME(CUR_TOKEN.type == TokenType::TOKEN && CUR_TOKEN.info.opt == Operators::FUNC_WALL);

    Node* action = GetSubProgram(storage, error);
    NULL_IF_ERR;

    StackPop(&NAMES_STK);

    CONSUME(CUR_TOKEN.type == TokenType::TOKEN && CUR_TOKEN.info.opt == Operators::FUNC_WALL);

    ConnectNodes(name, vars, action);

    Node* func = MakeNode(NodeType::OP, {.opt = Operators::FUNC}, name);

    Node* fictive_type = MakeNode(NodeType::OP, {.opt = Operators::TYPE}, type, func);

    return fictive_type;
}

// -------------------------------------------------------------

static Node* CallFunc(ParserState* storage, error_t* error)
{
    assert(storage);
    assert(error);

    Node* name = GetOldFuncName(storage, error);
    NULL_IF_ERR;

    Node* vars = CallVars(storage, error);
    NULL_IF_ERR;

    Node* fictive_node = MakeNode(NodeType::OP, {.opt = Operators::FUNC_CALL}, name);

    ConnectNodes(name, vars, nullptr);

    return fictive_node;
}

// -------------------------------------------------------------

static Node* GetOneFuncVar(ParserState* storage, error_t* error)
{
    assert(storage);
    assert(error);

    Node* type = GetType(storage, error);
    NULL_IF_ERR;

    CONSUME(CUR_TOKEN.type == TokenType::TOKEN && CUR_TOKEN.info.opt == Operators::L_BRACKET);

    Node* var = GetNewVar(storage, error);
    NULL_IF_ERR;

    CONSUME(CUR_TOKEN.type == TokenType::TOKEN && CUR_TOKEN.info.opt == Operators::R_BRACKET);

    Node* fictive_connect = MakeNode(NodeType::OP, {.opt = Operators::TYPE}, type, var);

    return fictive_connect;
}

// -------------------------------------------------------------

static Node* GetFuncVars(ParserState* storage, error_t* error)
{
    assert(storage);
    assert(error);

    Node* vars = nullptr;

    if (!(IsType(CUR_TOKEN.info.opt) && CUR_TOKEN.type == TokenType::TOKEN)) { return nullptr; }

    vars = GetOneFuncVar(storage, error);
    NULL_IF_ERR;

    while (CUR_TOKEN.info.opt == Operators::COMMA && CUR_TOKEN.type == TokenType::TOKEN)
    {
        INCREASE_PTR;

        Node* var = GetOneFuncVar(storage, error);
        NULL_IF_ERR;

        vars = MakeNode(NodeType::OP, {.opt = Operators::COMMA}, vars, var);
    }

    return vars;
}

// -------------------------------------------------------------

static Node* CallVars(ParserState* storage, error_t* error)
{
    assert(storage);
    assert(error);

    Node* vars = nullptr;

    CONSUME(CUR_TOKEN.info.opt == Operators::L_BRACKET && CUR_TOKEN.type == TokenType::TOKEN);

    if (CUR_TOKEN.info.opt == Operators::R_BRACKET && CUR_TOKEN.type == TokenType::TOKEN) { return nullptr; }

    vars = GetExpression(storage, error);
    NULL_IF_ERR;

    while (CUR_TOKEN.info.opt == Operators::COMMA && CUR_TOKEN.type == TokenType::TOKEN)
    {
        INCREASE_PTR;

        Node* var = GetExpression(storage, error);
        NULL_IF_ERR;

        vars = MakeNode(NodeType::OP, {.opt = Operators::COMMA}, vars, var);
    }

    CONSUME(CUR_TOKEN.info.opt == Operators::R_BRACKET && CUR_TOKEN.type == TokenType::TOKEN);

    return vars;
}

// -------------------------------------------------------------

static Node* GetBreak(ParserState* storage, error_t* error)
{
    CONSUME(CUR_TOKEN.type     == TokenType::TOKEN &&
            CUR_TOKEN.info.opt == Operators::LINE_END);

    Node* break_token = MakeNode(NodeType::OP, {.opt = Operators::LINE_END});

    return break_token;
}

// -------------------------------------------------------------

static Node* GetType(ParserState* storage, error_t* error)
{
    SYN_ASSERT(IsType(CUR_TOKEN.info.opt) && CUR_TOKEN.type == TokenType::TOKEN);
    Node* type = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
    INCREASE_PTR;

    return type;
}

// -------------------------------------------------------------

static Node* GetName(ParserState* storage, error_t* error)
{
    assert(storage);
    assert(error);

    SYN_ASSERT(CUR_TOKEN.type == TokenType::NAME);
    Node* var = MakeNode(NodeType::VAR, {.var = CUR_TOKEN.info.name_id});
    INCREASE_PTR;

    return var;
}

// -------------------------------------------------------------

static Node* GetNum(ParserState* storage, error_t* error)
{
    assert(storage);
    assert(error);

    SYN_ASSERT(CUR_TOKEN.type == TokenType::NUM);

    Node* var = MakeNode(NodeType::NUM, {.val = CUR_TOKEN.info.val});

    INCREASE_PTR;

    return var;
}

// -------------------------------------------------------------

static Node* GetInit(ParserState* storage, error_t* error)
{
    Node* type = GetType(storage, error);
    NULL_IF_ERR;

    CONSUME(CUR_TOKEN.type == TokenType::TOKEN && CUR_TOKEN.info.opt == Operators::L_BRACKET);

    Node* assign = GetInitialAssignment(storage, error);
    NULL_IF_ERR;

    CONSUME(CUR_TOKEN.type == TokenType::TOKEN && CUR_TOKEN.info.opt == Operators::R_BRACKET);

    Node* fictive_connect = MakeNode(NodeType::OP, {.opt = Operators::TYPE}, type, assign);

    return fictive_connect;
}


// -------------------------------------------------------------

static Node* GetSubProgram(ParserState* storage, error_t* error)
{
    assert(storage);
    assert(error);

    SKIP_BREAKS;

    Node* result = GetBlock(storage, error);
    NULL_IF_ERR;

    return result;
}

// -------------------------------------------------------------

static Node* GetBlock(ParserState* storage, error_t* error)
{
    assert(storage);
    assert(error);

    Node* val  = GetLine(storage, error);
    NULL_IF_ERR;

    Node* head = val;

    while (val != nullptr)
    {
        Node* val2 = GetLine(storage, error);
        NULL_IF_ERR;

        ConnectNodes(val, nullptr, val2);
        val = val2;
    }

    return head;
}

// -------------------------------------------------------------

static Node* GetLine(ParserState* storage, error_t* error)
{
    assert(storage);
    assert(error);

    Node* val = nullptr;

    SKIP_BREAKS;

    if (CUR_TOKEN.info.opt == Operators::IF && CUR_TOKEN.type == TokenType::TOKEN)
    {
        nametable_t* local_table = MakeNametable();
        StackPush(&NAMES_STK, local_table);

        val = GetIfSection(storage, error);

        StackPop(&NAMES_STK);
    }
    else if (CUR_TOKEN.info.opt == Operators::WHILE && CUR_TOKEN.type == TokenType::TOKEN)
    {
        nametable_t* local_table = MakeNametable();
        StackPush(&NAMES_STK, local_table);

        val = GetWhileSection(storage, error);

        StackPop(&NAMES_STK);
    }
    else if (CUR_TOKEN.info.opt == Operators::RETURN && CUR_TOKEN.type == TokenType::TOKEN)
    {
        val = GetReturn(storage, error);
    }
    else if (CUR_TOKEN.info.opt == Operators::READ && CUR_TOKEN.type == TokenType::TOKEN)
    {
        val = GetRead(storage, error);
    }
    else if (CUR_TOKEN.info.opt == Operators::PRINT && CUR_TOKEN.type == TokenType::TOKEN)
    {
        val = GetPrint(storage, error);
    }
    else if (IsType(CUR_TOKEN.info.opt) && CUR_TOKEN.type == TokenType::TOKEN)
    {
        val = GetInit(storage, error);
    }
    else if (CUR_TOKEN.type == TokenType::NAME)
    {
        val = GetAssignment(storage, error);
    }
    else
    {
        return nullptr;
    }

    NULL_IF_ERR;

    Node* break_token = GetBreak(storage, error);
    NULL_IF_ERR;

    ConnectNodes(break_token, val, nullptr);

    return break_token;
}

// -------------------------------------------------------------

static Node* GetIfSection(ParserState* storage, error_t* error)
{
    assert(storage);
    assert(error);

    SKIP_BREAKS;

    SYN_ASSERT(CUR_TOKEN.info.opt == Operators::IF && CUR_TOKEN.type == TokenType::TOKEN);
    Node* if_token = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
    INCREASE_PTR;

    CONSUME(CUR_TOKEN.type     == TokenType::TOKEN &&
            CUR_TOKEN.info.opt == Operators::L_BRACKET);

    Node* cond = GetExpression(storage, error);
    NULL_IF_ERR;

    CONSUME(CUR_TOKEN.type     == TokenType::TOKEN &&
            CUR_TOKEN.info.opt == Operators::R_BRACKET);

    SKIP_BREAKS;

    Node* action = GetBlock(storage, error);
    NULL_IF_ERR;

    ConnectNodes(if_token, cond, action);

    CONSUME(CUR_TOKEN.type     == TokenType::TOKEN &&
            CUR_TOKEN.info.opt == Operators::BLOCK_END);

    return if_token;
}

// -------------------------------------------------------------

static Node* GetWhileSection(ParserState* storage, error_t* error)
{
    assert(storage);
    assert(error);

    SKIP_BREAKS;

    SYN_ASSERT(CUR_TOKEN.info.opt == Operators::WHILE && CUR_TOKEN.type == TokenType::TOKEN);
    Node* while_token = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
    INCREASE_PTR;

    CONSUME(CUR_TOKEN.type     == TokenType::TOKEN &&
            CUR_TOKEN.info.opt == Operators::L_BRACKET);

    Node* cond = GetExpression(storage, error);
    NULL_IF_ERR;

    CONSUME(CUR_TOKEN.type     == TokenType::TOKEN &&
            CUR_TOKEN.info.opt == Operators::R_BRACKET);

    SKIP_BREAKS;

    Node* action = GetBlock(storage, error);
    NULL_IF_ERR;

    ConnectNodes(while_token, cond, action);

    CONSUME(CUR_TOKEN.type     == TokenType::TOKEN &&
            CUR_TOKEN.info.opt == Operators::BLOCK_END);

    return while_token;
}

// -------------------------------------------------------------

static Node* GetAssignment(ParserState* storage, error_t* error)
{
    assert(storage);
    assert(error);

    SKIP_BREAKS;

    Node* var = GetOldVar(storage, error);
    NULL_IF_ERR;

    SYN_ASSERT(CUR_TOKEN.type == TokenType::TOKEN && CUR_TOKEN.info.opt == Operators::ASSIGN);
    Node* assign = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
    INCREASE_PTR;

    Node* val    = GetExpression(storage, error);
    NULL_IF_ERR;

    ConnectNodes(assign, var, val);

    return assign;
}

// -------------------------------------------------------------

static Node* GetInitialAssignment(ParserState* storage, error_t* error)
{
    assert(storage);
    assert(error);

    SKIP_BREAKS;

    Node* var = GetNewVar(storage, error);
    NULL_IF_ERR;

    SYN_ASSERT(CUR_TOKEN.type == TokenType::TOKEN && CUR_TOKEN.info.opt == Operators::ASSIGN);
    Node* assign = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
    INCREASE_PTR;

    Node* val    = GetExpression(storage, error);
    NULL_IF_ERR;

    ConnectNodes(assign, var, val);

    return assign;
}

// -------------------------------------------------------------

static Node* GetReturn(ParserState* storage, error_t* error)
{
    assert(storage);
    assert(error);

    SKIP_BREAKS;

    SYN_ASSERT(CUR_TOKEN.info.opt == Operators::RETURN && CUR_TOKEN.type == TokenType::TOKEN);
    Node* ret = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
    INCREASE_PTR;

    Node* expr   = GetExpression(storage, error);

    NULL_IF_ERR;

    ConnectNodes(ret, expr, nullptr);

    return ret;
}

// -------------------------------------------------------------

static Node* GetExpression(ParserState* storage, error_t* error)
{
    assert(error);
    assert(storage);

    Node* val = GetAndOperand(storage, error);
    NULL_IF_ERR;

    while (CUR_TOKEN.type == TokenType::TOKEN && CUR_TOKEN.info.opt == Operators::OR)
    {
        Node* op = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
        INCREASE_PTR;

        Node* val2 = GetAndOperand(storage, error);
        NULL_IF_ERR;

        val = ConnectNodes(op, val, val2);
    }

    return val;
}


// -------------------------------------------------------------

static Node* GetAndOperand(ParserState* storage, error_t* error)
{
    assert(error);
    assert(storage);

    Node* val = GetComparison(storage, error);
    NULL_IF_ERR;

    while (CUR_TOKEN.type == TokenType::TOKEN && CUR_TOKEN.info.opt == Operators::AND)
    {
        Node* op = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
        INCREASE_PTR;

        Node* val2 = GetComparison(storage, error);
        NULL_IF_ERR;

        val = ConnectNodes(op, val, val2);
    }

    return val;
}

// -------------------------------------------------------------

static Node* GetComparison(ParserState* storage, error_t* error)
{
    assert(error);
    assert(storage);

    Node* val = GetSumm(storage, error);
    NULL_IF_ERR;

    if (CUR_TOKEN.type == TokenType::TOKEN &&
       (CUR_TOKEN.info.opt == Operators::LESS         || CUR_TOKEN.info.opt == Operators::LESS_EQ ||
        CUR_TOKEN.info.opt == Operators::EQ        || CUR_TOKEN.info.opt == Operators::GREATER   ||
        CUR_TOKEN.info.opt == Operators::GREATER_EQ || CUR_TOKEN.info.opt == Operators::NOT_EQ))
    {
        Node* op = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
        INCREASE_PTR;

        Node* val2 = GetSumm(storage, error);
        NULL_IF_ERR;

        val = ConnectNodes(op, val, val2);
    }

    return val;
}

// -------------------------------------------------------------

static Node* GetSumm(ParserState* storage, error_t* error)
{
    assert(storage);
    assert(error);

    Node* val = GetTerm(storage, error);
    NULL_IF_ERR;

    while (CUR_TOKEN.type     == TokenType::TOKEN  &&
          (CUR_TOKEN.info.opt == Operators::ADD ||
           CUR_TOKEN.info.opt == Operators::SUB))
    {
        Node* op = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
        INCREASE_PTR;

        Node* val2 = GetTerm(storage, error);
        NULL_IF_ERR;

        val = ConnectNodes(op, val, val2);
    }
    return val;
}

// -------------------------------------------------------------

static Node* GetTerm(ParserState* storage, error_t* error)
{
    assert(error);
    assert(storage);

    Node* val = GetDegree(storage, error);
    NULL_IF_ERR;

    while (CUR_TOKEN.type     == TokenType::TOKEN  &&
          (CUR_TOKEN.info.opt == Operators::DIV ||
           CUR_TOKEN.info.opt == Operators::MUL))
    {
        Node* op = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
        INCREASE_PTR;

        Node* val2 = GetDegree(storage, error);
        NULL_IF_ERR;

        val = ConnectNodes(op, val, val2);
    }
    return val;
}

// -------------------------------------------------------------

static Node* GetDegree(ParserState* storage, error_t* error)
{
    assert(error);
    assert(storage);

    Node* val = GetTrigonometry(storage, error);
    NULL_IF_ERR;

    while (CUR_TOKEN.type     == TokenType::TOKEN &&
           CUR_TOKEN.info.opt == Operators::DEG)
    {
        Node* op = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
        INCREASE_PTR;

        Node* val2 = GetTrigonometry(storage, error);
        NULL_IF_ERR;

        val = ConnectNodes(op, val, val2);
    }
    return val;
}

// -------------------------------------------------------------

static Node* GetTrigonometry(ParserState* storage, error_t* error)
{
    assert(error);
    assert(storage);

    if (CUR_TOKEN.type     == TokenType::TOKEN &&
       (CUR_TOKEN.info.opt == Operators::SIN    ||
        CUR_TOKEN.info.opt == Operators::COS    ||
        CUR_TOKEN.info.opt == Operators::SQRT))
    {
        Node* op = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
        INCREASE_PTR;

        Node* val = GetBrackets(storage, error);
        NULL_IF_ERR;

        val = ConnectNodes(op, val, nullptr);

        return val;
    }

    return GetBrackets(storage, error);
    NULL_IF_ERR;

}

// -------------------------------------------------------------

static Node* GetBrackets(ParserState* storage, error_t* error)
{
    assert(error);
    assert(storage);

    if (CUR_TOKEN.type     == TokenType::TOKEN &&
        CUR_TOKEN.info.opt == Operators::L_BRACKET)
    {
        INCREASE_PTR;
        Node* val = GetExpression(storage, error);
        NULL_IF_ERR;

        SYN_ASSERT(CUR_TOKEN.type     == TokenType::TOKEN &&
                   CUR_TOKEN.info.opt == Operators::R_BRACKET);
        INCREASE_PTR;

        return val;
    }

    return GetComponent(storage, error);
}

// -------------------------------------------------------------

static Node* GetComponent(ParserState* storage, error_t* error)
{
    assert(error);
    assert(storage);

    Node* val = nullptr;

    if  (CUR_TOKEN.type     == TokenType::TOKEN  &&
        (CUR_TOKEN.info.opt == Operators::ADD ||
         CUR_TOKEN.info.opt == Operators::SUB))
    {
        Node* op = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
        INCREASE_PTR;

        SYN_ASSERT(CUR_TOKEN.type == TokenType::NUM || CUR_TOKEN.type == TokenType::NAME);

        Node* num = nullptr;

        if (CUR_TOKEN.type == TokenType::NUM)
            num = GetNum(storage, error);
        else
        {
            if (NEXT_TOKEN.type == TokenType::TOKEN && NEXT_TOKEN.info.opt == Operators::L_BRACKET)
            {
                Node* call_func = CallFunc(storage, error);
                NULL_IF_ERR;

                return call_func;
            }
            else
                num = GetOldVar(storage, error);
        }

        NULL_IF_ERR;

        val = ConnectNodes(op, MakeNode(NodeType::NUM, {.val = 0}), num);
    }
    else
    {
        SYN_ASSERT(CUR_TOKEN.type == TokenType::NUM || CUR_TOKEN.type == TokenType::NAME);

        Node* num = nullptr;

        if (CUR_TOKEN.type == TokenType::NUM)
            num = GetNum(storage, error);
        else
        {
            if (NEXT_TOKEN.type == TokenType::TOKEN && NEXT_TOKEN.info.opt == Operators::L_BRACKET)
            {
                Node* call_func = CallFunc(storage, error);
                NULL_IF_ERR;

                return call_func;
            }
            else
                num = GetOldVar(storage, error);
        }

        NULL_IF_ERR;

        val = num;
    }

    return val;
}

//-----------------------------------------------------------------------------------------------------

static void TryInsertNameInStack(const Stack_t* stk, const char* str,
                                 bool* exists, bool* is_func, const TokenType type)
{
    assert(stk);
    assert(str);
    assert(is_func);
    assert(exists);

    bool found = false;

    for (int i = 0; i < stk->size; i++)
    {
        found = FindNameInTable(stk->data[i], str, exists, is_func);

        if (found)
            break;
    }

    if (!found)
    {
        InsertNameInTable(stk->data[stk->size - 1], str, type);
    }
}

//-----------------------------------------------------------------------------------------------------

static Node* GetPrint(ParserState* storage, error_t* error)
{
    CONSUME(CUR_TOKEN.type == TokenType::TOKEN && CUR_TOKEN.info.opt == Operators::PRINT);

    SYN_ASSERT(CUR_TOKEN.type == TokenType::NUM || CUR_TOKEN.type == TokenType::NAME);

    Node* elem = nullptr;

    if (CUR_TOKEN.type == TokenType::NUM)
        elem = GetNum(storage, error);
    else
        elem = GetOldVar(storage, error);

    NULL_IF_ERR;

    Node* print = MakeNode(NodeType::OP, {.opt = Operators::PRINT});

    ConnectNodes(print, elem, nullptr);

    return print;
}

//-----------------------------------------------------------------------------------------------------

static Node* GetRead(ParserState* storage, error_t* error)
{
    CONSUME(CUR_TOKEN.type == TokenType::TOKEN && CUR_TOKEN.info.opt == Operators::READ);

    SYN_ASSERT(CUR_TOKEN.type == TokenType::NAME);

    Node* elem = GetOldVar(storage, error);

    NULL_IF_ERR;

    Node* read = MakeNode(NodeType::OP, {.opt = Operators::READ});

    ConnectNodes(read, elem, nullptr);

    return read;
}


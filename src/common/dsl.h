// ======================================================================
// DSL
// ======================================================================

#ifdef L
#undef L
#endif
#define L(node)   node->left

#ifdef R
#undef R
#endif
#define R(node)   node->right

#ifdef TYPE
#undef TYPE
#endif
#define TYPE(node)   node->type

#ifdef VAL
#undef VAL
#endif
#define VAL(node)   node->value.val

#ifdef VAR
#undef VAR
#endif
#define VAR(node)   node->value.var

#ifdef OPT
#undef OPT
#endif
#define OPT(node)   node->value.opt

#ifdef _NUM
#undef _NUM
#endif
#define _NUM(num)  MakeNode(NodeType::NUM, {.val = num}, nullptr, nullptr, nullptr)

#ifdef _VAR
#undef _VAR
#endif
#define _VAR(id)   MakeNode(NodeType::VAR, {.var = id}, nullptr, nullptr, nullptr)

#ifdef _OPT
#undef _OPT
#endif
#define _OPT(op)  MakeNode(NodeType::OP, {.opt = op}, nullptr, nullptr, nullptr)


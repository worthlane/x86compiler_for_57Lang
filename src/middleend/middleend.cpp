#include <math.h>

#include "tree/tree.h"
#include "middleend.h"
#include "common/dsl.h"

static void SimplifyTreeConstants(tree_t* tree, Node* node, int* transform_cnt, error_t* error);
static double CalculateSubtree(const tree_t* tree, Node* node, error_t* error);
static void UniteSubtree(tree_t* tree, Node* node, error_t* error);
static double OperatorAction(const double NUMBER_1, const double NUMBER_2,
                             const Operators operation, error_t* error);
static double CalculateSubtree(const tree_t* tree, Node* node, error_t* error);
static void SimplifyExpressionNeutrals(tree_t* tree, Node* node, int* transform_cnt, error_t* error);

static void RemoveNeutralADD(tree_t* tree, Node* node, int* transform_cnt, error_t* error);
static void RemoveNeutralSUB(tree_t* tree, Node* node, int* transform_cnt, error_t* error);
static void RemoveNeutralDIV(tree_t* tree, Node* node, int* transform_cnt, error_t* error);
static void RemoveNeutralMUL(tree_t* tree, Node* node, int* transform_cnt, error_t* error);
static void RemoveNeutralDEG(tree_t* tree, Node* node, int* transform_cnt, error_t* error);

static bool AreEqual(const double a, const double b);
static const double EPSILON  = 1e-9;


//------------------------------------------------------------------

void OptimizeTree(tree_t* tree, error_t* error)
{
    assert(tree);
    assert(error);

    int cnt = 1;
    int simple_flag = false;
    while (cnt != 0)
    {
        cnt = 0;
        SimplifyTreeConstants(tree, tree->root, &cnt, error);
        if (error->code != (int) MiddleendErrors::NONE)
            return;

        int save_cnt = cnt;

        SimplifyExpressionNeutrals(tree, tree->root, &cnt, error);
        if (error->code != (int) MiddleendErrors::NONE)
            return;
    }
}

//------------------------------------------------------------------

static void SimplifyTreeConstants(tree_t* tree, Node* node, int* transform_cnt, error_t* error)
{
    assert(tree);
    assert(error);
    assert(transform_cnt);

    if (!node)
        return;
    if (node->left == nullptr && node->right == nullptr)
        return;

    SimplifyTreeConstants(tree, node->left, transform_cnt, error);
    if (error->code != (int) MiddleendErrors::NONE)
        return;

    SimplifyTreeConstants(tree, node->right, transform_cnt, error);
    if (error->code != (int) MiddleendErrors::NONE)
        return;

    if (node->left == nullptr || node->right == nullptr)
        return;

    if (TYPE(node->left) == NodeType::NUM && TYPE(node->right) == NodeType::NUM)
    {
        UniteSubtree(tree, node, error);
        *transform_cnt++;
        return;
    }
}

//------------------------------------------------------------------

static void UniteSubtree(tree_t* tree, Node* node, error_t* error)
{
    assert(tree);
    assert(error);

    double num = CalculateSubtree(tree, node, error);
    if (error->code != (int) MiddleendErrors::NONE)
        return;

    DestructNodes(node->left);
    DestructNodes(node->right);

    FillNode(node, nullptr, nullptr, node->parent, NodeType::NUM, {.val = num});
}

//------------------------------------------------------------------

#define DEF_OP(name, is_char, symb, action, is_calc, ...)      \
            if (operation == Operators::name)    \
            {                                               \
                return action;                                  \
            }                             \
            else

static double OperatorAction(const double NUMBER_1, const double NUMBER_2,
                                      const Operators operation, error_t* error)
{

    #include "common/operations.h"

    /* else */
    {
        error->code = (int) MiddleendErrors::UNKNOWN_OPERATION;
        return POISON;
    }

    return POISON;
}

#undef DEF_OP

//------------------------------------------------------------------

static double CalculateSubtree(const tree_t* tree, Node* node, error_t* error)
{
    assert(tree);
    assert(error);

    if (!node) return 0;

    if (node->left == nullptr && node->right == nullptr)
    {
        if (TYPE(node) == NodeType::NUM)             return VAL(node);
        else
            return POISON;
    }

    double left_result  = CalculateSubtree(tree, node->left, error);
    double right_result = CalculateSubtree(tree, node->right, error);

    if (TYPE(node) != NodeType::OP)
    {
        error->code = (int) MiddleendErrors::INVALID_TREE;
        return 0;
    }

    if (left_result == POISON || right_result == POISON)
        return POISON;

    double result = OperatorAction(left_result, right_result, OPT(node), error);

    if (error->code == (int) MiddleendErrors::NONE)
        return result;
    else
        return POISON;

    return POISON;
}

//------------------------------------------------------------------

static void SimplifyExpressionNeutrals(tree_t* tree, Node* node, int* transform_cnt, error_t* error)
{
    assert(tree);
    assert(error);
    assert(transform_cnt);

    if (!node)
        return;
    if (node->left == nullptr && node->right == nullptr)
        return;

    SimplifyExpressionNeutrals(tree, node->left, transform_cnt, error);
    if (error->code != (int) MiddleendErrors::NONE)
        return;
    SimplifyExpressionNeutrals(tree, node->right, transform_cnt, error);
    if (error->code != (int) MiddleendErrors::NONE)
        return;

    if (TYPE(node) != NodeType::OP)
    {
        error->code = (int) MiddleendErrors::INVALID_TREE;
        return;
    }

    switch (OPT(node))
    {
        case (Operators::ADD):
            RemoveNeutralADD(tree, node, transform_cnt, error);
            break;
        case (Operators::SUB):
            RemoveNeutralSUB(tree, node, transform_cnt, error);
            break;
        case (Operators::MUL):
            RemoveNeutralMUL(tree, node, transform_cnt, error);
            break;
        case (Operators::DIV):
            RemoveNeutralDIV(tree, node, transform_cnt, error);
            break;
        case (Operators::DEG):
            RemoveNeutralDEG(tree, node, transform_cnt, error);
            break;
        default:
            break;
    }
}

//------------------------------------------------------------------

static Node* ReplaceNodeWithItsKid(tree_t* tree, Node* cur_node, NodeKid kid_side)
{
    assert(tree);
    assert(cur_node);

    Node* kid = nullptr;

    if (kid_side == NodeKid::LEFT)
        kid = cur_node->left;
    else
        kid = cur_node->right;

    assert(kid);

    if (cur_node->parent != nullptr)
        assert(cur_node->parent->left == cur_node || cur_node->parent->right == cur_node);

    kid->parent = cur_node->parent;

    if (cur_node->parent != nullptr)
    {
        if (cur_node->parent->left  == cur_node)
            cur_node->parent->left  = kid;
        else
            cur_node->parent->right = kid;
    }
    else
    {
        tree->root = kid;
    }

    cur_node->right = nullptr;
    cur_node->left  = nullptr;

    NodeDtor(cur_node);

    return kid->parent;
}

//------------------------------------------------------------------

static void RemoveNeutralADD(tree_t* tree, Node* node, int* transform_cnt, error_t* error)
{
    assert(tree);
    assert(error);
    assert(node);
    assert(transform_cnt);

    if (node->left == nullptr || node->right == nullptr)
    {
        error->code = (int) MiddleendErrors::INVALID_TREE;
        return;
    }

    if (TYPE(node->left) == NodeType::NUM && AreEqual(VAL(node->left), 0))
    {
        (*transform_cnt)++;

        DestructNodes(node->left);
        ReplaceNodeWithItsKid(tree, node, NodeKid::RIGHT);

        return;
    }

    if (TYPE(node->right) == NodeType::NUM && AreEqual(VAL(node->right), 0))
    {
        (*transform_cnt)++;

        DestructNodes(node->right);
        ReplaceNodeWithItsKid(tree, node, NodeKid::LEFT);
    }
}

//------------------------------------------------------------------

static void RemoveNeutralSUB(tree_t* tree, Node* node, int* transform_cnt, error_t* error)
{
    assert(tree);
    assert(error);
    assert(node);
    assert(transform_cnt);

    if (node->left == nullptr || node->right == nullptr)
    {
        error->code = (int) MiddleendErrors::INVALID_TREE;
        return;
    }

    if (TYPE(node->right) == NodeType::NUM && AreEqual(VAL(node->right), 0))
    {
        (*transform_cnt)++;

        DestructNodes(node->right);
        ReplaceNodeWithItsKid(tree, node, NodeKid::LEFT);

        return;
    }


    if ((TYPE(node->right) == NodeType::VAR   &&
         TYPE(node->left)  == NodeType::VAR)  &&
         VAR(node->right)  == VAR(node->left))
    {
        (*transform_cnt)++;

        DestructNodes(node->right);
        DestructNodes(node->left);

        FillNode(node, nullptr, nullptr, node->parent, NodeType::NUM, {.val = 0});
    }
}

//------------------------------------------------------------------

static void RemoveNeutralDIV(tree_t* tree, Node* node, int* transform_cnt, error_t* error)
{
    assert(tree);
    assert(error);
    assert(node);
    assert(transform_cnt);

    if (node->left == nullptr || node->right == nullptr)
    {
        error->code = (int) MiddleendErrors::INVALID_TREE;
        return;
    }

    if (TYPE(node->right) == NodeType::NUM && AreEqual(VAL(node->right), 1))
    {
        (*transform_cnt)++;

        DestructNodes(node->right);
        ReplaceNodeWithItsKid(tree, node, NodeKid::LEFT);

        return;
    }
}

//------------------------------------------------------------------

static void RemoveNeutralMUL(tree_t* tree, Node* node, int* transform_cnt, error_t* error)
{
    assert(tree);
    assert(error);
    assert(node);
    assert(transform_cnt);

    if (node->left == nullptr || node->right == nullptr)
    {
        error->code = (int) MiddleendErrors::INVALID_TREE;
        return;
    }

    if (TYPE(node->left) == NodeType::NUM)
    {
        if (AreEqual(VAL(node->left), 1))
        {
            (*transform_cnt)++;

            DestructNodes(node->left);
            ReplaceNodeWithItsKid(tree, node, NodeKid::RIGHT);
        }
        else if (AreEqual(VAL(node->left), 0))
        {
            (*transform_cnt)++;

            DestructNodes(node->right);
            ReplaceNodeWithItsKid(tree, node, NodeKid::LEFT);
        }

        return;
    }

    if (TYPE(node->right) == NodeType::NUM)
    {
        if (AreEqual(VAL(node->right), 1))
        {
            (*transform_cnt)++;

            DestructNodes(node->right);
            ReplaceNodeWithItsKid(tree, node, NodeKid::LEFT);
        }
        else if (AreEqual(VAL(node->right), 0))
        {
            (*transform_cnt)++;

            DestructNodes(node->left);
            ReplaceNodeWithItsKid(tree, node, NodeKid::RIGHT);
        }
    }
}

//------------------------------------------------------------------

static void RemoveNeutralDEG(tree_t* tree, Node* node, int* transform_cnt, error_t* error)
{
    assert(tree);
    assert(error);
    assert(node);
    assert(transform_cnt);

    if (node->left == nullptr || node->right == nullptr)
    {
        error->code = (int) MiddleendErrors::INVALID_TREE;
        return;
    }

    if (TYPE(node->left) == NodeType::NUM)
    {
        if (AreEqual(VAL(node->left), 1))
        {
            (*transform_cnt)++;

            DestructNodes(node->right);
            DestructNodes(node->left);

            FillNode(node, nullptr, nullptr, node->parent, NodeType::NUM, {.val = 1});
        }

        return;
    }

    if (TYPE(node->right) == NodeType::NUM)
    {
        if (AreEqual(VAL(node->right), 1))
        {
            (*transform_cnt)++;

            DestructNodes(node->right);
            ReplaceNodeWithItsKid(tree, node, NodeKid::LEFT);
        }
        else if (AreEqual(VAL(node->right), 0))
        {
            (*transform_cnt)++;

            DestructNodes(node->right);
            DestructNodes(node->left);

            FillNode(node, nullptr, nullptr, node->parent, NodeType::NUM, {.val = 1});
        }
    }
}

//------------------------------------------------------------------

static bool AreEqual(const double a, const double b)
{
    assert(isfinite(a));
    assert(isfinite(b));

    double diff = a - b;

    return (fabs(diff) < EPSILON) ? true : false;
}


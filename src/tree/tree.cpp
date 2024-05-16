#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "tree.h"
#include "visual.h"
#include "common/input_and_output.h"
#include "common/file_read.h"
#include "common/colorlib.h"

static TreeErrors  VerifyNodes(const Node* node, error_t* error);

//-----------------------------------------------------------------------------------------------------

Node* ConnectNodes(Node* node, Node* left, Node* right)
{
    assert(node);


    if (left != nullptr)
    {
        node->left   = left;
        left->parent = node;
    }

    if (right != nullptr)
    {
        node->right   = right;
        right->parent = node;
    }

    return node;
}

//-----------------------------------------------------------------------------------------------------

void FillNode(Node* node, Node* left, Node* right, Node* parent, const NodeType type, const NodeValue value)
{
    assert(node);

    node->left   = left;
    node->right  = right;
    node->parent = parent;
    node->type   = type;
    node->value  = value;
}

//-----------------------------------------------------------------------------------------------------

Node* MakeNode(const NodeType type, const NodeValue value,
               Node* left, Node* right, Node* parent)
{
    Node* node = (Node*) calloc(1, sizeof(Node));
    if (node == nullptr)
        return nullptr;

    node->type   = type;
    node->value  = value;
    node->parent = parent;
    node->left   = left;
    node->right  = right;

    if (right != nullptr)
        right->parent = node;

    if (left != nullptr)
        left->parent = node;

    return node;
}

//-----------------------------------------------------------------------------------------------------

void NodeDtor(Node* node)
{
    assert(node);

    free(node);
}

//-----------------------------------------------------------------------------------------------------

void LinkNodesWithParents(Node* node)
{
    if (!node) return;

    LinkNodesWithParents(node->left);
    LinkNodesWithParents(node->right);

    if (node->left != nullptr)
        node->left->parent = node;

    if (node->right != nullptr)
        node->right->parent = node;

}

//-----------------------------------------------------------------------------------------------------

TreeErrors TreeCtor(tree_t* expr, error_t* error)
{
    Node* root = MakeNode(NodeType::POISON, ZERO_VALUE, nullptr, nullptr, nullptr);

    NametableCtor(&(expr->names));

    expr->root = root;

    return TreeErrors::NONE;
}

//-----------------------------------------------------------------------------------------------------

tree_t* MakeTree(error_t* error)
{
    assert(error);

    tree_t* expr = (tree_t*) calloc(1, sizeof(tree_t));
    if (expr == nullptr)
    {
        error->code = (int) TreeErrors::ALLOCATE_MEMORY;
        error->data = "TREE STRUCT";
        return nullptr;
    }

    TreeCtor(expr, error);
    if (error->code != (int) TreeErrors::NONE)
        return nullptr;

    return expr;
}


//-----------------------------------------------------------------------------------------------------

void TreeDtor(tree_t* expr)
{
    DestructNodes(expr->root);

    NametableDtor(&(expr->names));

    expr->root = nullptr;
}

//-----------------------------------------------------------------------------------------------------

void DestructNodes(Node* root)
{
    if (!root) return;

    DestructNodes(root->left);
    DestructNodes(root->right);

    NodeDtor(root);
}

//-----------------------------------------------------------------------------------------------------

int PrintTreeError(FILE* fp, const void* err, const char* func, const char* file, const int line)
{
    assert(err);

    LOG_START(func, file, line);

    const struct ErrorInfo* error = (const struct ErrorInfo*) err;

    switch ((TreeErrors) error->code)
    {
        case (TreeErrors::NONE):
            LOG_END();
            return (int) error->code;

        case (TreeErrors::ALLOCATE_MEMORY):
            fprintf(fp, "CAN NOT ALLOCATE MEMORY FOR %s<br>\n", (const char*) error->data);
            LOG_END();
            return (int) error->code;

        case (TreeErrors::NO_EXPRESSION):
            fprintf(fp, "EXPRESSION TREE IS EMPTY<br>\n");
            LOG_END();
            return (int) error->code;

        case (TreeErrors::INVALID_SYNTAX):
            fprintf(fp, "INVALID SYNTAX<br>\n");
            LOG_END();
            return (int) error->code;

        case (TreeErrors::CYCLED_NODE):
            fprintf(fp, "NODE ID IT'S OWN PREDECESSOR<br>\n");
            LOG_END();
            return (int) error->code;

        case (TreeErrors::COMMON_HEIR):
            fprintf(fp, "NODE'S HEIRS ARE SAME<br>\n");
            LOG_END();
            return (int) error->code;

        case (TreeErrors::INVALID_EXPRESSION_FORMAT):
            fprintf(fp, "EXPRESSION FORMAT IS WRONG<br>\n");
            LOG_END();
            return (int) error->code;

        case (TreeErrors::UNKNOWN_OPERATION):
            fprintf(fp, "UNKNOWN OPERATION<br>\n");
            LOG_END();
            return (int) error->code;

        case (TreeErrors::UNKNOWN):
        // fall through
        default:
            fprintf(fp, "UNKNOWN ERROR WITH EXPRESSION<br>\n");
            LOG_END();
            return (int) TreeErrors::UNKNOWN;
    }
}

//-----------------------------------------------------------------------------------------------------

TreeErrors NodeVerify(const Node* node, error_t* error)
{
    assert(node);
    assert(error);

    if (node == node->left || node == node->right)
    {
        error->code = (int) TreeErrors::CYCLED_NODE;
        error->data = node;
        return TreeErrors::CYCLED_NODE;
    }
    if (node->left == node->right)
    {
        error->code = (int) TreeErrors::COMMON_HEIR;
        error->data = node;
        return TreeErrors::COMMON_HEIR;
    }

    return TreeErrors::NONE;
}

//-----------------------------------------------------------------------------------------------------

TreeErrors TreeVerify(const tree_t* expr, error_t* error)
{
    assert(expr);
    assert(error);

    VerifyNodes(expr->root, error);

    return (TreeErrors) error->code;
}

//-----------------------------------------------------------------------------------------------------

static TreeErrors VerifyNodes(const Node* node, error_t* error)
{
    assert(node);
    assert(error);

    if (node->left != nullptr)
    {
        NodeVerify(node->left, error);
        RETURN_IF_TREE_ERROR((TreeErrors) error->code);
    }

    if (node->right != nullptr)
    {
        NodeVerify(node->right, error);
        RETURN_IF_TREE_ERROR((TreeErrors) error->code);
    }

    NodeVerify(node, error);

    return (TreeErrors) error->code;
}

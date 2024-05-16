#ifndef _MIDDLEEND_H_
#define _MIDDLEEND_H_

#include "tree/tree.h"

void OptimizeTree(tree_t* tree, error_t* error);

enum class MiddleendErrors
{
    NONE,

    UNKNOWN_OPERATION,

    INVALID_TREE,

    UNKNOWN,
};

#endif

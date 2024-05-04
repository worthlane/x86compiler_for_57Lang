#ifndef _BACKEND_H_
#define _BACKEND_H_

#include "tree/tree.h"
#include "common/errors.h"
#include "ir.h"
#include "list.h"

void MoveTreeToIR(const tree_t* tree, ir_t* ir, list_t* labels, error_t* error);
void FillList(const tree_t* tree, list_t* labels, error_t* error);

enum class BackendErrors
{
    NONE,

    NON_INIT_VAR,

    UNKNOWN,
};

#endif

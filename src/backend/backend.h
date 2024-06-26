#ifndef _BACKEND_H_
#define _BACKEND_H_

#include "tree/tree.h"
#include "common/errors.h"
#include "compiler/ir.h"
#include "compiler/x86-64/x86_encode.h"

void MoveTreeToIR(const tree_t* tree, ir_t* ir, nametable_t* labels, error_t* error);
void FillLabelsTable(const tree_t* tree, nametable_t* labels, error_t* error);

enum class BackendErrors
{
    NONE,

    NON_INIT_VAR,

    UNKNOWN,
};

#endif

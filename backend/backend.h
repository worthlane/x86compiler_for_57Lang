#ifndef _BACKEND_H_
#define _BACKEND_H_

#include "tree/tree.h"
#include "common/errors.h"
#include "ir.h"
#include "labels_table.h"

void MoveTreeToIR(const tree_t* tree, ir_t* ir, labels_table_t* labels, error_t* error);
void FillLabelsTable(const tree_t* tree, labels_table_t* labels, error_t* error);

enum class BackendErrors
{
    NONE,

    NON_INIT_VAR,

    UNKNOWN,
};

#endif

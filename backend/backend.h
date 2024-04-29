#ifndef _BACKEND_H_
#define _BACKEND_H_

#include "tree/tree.h"
#include "common/errors.h"

void TranslateToAsm(const tree_t* tree, FILE* out_stream, error_t* error);

enum class BackendErrors
{
    NONE,

    NON_INIT_VAR,

    UNKNOWN,
};

#endif

/*! \file
* \brief Contains all info about typed
*/

#ifndef _TYPES_H_
#define _TYPES_H_

#include <math.h>
#include <assert.h>

/// stack type
typedef struct Stack Stack_t;

/// hash type
typedef unsigned int hash_t;
/// hash function type
typedef hash_t (*hash_f) (const void* obj, size_t size);
/// canary type
typedef long long unsigned int canary_t;
/// dump function
typedef int (*dump_f)(FILE*, const void*, const char*, const char*, const int);

static const double POISON = 0xDEC0;

typedef int code_t;

#define PRINT_ELEM_T "[%p]"

#endif


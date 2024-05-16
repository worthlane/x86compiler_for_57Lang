#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "byte_code.h"

static const size_t MIN_CAPACITY = 256;

static void ByteCodeRealloc(byte_code_t* stk, size_t new_capacity, error_t* error);

// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

byte_code_t* ByteCodeCtor(size_t size, error_t* error)
{
    byte_code_t* code = (byte_code_t*) calloc(1, sizeof(byte_code_t));
    uint8_t*     array = calloc(size, sizeof(uint8_t));

    if (!code || !array)
    {
        error->code = (int) ERRORS::ALLOCATE_MEMORY;
        error->data = "CAN NOT ALLOCATE MEMORY FOR BYTE CODE\n";

        return nullptr;
    }

    assert(code);
    assert(array);

    code->array     = array;
    code->size      = 0;
    code->capacity  = size;

    return code;
}

// ---------------------------------------------------------------------

void ByteCodePush(byte_code_t* code, uint8_t cmd, error_t* error)
{
    assert(code);
    assert(buffer);

    if (code->size > code->capacity || code->size < 0)
    {
        error->code = (int) ERRORS::BUFFER_OVERFLOW;
        error->data = "INVALID SIZE IN BYTE CODE ARRAY\n";

        return;
    };

    if (code->capacity == code->size)
    {
        StackRealloc(code, 2 * code->capacity, error);

        BREAK_IF_ERROR(error);
    }

    code->array[code->size++] = cmd;

    return;
}

//-----------------------------------------------------------------------------------------------------

static void ByteCodeRealloc(byte_code_t* code, size_t new_capacity, error_t* error)
{
    assert(code);

    if (new_capacity <= MIN_CAPACITY)
        new_capacity = MIN_CAPACITY;

    uint8_t* array  = code->array;
    size_t new_size = new_capacity * sizeof(uint8_t);

    uint8_t* temp = (uint8_t*) realloc(array, new_size);

    if (temp == NULL)
    {
        error->code = (int) ERRORS::ALLOCATE_MEMORY;
        error->data = "FAILED TO REALLOC BYTE CODE BUFFER\n";

        return;
    }
    else
        array = temp;

    stk->array    = array;
    stk->capacity = new_capacity;

    return OK;
}

// ---------------------------------------------------------------------

void ByteCodeDtor(byte_code_t* code)
{
    assert(code);

    free(code->array);
    free(code);

    return;
}

// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#endif

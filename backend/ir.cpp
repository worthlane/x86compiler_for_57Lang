#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "ir.h"

static const size_t MIN_CAPACITY = 128;

static void IRRealloc(ir_t* ir, size_t new_capacity, error_t* error);

static inline int PrintWithTabs(FILE* fp, const size_t tabs_amt, const char *format, ...)
{
    for (size_t i = 0; i < tabs_amt; i++)
    {
        fprintf(fp, "\t");
    }

    va_list arg;
    int done;

    va_start (arg, format);
    done = vfprintf(fp, format, arg);
    va_end (arg);

    return done;
}

// ---------------------------------------------------------------

ir_t* IRCtor(const size_t size, error_t* error)
{
    assert(error);

    ir_t* ir = (ir_t*) calloc(1, sizeof(ir_t));
    if (ir == nullptr)
    {
        error->code = (int) ERRORS::ALLOCATE_MEMORY;
        error->data = "IR ALLOCATION";
        return nullptr;
    }

    instruction_t* array = (instruction_t*) calloc(size, sizeof(instruction_t));
    if (array == nullptr)
    {
        error->code = (int) ERRORS::ALLOCATE_MEMORY;
        error->data = "INSTRUCTIONS ALLOCATION";
        return nullptr;
    }

    ir->size  = 0;
    ir->cap   = size;
    ir->array = array;

    return ir;
}

// ---------------------------------------------------------------

void IRDtor(ir_t* ir)
{
    free(ir->array);

    free(ir);
}

//-----------------------------------------------------------------------------------------------------

static void IRRealloc(ir_t* ir, size_t new_capacity, error_t* error)
{
    assert(ir);

    if (new_capacity < MIN_CAPACITY)
        new_capacity = MIN_CAPACITY;

    instruction_t* array   = ir->array;
    size_t         new_cap = sizeof(instruction_t) * new_capacity;

    instruction_t* temp = (instruction_t*) realloc(array, new_cap);

    if (temp == nullptr)
    {
        IRDtor(ir);

        error->code = (int) ERRORS::ALLOCATE_MEMORY;
        error->data = "IR REALLOC";

        return;
    }

    ir->array = temp;
    ir->cap   = new_capacity;
}

// ---------------------------------------------------------------

void IRInsert(ir_t* ir, const instruction_t* instr, error_t* error)
{
    assert(ir);
    assert(error);

    if (ir->size >= ir->cap)
    {
        IRRealloc(ir, ir->cap * 2, error);
        BREAK_IF_ERROR(error);
    }

    size_t cur_instr = ir->size;

    ir->array[cur_instr] = *instr;

    ir->size++;
}

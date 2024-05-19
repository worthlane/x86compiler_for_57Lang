#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <math.h>

#include "ir.h"

static const size_t MIN_CAPACITY = 128;

static void IRRealloc(ir_t* ir, size_t new_capacity, error_t* error);

static void OptimizePushPop(ir_t* ir, const int i, error_t* error);

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

    if (size == FAKE_IR_CAP)
    {
        ir->size  = 0;
        ir->cap   = FAKE_IR_CAP;
        ir->array = NULL;
        return ir;
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
    if (!FakeIR(ir))
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
    if (FakeIR(ir))   // FAKE IR
    {
        ir->size++;
        return;
    }

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

// ---------------------------------------------------------------

#define DEF_CMD(name, ...)   \
            case InstructionCode::ID_##name:                       \
                fprintf(fp, #name);                                 \
                break;

void IRDump(FILE* fp, ir_t* ir)
{
    if (FakeIR(ir))   // FAKE IR
    {
        fprintf(fp, "FAKE IR (%lu)\n", ir->size);
        return;
    }

    for (size_t i = 0; i < ir->size; i++)
    {
        instruction_t elem = ir->array[i];

        fprintf(fp, "[%lu] (%d) { ", i, elem.address);

        switch (elem.code)
        {
            #include "instructions.h"

            /*default*/

            fprintf(fp, "UNKNOWN_INSTR");
        }

        fprintf(fp, " patch: %d; refer_to %lu; arg1: ", elem.need_patch, elem.refer_to);
        DumpArgument(fp, elem.type1, elem.arg1);
        fprintf(fp, " arg2: ");
        DumpArgument(fp, elem.type2, elem.arg2);
        fprintf(fp, "}\n");
    }
}

#undef DEF_CMD

// ---------------------------------------------------------------

void DumpArgument(FILE* fp, const ArgumentType type, const int data)
{
    switch (type)
    {
        case (ArgumentType::NUM):
            fprintf(fp, "%d", data);
            break;
        case (ArgumentType::REGISTER):
        {
            const char* reg = "UNKNOWN REGISTER";

            if (data >= 0 && data < REG_AMT)
                reg = REGISTERS_STR[data];

            fprintf(fp, "%s", reg);
            break;
        }
        case (ArgumentType::RAM):
        {
            char sign = '+';

            if (data < 0)
                sign = '-';

            fprintf(fp, "[rbp %c %d]", sign, abs(data));
            break;
        }
        case (ArgumentType::NONE):
            fprintf(fp, "(NO ARGUMENT)");
            break;
        default:
            fprintf(fp, "(UNKNOWN ARGUMENT)");
    }
}

// ---------------------------------------------------------------

void DumpRAMArgument(FILE* fp, const int data)
{
    char sign = '+';

    if (data < 0)
        sign = '-';

    fprintf(fp, "[rbp %c %d]", sign, abs(data));
}

// ---------------------------------------------------------------

bool IsRegister(const int reg)
{
    if (reg >= 0 && reg < REG_AMT)
        return true;
    else
        return false;
}

// ---------------------------------------------------------------

bool FakeIR(const ir_t* ir)
{
    return (ir->cap == FAKE_IR_CAP) ? true : false;
}

// ---------------------------------------------------------------

void OptimizeIR(ir_t* ir, error_t* error)
{
    assert(ir);

    for (size_t i = 0; i < ir->size; i++)
    {
        OptimizePushPop(ir, i, error);
    }
}

// ---------------------------------------------------------------

static void OptimizePushPop(ir_t* ir, const int i, error_t* error)
{
    assert(ir);

    instruction_t instr1 = ir->array[i];
    instruction_t instr2 = ir->array[i + 1];

    if ((instr1.type1 != instr2.type1) || (instr1.arg1 != instr2.arg1))
        return;

    InstructionCode code1 = instr1.code;
    InstructionCode code2 = instr2.code;

    if ((code1 == InstructionCode::ID_PUSH     && code2 == InstructionCode::ID_POP)      ||
        (code1 == InstructionCode::ID_PUSH_XMM && code2 == InstructionCode::ID_POP_XMM)  ||
        (code1 == InstructionCode::ID_POP      && code2 == InstructionCode::ID_PUSH)     ||
        (code1 == InstructionCode::ID_POP_XMM  && code2 == InstructionCode::ID_PUSH_XMM))
    {
        instruction_t nop = {.code = InstructionCode::ID_NOP};

        ir->array[i]     = nop;
        ir->array[i + 1] = nop;
    }
}

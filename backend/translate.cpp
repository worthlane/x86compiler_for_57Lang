#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#include "translate.h"
#include "common/input_and_output.h"

void TranslateIrToX86(const char* file_name, ir_t* ir, error_t* error)
{
    FillIRAddrX86(ir, error);
    BREAK_IF_ERROR(error);

    PatchIRX86(ir, error);
    BREAK_IF_ERROR(error);

    FILE* out_stream = OpenFile(TREE_FILE, "w", error);
    BREAK_IF_ERROR(error);

    DumpIRtoX86(out_stream, ir, error);
    BREAK_IF_ERROR(error);

    fclose(out_stream)
}

// -----------------------------------------------------------------------------

#define DEF_CMD(name, ...) \


static void FillIRAddrX86(ir_t* ir, error_t* error)
{
    assert(ir);

    if (ir->cap = FAKE_IR_CAP)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "CAN NOT TRANSLATE FAKE IR TO X86\n");
        return;
    }

    int address = 0;

    for (size_t i = 0; i < ir->size; i++)
    {
        instruction_t instr = ir->array[i];

        switch (instr.code)
        {
            #include "instructions.h"

            default:
                error->code = (int) ERRORS::INVALID_IR;
                SetErrorData(error, "UNKNOWN CODE INSTRUCTION IN (%lu) CELL\n", i);
                return;
        }
    }
}

#undef DEF_CMD

// -----------------------------------------------------------------------------

static void PatchIRX86(ir_t* ir, error_t* error)
{
    assert(ir);

    if (ir->cap = FAKE_IR_CAP)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "CAN NOT TRANSLATE FAKE IR TO X86\n");
        return;
    }

    for (size_t i = 0; i < ir->size; i++)
    {
        instruction_t instr = ir->array[i];

        if (instr.need_patch)
        {
            instr.type1 = ArgumentType::NUM;
            instr.arg1  = ir->array[instr.refet_to].address;
        }
    }
}

// -----------------------------------------------------------------------------

#define DEF_CMD(name, asm)                                      \
        case InstructionCode::ID_##name:                        \
            asm


static void DumpIRtoX86(FILE* out_stream, ir_t* ir, error_t* error)
{
    assert(ir);

    if (ir->cap = FAKE_IR_CAP)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "CAN NOT TRANSLATE FAKE IR TO X86\n");
        return;
    }

    int address = 0;

    for (size_t i = 0; i < ir->size; i++)
    {
        instruction_t instr = ir->array[i];

        switch (instr.code)
        {
            #include "instructions.h"

            default:
                error->code = (int) ERRORS::INVALID_IR;
                SetErrorData(error, "UNKNOWN CODE INSTRUCTION IN (%lu) CELL\n", i);
                return;
        }
    }
}

#undef DEF_CMD

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#include "x86_asm.h"
#include "x86_encode.h"
#include "common/input_and_output.h"
#include "byte_code.h"
#include "stack/stack.h"

static const char* STD_LIBRARY_PATH = "std_lib/stdlib.s";
static const size_t MAX_LABEL_LEN = 20;

static const size_t TABS_AMT = 2;

static void PatchIRX86(ir_t* ir, error_t* error);
static void FillIRAddrX86(ir_t* ir, nametable_t* labels, error_t* error);
static void DumpIRtoX86(FILE* out_stream, ir_t* ir, nametable_t* labels, error_t* error);

static inline int PrintWithTabs(FILE* fp, const char *format, ...)
{
    for (size_t i = 0; i < TABS_AMT; i++)
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

// ------------------------------------------------------

static inline void DumpHeader(FILE* out_stream)
{
    fprintf(out_stream,  "%%include \"%s\"\n\n"
                                        "global _start\n"
                                        "_start:\n\n", STD_LIBRARY_PATH);
}

// ------------------------------------------------------

static inline void DumpInclude(FILE* out_stream)
{
    fprintf(out_stream, "\n%%include \"%s\"\n", STD_LIBRARY_PATH);
}

// ------------------------------------------------------

void TranslateIrToX86(const char* file_name, ir_t* ir, bool need_dump, error_t* error)
{
    nametable_t* labels = MakeNametable();

    FillIRAddrX86(ir, labels, error);
    BREAK_IF_ERROR(error);

    PatchIRX86(ir, error);
    BREAK_IF_ERROR(error);

    if (!need_dump)
    {
        NametableDtor(labels);
        return;
    }

    FILE* out_stream = OpenFile(file_name, "w", error);
    BREAK_IF_ERROR(error);

    DumpIRtoX86(out_stream, ir, labels, error);

    fclose(out_stream);
    NametableDtor(labels);
}

// -----------------------------------------------------------------------------

#define DEF_CMD(name, asm, size_upd, ...) \
        case InstructionCode::ID_##name:                        \
            size_upd                                                 \
            break;

static void FillIRAddrX86(ir_t* ir, nametable_t* labels, error_t* error)
{
    assert(ir);
    assert(labels);

    if (FakeIR(ir))
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "CAN NOT TRANSLATE FAKE IR TO X86\n");
        return;
    }

    int label_cnt = 0;

    byte_code_t* program_code = ByteCodeCtor(FAKE_BYTECODE_CAP, error);

    for (size_t i = 0; i < ir->size; i++)
    {
        instruction_t instr = ir->array[i];

        ir->array[i].address = program_code->size;

        switch (instr.code)
        {
            #include "instructions.h"

            default:
                error->code = (int) ERRORS::INVALID_IR;
                SetErrorData(error, "UNKNOWN CODE INSTRUCTION IN (%lu) CELL\n", i);
                return;
        }

        if (instr.refer_to)
        {
            char label[MAX_LABEL_LEN] = "";
            char* table_label = GetNameFromNameTable(labels, instr.refer_to);

            if (!table_label)
            {
                snprintf(label, MAX_LABEL_LEN, ".L%d", label_cnt++);
                InsertPairInTable(labels, label, instr.refer_to);
            }
        }
    }

    ByteCodeDtor(program_code);
}

#undef DEF_CMD

// -----------------------------------------------------------------------------

static void PatchIRX86(ir_t* ir, error_t* error)
{
    assert(ir);

    if (FakeIR(ir))
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
            if (instr.type1 == ArgumentType::NUM)
            {
                ir->array[i].arg1  = instr.arg1 - instr.address - 1;
            }
            else
            {
                ir->array[i].type1 = ArgumentType::NUM;

                if (instr.code == InstructionCode::ID_CALL || instr.code == InstructionCode::ID_JMP)
                    ir->array[i].arg1  = ir->array[instr.refer_to].address - instr.address - 1;
                else
                    ir->array[i].arg1  = ir->array[instr.refer_to].address - instr.address - 2;
            }
        }
    }
}

// -----------------------------------------------------------------------------

#define DEF_CMD(name, asm, ...)                                      \
        case InstructionCode::ID_##name:                        \
            asm                                                 \
            break;


static void DumpIRtoX86(FILE* out_stream, ir_t* ir, nametable_t* labels, error_t* error)
{
    assert(ir);
    assert(labels);

    if (FakeIR(ir))
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "CAN NOT TRANSLATE FAKE IR TO X86\n");
        return;
    }

    DumpHeader(out_stream);

    for (size_t i = 0; i < ir->size; i++)
    {
        instruction_t instr = ir->array[i];

        char* table_label = GetNameFromNameTable(labels, i);
        if (table_label)
            fprintf(out_stream, "%s:\n", table_label);

        switch (instr.code)
        {
            #include "instructions.h"

            default:
                error->code = (int) ERRORS::INVALID_IR;
                SetErrorData(error, "UNKNOWN CODE INSTRUCTION IN (%lu) CELL\n", i);
                return;
        }

        fprintf(out_stream, "\n");
    }

    //DumpInclude(out_stream);
}

#undef DEF_CMD



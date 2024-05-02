#ifndef _IR_H_
#define _IR_H_

#include "stack/nametable.h"
#include "common/errors.h"

#define DEF_CMD(name, ...) \
          ID_##name,

enum class InstructionCode
{
    #include "instructions.h"
};

#undef DEF_CMD

enum Registers
{
    RAX,
    RBX,
    RCX,
    RDX,
    RBP,
    RSI,
    RDI,
    RSP,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15,

    XMM0,
    XMM1,
    XMM2,
    XMM3,
    XMM4,
    XMM5,
    XMM6,
    XMM7
};

enum class ArgumentType
{
    NONE = 0,

    VALUE,
    REGISTER,
    RAM,
};

struct instruction_t
{
    InstructionCode code;

    bool   need_patch;
    size_t refer_to;

    size_t x86size;

    ArgumentType type1;
    int          arg1;

    ArgumentType type2;
    int          arg2;
};

struct ir_t
{
    instruction_t* array;

    size_t size;
    size_t cap;
};

ir_t*   IRCtor(const size_t size, error_t* error);
void    IRDtor(ir_t* ir);

void    IRInsert(ir_t* ir, const instruction_t* instr, error_t* error);



#endif

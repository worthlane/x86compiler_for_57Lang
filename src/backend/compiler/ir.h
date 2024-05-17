#ifndef _IR_H_
#define _IR_H_

#include "stack/nametable.h"
#include "common/errors.h"

static const size_t FAKE_IR_CAP = 0;

#define DEF_CMD(name, ...) \
          ID_##name,

enum class InstructionCode
{
    #include "instructions.h"
};

#undef DEF_CMD

enum Registers
{
    RAX = 0,
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
    XMM7,

    REG_AMT
};

static const char* REGISTERS_STR[] = {"rax", "rbx", "rcx", "rdx", "rbp", "rsi", "rdi", "rsp",
                                     "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15",
                                     "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7"};

bool IsRegister(const int reg);

enum class ArgumentType
{
    NONE = 0,

    NUM,
    REGISTER,
    RAM,
};

void DumpArgument(FILE* fp, const ArgumentType type, const int data);
void DumpRAMArgument(FILE* fp, const int data);

struct instruction_t
{
    int address;

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

void IRDump(FILE* fp, ir_t* ir);

bool FakeIR(const ir_t* ir);



#endif

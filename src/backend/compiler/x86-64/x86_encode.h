#ifndef _X86_ENCODE_H_
#define _X86_ENCODE_H_

#include <stdio.h>
#include "../byte_code.h"
#include "../ir.h"

static const int START_ADDR = 0x401204;
static const int HLT_ADDR   = 0x4011f7 - START_ADDR;
static const int IN_ADDR    = 0x4010d2 - START_ADDR;
static const int OUT_ADDR   = 0x401000 - START_ADDR;

enum IntegrerRegisters
{
    INT_RAX = 0b0000,
    INT_RCX = 0b0001,
    INT_RDX = 0b0010,
    INT_RBX = 0b0011,
    INT_RSP = 0b0100,
    INT_RBP = 0b0101,
    INT_RSI = 0b0110,
    INT_RDI = 0b0111,

    INT_R8  = 0b1000,
    INT_R9  = 0b1001,
    INT_R10 = 0b1010,
    INT_R11 = 0b1011,
    INT_R12 = 0b1100,
    INT_R13 = 0b1101,
    INT_R14 = 0b1110,
    INT_R15 = 0b1111,
};

enum DoubleRegisters
{
    DBL_XMM0  = 0b0000,
    DBL_XMM1  = 0b0001,
    DBL_XMM2  = 0b0010,
    DBL_XMM3  = 0b0011,
    DBL_XMM4  = 0b0100,
    DBL_XMM5  = 0b0101,
    DBL_XMM6  = 0b0110,
    DBL_XMM7  = 0b0111,

    DBL_XMM8  = 0b1000,
    DBL_XMM9  = 0b1001,
    DBL_XMM10 = 0b1010,
    DBL_XMM11 = 0b1011,
    DBL_XMM12 = 0b1100,
    DBL_XMM13 = 0b1101,
    DBL_XMM14 = 0b1110,
    DBL_XMM15 = 0b1111,
};

static const size_t NUM_OPERAND_LEN = 4;

enum Opcodes
{
    PUSH_POP_REG = 0x5,

    CALL = 0xE8,
    JMP  = 0xE9,
    RET  = 0xC3,

    SUB  = 0x5C,
    ADD  = 0x58,
    MUL  = 0x59,
    DIV  = 0x5E,
    SQRT = 0x51,
    CMP  = 0x2E,

    PUSH_XMM = 0x11,
    POP_XMM  = 0x10,

    REG_MOV = 0x89,

    JA   = 0x0f87,
    JAE  = 0x0f83,
    JB   = 0x0f82,
    JBE  = 0x0f86,
    JNE  = 0x0f85,
    JE   = 0x0f84,
};

int EncodeIRRegister(const int reg);

void EncodeSQRT(byte_code_t* program_code, const int imm1, const int imm2, error_t* error);

void EncodePushPopREG(byte_code_t* program_code, const int reg, const InstructionCode instr, error_t* error);

void EncodeXMMArithm(byte_code_t* program_code, const int imm1, const int imm2, const int imm3,
                    const InstructionCode instr,  error_t* error);

void EncodeCMP(byte_code_t* program_code, const int xmm1, const int xmm2,  error_t* error);

void EncodeMOV_REG_REG(byte_code_t* program_code, const int dest, const int src, error_t* error);
void EncodeMOV_REG_NUM(byte_code_t* program_code, const int dest, const int num, error_t* error);

void EncodePushPopXMM(byte_code_t* program_code, const int xmm, const InstructionCode instr, error_t* error);
void EncodeMOV_RBX_to_XMM5(byte_code_t* program_code, error_t* error);

void EncodeMOV_RAM(byte_code_t* program_code, const int xmm, const uint8_t shift, bool to_ram, error_t* error);

#endif

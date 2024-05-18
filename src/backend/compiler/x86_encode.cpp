#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "x86_encode.h"

static uint8_t  GetMODRM(const int prefix, const int xmm, const int reg);
static uint16_t GetVEX_TwoByte(const int xmm2, const int L, const int pp);
static uint8_t  GetOpcodeFromInstr(const InstructionCode instr);
static uint8_t  GetREX_Prefix(const int dest, const int src, const int W, const int X);

// -----------------------------------------------------------------------------------

int EncodeIRRegister(const int reg)
{
    switch (reg)
    {
        case (Registers::RAX):      return IntegrerRegisters::INT_RAX;
        case (Registers::RBX):      return IntegrerRegisters::INT_RBX;
        case (Registers::RCX):      return IntegrerRegisters::INT_RCX;
        case (Registers::RDX):      return IntegrerRegisters::INT_RDX;
        case (Registers::RBP):      return IntegrerRegisters::INT_RBP;
        case (Registers::RSI):      return IntegrerRegisters::INT_RSI;
        case (Registers::RDI):      return IntegrerRegisters::INT_RDI;
        case (Registers::RSP):      return IntegrerRegisters::INT_RSP;
        case (Registers::R8):       return IntegrerRegisters::INT_R8;
        case (Registers::R9):       return IntegrerRegisters::INT_R9;
        case (Registers::R10):      return IntegrerRegisters::INT_R10;
        case (Registers::R11):      return IntegrerRegisters::INT_R11;
        case (Registers::R12):      return IntegrerRegisters::INT_R12;
        case (Registers::R13):      return IntegrerRegisters::INT_R13;
        case (Registers::R14):      return IntegrerRegisters::INT_R14;
        case (Registers::R15):      return IntegrerRegisters::INT_R15;

        case (Registers::XMM0):     return DoubleRegisters::DBL_XMM0;
        case (Registers::XMM1):     return DoubleRegisters::DBL_XMM1;
        case (Registers::XMM2):     return DoubleRegisters::DBL_XMM2;
        case (Registers::XMM3):     return DoubleRegisters::DBL_XMM3;
        case (Registers::XMM4):     return DoubleRegisters::DBL_XMM4;
        case (Registers::XMM5):     return DoubleRegisters::DBL_XMM5;
        case (Registers::XMM6):     return DoubleRegisters::DBL_XMM6;
        case (Registers::XMM7):     return DoubleRegisters::DBL_XMM7;
        default:
            return 0;
    }
}

// -----------------------------------------------------------------------------------

void EncodePushPopREG(byte_code_t* program_code, const int reg, const InstructionCode instr, error_t* error)
{
    static const int POP_MASK = 0b1000;
    static const int BIG_REG  = 0x41;

    int mask = 0;

    if (instr == InstructionCode::ID_POP)
        mask = POP_MASK;
    else if (instr != InstructionCode::ID_PUSH)
    {
        error->code = (int) ERRORS::INVALID_IR;
        error->data = "INVALID PUSH/POP REGISTER INSTRUCTION\n";
    }

    if (reg >= Registers::R8 && reg <= Registers::R15)
    {
        ByteCodePush(program_code, BIG_REG, error);
        mask ^= POP_MASK;
    }

    int encode_reg = EncodeIRRegister(reg);

    uint16_t cmd = (Opcodes::PUSH_POP_REG << 4) | (encode_reg ^ mask);
    ByteCodePush(program_code, cmd, error);
}

// -----------------------------------------------------------------------------------

void EncodeXMMArithm(byte_code_t* program_code, const int xmm1, const int xmm2, const int xmm3,
                    const InstructionCode instr,  error_t* error)
{
    uint16_t vex   = GetVEX_TwoByte(xmm2, 0, 0b10);
    uint8_t  modrm = GetMODRM(0b11, xmm1, xmm3);

    PushDoubleByte(program_code, vex, error);

    uint8_t opcode = GetOpcodeFromInstr(instr);

    ByteCodePush(program_code, opcode, error);
    ByteCodePush(program_code, modrm, error);
}

// -----------------------------------------------------------------------------------

void EncodeCMP(byte_code_t* program_code, const int xmm1, const int xmm2, error_t* error)
{
    uint8_t  modrm = GetMODRM(0b11, xmm1, xmm2);
    uint16_t vex   = 0xC5F8;

    PushDoubleByte(program_code, vex, error);

    uint8_t opcode = Opcodes::CMP;

    ByteCodePush(program_code, opcode, error);
    ByteCodePush(program_code, modrm, error);
}

// -----------------------------------------------------------------------------------

void EncodeSQRT(byte_code_t* program_code, const int xmm1, const int xmm2, error_t* error)
{
    uint16_t vex   = GetVEX_TwoByte(xmm1, 0, 0b10);
    uint8_t  modrm = GetMODRM(0b11, xmm1, xmm2);

    PushDoubleByte(program_code, vex, error);

    uint8_t opcode = Opcodes::SQRT;

    ByteCodePush(program_code, opcode, error);
    ByteCodePush(program_code, modrm, error);
}

// -----------------------------------------------------------------------------------

void EncodeMOV_REG_REG(byte_code_t* program_code, const int dest, const int src, error_t* error)
{
    uint8_t rex   = GetREX_Prefix(dest, src, 1, 0);
    uint8_t modrm = GetMODRM(0b11, src, dest);

    uint8_t opcode = Opcodes::REG_MOV;

    ByteCodePush(program_code, rex, error);
    ByteCodePush(program_code, opcode, error);
    ByteCodePush(program_code, modrm, error);
}

// -----------------------------------------------------------------------------------

void EncodePushPopXMM(byte_code_t* program_code, const int xmm, const InstructionCode instr, error_t* error)
{
    uint16_t vex   = 0xC5FA;
    uint8_t  modrm = GetMODRM(0b00, xmm, Registers::RSP);

    uint8_t opcode = GetOpcodeFromInstr(instr);

    if (opcode != Opcodes::POP_XMM && opcode != Opcodes::PUSH_XMM)
    {
        error->code = (int) ERRORS::INVALID_IR;
        error->data = "INVALID PUSH/POP XMM REGISTER INSTRUCTION\n";
    }

    if (opcode == Opcodes::PUSH_XMM)
    {   // sub rsp, 16
        ByteCodePush(program_code, 0x48, error);
        ByteCodePush(program_code, 0x83, error);
        ByteCodePush(program_code, 0xEC, error);
        ByteCodePush(program_code, 0x10, error);
    }

    PushDoubleByte(program_code, vex, error);
    ByteCodePush(program_code, opcode, error);
    ByteCodePush(program_code, modrm, error);
    ByteCodePush(program_code, 0x24, error);

    if (opcode == Opcodes::POP_XMM)
    {   // add rsp, 16
        ByteCodePush(program_code, 0x48, error);
        ByteCodePush(program_code, 0x83, error);
        ByteCodePush(program_code, 0xC4, error);
        ByteCodePush(program_code, 0x10, error);
    }
}

// -----------------------------------------------------------------------------------

void EncodeMOV_RAM(byte_code_t* program_code, const int xmm, const uint8_t shift, bool to_ram, error_t* error)
{
    uint16_t vex   = 0xC5FA;
    uint8_t  modrm = GetMODRM(0b01, xmm, Registers::RBP);

    uint8_t opcode = 0;

    if (to_ram)
        opcode = Opcodes::PUSH_XMM;
    else
        opcode = Opcodes::POP_XMM;

    PushDoubleByte(program_code, vex, error);
    ByteCodePush(program_code, opcode, error);
    ByteCodePush(program_code, modrm, error);
    ByteCodePush(program_code, shift, error);
}

// -----------------------------------------------------------------------------------

void EncodeMOV_RBX_to_XMM5(byte_code_t* program_code, error_t* error)
{
    ByteCodePush(program_code, 0xC4, error);
    ByteCodePush(program_code, 0xE1, error);
    ByteCodePush(program_code, 0xD2, error);
    ByteCodePush(program_code, 0x2A, error);
    ByteCodePush(program_code, 0xEB, error);
}

// -----------------------------------------------------------------------------------

void EncodeMOV_REG_NUM(byte_code_t* program_code, const int dest, const int num, error_t* error)    // TODO add r8+
{
    static const int MOV_NUM_OPCODE = 0xB8;

    int reg = EncodeIRRegister(dest) & 0b0111;

    ByteCodePush(program_code, MOV_NUM_OPCODE + reg, error);
    PushQuadByte(program_code, num, error);
}

// -----------------------------------------------------------------------------------

static uint8_t GetMODRM(const int prefix, const int xmm, const int reg)
{
    int mm  = EncodeIRRegister(xmm) & 0b0111;
    int rm  = EncodeIRRegister(reg) & 0b0111;

    // MODRM: (prefix)(mm)(rm)

    return (prefix << 6) + (mm << 3) + rm;
}

// -----------------------------------------------------------------------------------

static uint8_t GetREX_Prefix(const int dest, const int src, const int W, const int X)
// for naming understanding visit (https://wiki.osdev.org/X86-64_Instruction_Encoding#Three_byte_VEX_escape_prefix)
{
    int B = (EncodeIRRegister(dest) & 0b1000) >> 3;
    int R = (EncodeIRRegister(src)  & 0b1000) >> 3;

    // PREFIX: 0100WRXB

    return 0b01000000 | (W << 3) | (R << 2) | (X << 1) | B;
}

// -----------------------------------------------------------------------------------

static uint16_t GetVEX_TwoByte(const int xmm, const int L, const int pp)
// for naming understanding visit (https://wiki.osdev.org/X86-64_Instruction_Encoding#Three_byte_VEX_escape_prefix)
{
    static const int TWO_BYTE_VEX = 0xC5;

    int vvvv = (~EncodeIRRegister(xmm)) & 0b1111;
    int R    = 1;       // (inversed 0) we use only xmm0 - xmm7 registers

    // VEX: C5 RvvvvLpp

    return (TWO_BYTE_VEX << 8) | ( (R << 7) | (vvvv << 3) | (L << 2) | pp);
}

// -----------------------------------------------------------------------------------

static uint8_t GetOpcodeFromInstr(const InstructionCode instr)
{
    switch (instr)
    {
        case (InstructionCode::ID_ADD):     return Opcodes::ADD;
        case (InstructionCode::ID_SUB):     return Opcodes::SUB;
        case (InstructionCode::ID_MUL):     return Opcodes::MUL;
        case (InstructionCode::ID_DIV):     return Opcodes::DIV;

        case (InstructionCode::ID_PUSH_XMM):        return Opcodes::PUSH_XMM;
        case (InstructionCode::ID_POP_XMM):         return Opcodes::POP_XMM;

        case (InstructionCode::ID_CMP):     return Opcodes::CMP;
        case (InstructionCode::ID_JMP):     return Opcodes::JMP;
        case (InstructionCode::ID_JA):      return Opcodes::JA;
        case (InstructionCode::ID_JAE):     return Opcodes::JAE;
        case (InstructionCode::ID_JB):      return Opcodes::JB;
        case (InstructionCode::ID_JBE):     return Opcodes::JBE;
        case (InstructionCode::ID_JE):      return Opcodes::JE;
        case (InstructionCode::ID_JNE):     return Opcodes::JNE;
        case (InstructionCode::ID_CALL):    return Opcodes::CALL;

        default:
            return 0;
    }
}


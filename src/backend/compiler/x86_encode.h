#ifndef _X86_ENCODE_H_
#define _X86_ENCODE_H_

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

#endif

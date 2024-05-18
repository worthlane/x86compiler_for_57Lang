#ifndef _BYTE_CODE_H_
#define _BYTE_CODE_H_

#include <stdint.h>

#include "common/errors.h"

struct byte_code_t
{
    size_t size;
    size_t capacity;

    uint8_t* array;
};

static const size_t FAKE_BYTECODE_CAP = 0;

byte_code_t*  ByteCodeCtor(size_t size, error_t* error);
void          ByteCodePush(byte_code_t* code, uint8_t cmd, error_t* error);
void          ByteCodeDtor(byte_code_t* code);

void PushQuadByte(byte_code_t* program_code, const int num, error_t* error);
void PushDoubleByte(byte_code_t* program_code, const int num, error_t* error);

static const size_t MIN_CODE_SIZE = 256;

#endif

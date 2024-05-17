#ifndef _X86_ELF_H_
#define _X86_ELF_H_

#include <elf.h>
#include <fcntl.h>
#include <unistd.h>

#include "ir.h"
#include "byte_code.h"
#include "x86_encode.h"

static const int PROGRAM_HDRS_AMT = 3;
static const int SECTION_HDRS_AMT = 4;
static const int SHSTI_SYS        = 3;

static const int STDLIB_CODE_SIZE = START_ADDR - 0x401000;
static const int STDLIB_VARS_SIZE = 0x40;


void TranslateIrToElf(const char* file_name, ir_t* ir, error_t* error);

#endif

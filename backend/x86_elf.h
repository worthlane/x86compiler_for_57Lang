#ifndef _X86_ELF_H_
#define _X86_ELF_H_

#include <elf.h>
#include <fcntl.h>
#include <unistd.h>

#include "ir.h"
#include "byte_code.h"

void PrintElfHeader(FILE* fp, byte_code_t ProgramCode, error_t* error);

#endif

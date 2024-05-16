#ifndef _X86_ELF_H_
#define _X86_ELF_H_

#include <elf.h>
#include <fcntl.h>
#include <unistd.h>

#include "ir.h"
#include "byte_code.h"

void TranslateIrToElf(const char* file_name, ir_t* ir, error_t* error);

#endif

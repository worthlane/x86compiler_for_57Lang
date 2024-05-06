#ifndef _TRANSLATE_H_
#define _TRANSLATE_H_

#include "ir.h"

void TranslateIrToX86(const char* file_name, ir_t* ir, error_t* error);

#endif

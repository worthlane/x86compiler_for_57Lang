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

struct instruction_t
{
    InstructionCode code;

    bool   need_patch;
    size_t refer_to;

    size_t x86size;
};

struct ir_t
{
    instruction_t* array;

    size_t size;
    size_t cap;
};

ir_t*   IRCtor(const size_t size, error_t* error);
void    IRDtor(ir_t* ir);

void    IRInsert(ir_t* ir, const InstructionCode code, const bool need_patch,
                           const size_t refer_to, error_t* error);



#endif

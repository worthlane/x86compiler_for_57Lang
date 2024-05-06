#ifndef DEF_CMD
#define DEF_CMD(...) ;
#endif

// DEF_CMD(NAME, x86_size)

DEF_CMD(HLT,
{
    fprintf(out_stream, "call _hlt\n");
})

DEF_CMD(OUT,
{
    fprintf(out_stream, "call _out\n");
})

DEF_CMD(IN,
{
    fprintf(out_stream, "call _in\n");
})

DEF_CMD(POP,
{
    if (instr.type1 == ArgumentType::REGISTER || !IsRegister(instr.arg1))
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    fprintf(out_stream, "pop %s", REGISTERS_STR[instr.arg1]);
})

DEF_CMD(PUSH,
{
    if (instr.type1 == ArgumentType::REGISTER)
    {
        if (!IsRegister(instr.arg1))
        {
            error->code = (int) ERRORS::INVALID_IR;
            SetErrorData(error, "INVALID REGISTER CODE IN IR (%lu)\n", i);
            return;
        }

        fprintf(out_stream, "push %s\n", REGISTERS_STR[instr.arg1]);
    }
    else if (instr.type1 == ArgumentType::NUM)
    {
        fprintf(out_stream, "push %d\n", instr.arg1);
    }
    else
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }
})

DEF_CMD(POP_XMM,
{
    if (instr.type1 == ArgumentType::REGISTER)
    {
        if (!IsRegister(instr.arg1))
        {
            error->code = (int) ERRORS::INVALID_IR;
            SetErrorData(error, "INVALID REGISTER CODE IN IR (%lu)\n", i);
            return;
        }

        fprintf(out_stream, "vmovss %s, DQWORD [rsp]\n"
                            "add rsp, 16\n", REGISTERS_STR[instr.arg1]);
    }
    else if (instr.type1 == ArgumentType::NUM)
    {

    }
    else if (instr.type1 == ArgumentType::RAM)
    {

    }
    else
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }
})

DEF_CMD(PUSH_XMM,
{
    if (instr.type1 == ArgumentType::REGISTER)
    {
        if (!IsRegister(instr.arg1))
        {
            error->code = (int) ERRORS::INVALID_IR;
            SetErrorData(error, "INVALID REGISTER CODE IN IR (%lu)\n", i);
            return;
        }

        fprintf(out_stream, "sub rsp, 16\n"
                            "vmovss DQWORD [rsp], %s\n", REGISTERS_STR[instr.arg1]);
    }
    else if (instr.type1 == ArgumentType::NUM)
    {

    }
    else if (instr.type1 == ArgumentType::RAM)
    {

    }
    else
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }
})

DEF_CMD(CMP,
{
    if (instr.type1 != ArgumentType::REGISTER ||  instr.type1 != ArgumentType::REGISTER)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    if (!IsRegister(instr.arg1) || !IsRegister(instr.arg2))
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID REGISTER CODE IN IR (%lu)\n", i);
        return;
    }

    fprintf(out_stream, "vucomiss %s, %s\n",
            REGISTERS_STR[instr.arg2], REGISTERS_STR[instr.arg1]);
})

DEF_CMD(MOV,
{
    if (instr.type1 != ArgumentType::REGISTER || !IsRegister(instr.arg1))
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    if (instr.type2 == ArgumentType::REGISTER)
    {
        if (!IsRegister(instr.arg2))
        {
            error->code = (int) ERRORS::INVALID_IR;
            SetErrorData(error, "INVALID REGISTER CODE IN IR (%lu)\n", i);
            return;
        }

        fprintf(out_stream, "mov %s, %s\n", REGISTERS_STR[instr.arg1], REGISTERS_STR[instr.arg2]);
    }
    else if (instr.type2 == ArgumentType::NUM)
    {
        fprintf(out_stream, "mov %s, %d\n", REGISTERS_STR[instr.arg1], instr.arg2);
    }
    else
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }
})

// ====== ARITHMETICS CMDS =======

DEF_CMD(SUB,
{
    if (instr.type1 != ArgumentType::REGISTER ||  instr.type1 != ArgumentType::REGISTER)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    if (!IsRegister(instr.arg1) || !IsRegister(instr.arg2))
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID REGISTER CODE IN IR (%lu)\n", i);
        return;
    }

    fprintf(out_stream, "vsubss %s, %s, %s\n",
            REGISTERS_STR[instr.arg1], REGISTERS_STR[instr.arg1], REGISTERS_STR[instr.arg2]);
})

DEF_CMD(ADD,
{
    if (instr.type1 != ArgumentType::REGISTER ||  instr.type1 != ArgumentType::REGISTER)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    if (!IsRegister(instr.arg1) || !IsRegister(instr.arg2))
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID REGISTER CODE IN IR (%lu)\n", i);
        return;
    }

    fprintf(out_stream, "vaddss %s, %s, %s\n",
            REGISTERS_STR[instr.arg1], REGISTERS_STR[instr.arg1], REGISTERS_STR[instr.arg2]);
})

DEF_CMD(MUL,
{
    if (instr.type1 != ArgumentType::REGISTER ||  instr.type1 != ArgumentType::REGISTER)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    if (!IsRegister(instr.arg1) || !IsRegister(instr.arg2))
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID REGISTER CODE IN IR (%lu)\n", i);
        return;
    }

    fprintf(out_stream, "vmulss %s, %s, %s\n",
            REGISTERS_STR[instr.arg1], REGISTERS_STR[instr.arg1], REGISTERS_STR[instr.arg2]);
})

DEF_CMD(DIV,
{
    if (instr.type1 != ArgumentType::REGISTER ||  instr.type1 != ArgumentType::REGISTER)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    if (!IsRegister(instr.arg1) || !IsRegister(instr.arg2))
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID REGISTER CODE IN IR (%lu)\n", i);
        return;
    }

    fprintf(out_stream, "vdivss %s, %s, %s\n",
            REGISTERS_STR[instr.arg1], REGISTERS_STR[instr.arg1], REGISTERS_STR[instr.arg2]);
})


DEF_CMD(SQRT,
{

})

// ............. JMP ..............

DEF_CMD(JMP,
{
    if (instr.type1 != ArgumentType::NUM)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    fprintf(out_stream, "jmp %d\n", instr.arg1);
})

DEF_CMD(JA,
{
    if (instr.type1 != ArgumentType::NUM)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    fprintf(out_stream, "ja %d\n", instr.arg1);
})

DEF_CMD(JAE,
{
    if (instr.type1 != ArgumentType::NUM)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    fprintf(out_stream, "jae %d\n", instr.arg1);
})

DEF_CMD(JB,
{
    if (instr.type1 != ArgumentType::NUM)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    fprintf(out_stream, "jb %d\n", instr.arg1);
})

DEF_CMD(JBE,
{
    if (instr.type1 != ArgumentType::NUM)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    fprintf(out_stream, "jbe %d\n", instr.arg1);
})

DEF_CMD(JNE,
{
    if (instr.type1 != ArgumentType::NUM)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    fprintf(out_stream, "jne %d\n", instr.arg1);
})

DEF_CMD(JE,
{
    if (instr.type1 != ArgumentType::NUM)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    fprintf(out_stream, "je %d\n", instr.arg1);
})

DEF_CMD(CALL,
{
    if (instr.type1 != ArgumentType::NUM)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    fprintf(out_stream, "call %d\n", instr.arg1);
})

DEF_CMD(RET,
{
    fprintf(out_stream, "ret\n");
})

// .............................................

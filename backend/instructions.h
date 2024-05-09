#ifndef DEF_CMD
#define DEF_CMD(...) ;
#endif

// DEF_CMD(NAME, x86_size)

DEF_CMD(HLT,
{
    PrintWithTabs(out_stream, "call _hlt\n");
},
{
    address += 5;
})

DEF_CMD(OUT,
{
    PrintWithTabs(out_stream, "call _out\n");
},
{
    address += 5;
})

DEF_CMD(IN,
{
    PrintWithTabs(out_stream, "call _in\n");
},
{
    address += 5;
})

DEF_CMD(POP,
{
    if (instr.type1 != ArgumentType::REGISTER || !IsRegister(instr.arg1))
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    PrintWithTabs(out_stream, "pop %s\n", REGISTERS_STR[instr.arg1]);
},
{
    address += 1;
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

        PrintWithTabs(out_stream, "push %s\n", REGISTERS_STR[instr.arg1]);
    }
    else if (instr.type1 == ArgumentType::NUM)
    {
        PrintWithTabs(out_stream, "push %d\n", instr.arg1);
    }
    else
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }
},
{
    if (instr.type1 == ArgumentType::REGISTER)
        address += 1;
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

        PrintWithTabs(out_stream, "vmovss %s, [rsp]\n", REGISTERS_STR[instr.arg1]);
        PrintWithTabs(out_stream, "add rsp, 16\n");
    }
    else if (instr.type1 == ArgumentType::RAM)
    {
        PrintWithTabs(out_stream, "vmovss xmm5, [rsp]\n");
        PrintWithTabs(out_stream, "add rsp, 16\n");
        PrintWithTabs(out_stream, "vmovss ");
        DumpRAMArgument(out_stream, instr.arg1);
        fprintf(out_stream, ", xmm5\n");
    }
    else
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }
},
{
    if (instr.type1 == ArgumentType::REGISTER)
        address += 9;
    else if (instr.type1 == ArgumentType::RAM)
        address += 14;
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

        PrintWithTabs(out_stream, "sub rsp, 16\n");
        PrintWithTabs(out_stream, "vmovss [rsp], %s\n", REGISTERS_STR[instr.arg1]);
    }
    else if (instr.type1 == ArgumentType::NUM)
    {
        PrintWithTabs(out_stream, "mov rbx, %d\n", instr.arg1);
        PrintWithTabs(out_stream, "vcvtsi2ss xmm5, rbx\n");
        PrintWithTabs(out_stream, "sub rsp, 16\n");
        PrintWithTabs(out_stream, "vmovss [rsp], xmm5\n");
    }
    else if (instr.type1 == ArgumentType::RAM)
    {
        PrintWithTabs(out_stream, "vmovss xmm5, ");
        DumpRAMArgument(out_stream, instr.arg1);
        fprintf(out_stream, "\n");
        PrintWithTabs(out_stream, "sub rsp, 16\n");
        PrintWithTabs(out_stream, "vmovss [rsp], xmm5\n");
    }
    else
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }
},
{
    if (instr.type1 == ArgumentType::REGISTER)
        address += 9;
    else if (instr.type1 == ArgumentType::NUM)
    {
        // TODO mov
        address += 19;
    }
    else if (instr.type1 == ArgumentType::RAM)
        address += 14;
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

    if (instr.type2 == ArgumentType::REGISTER)
    {
        if (!IsRegister(instr.arg2))
        {
            error->code = (int) ERRORS::INVALID_IR;
            SetErrorData(error, "INVALID REGISTER CODE IN IR (%lu)\n", i);
            return;
        }

        PrintWithTabs(out_stream, "vucomiss %s, %s\n",
                    REGISTERS_STR[instr.arg1], REGISTERS_STR[instr.arg2]);
    }
    else if (instr.type2 == ArgumentType::NUM)
    {
        PrintWithTabs(out_stream, "mov rbx, %d\n", instr.arg2);
        PrintWithTabs(out_stream, "vcvtsi2ss xmm5, rbx\n");
        PrintWithTabs(out_stream, "vucomiss %s, xmm5\n", REGISTERS_STR[instr.arg1]);
    }
},
{
    if (instr.type2 == ArgumentType::REGISTER)
        address += 4;
    else
        address += 14;
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

        PrintWithTabs(out_stream, "mov %s, %s\n", REGISTERS_STR[instr.arg1], REGISTERS_STR[instr.arg2]);
    }
    else if (instr.type2 == ArgumentType::NUM)
    {
        PrintWithTabs(out_stream, "mov %s, %d\n", REGISTERS_STR[instr.arg1], instr.arg2);
    }
    else
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }
},
{
    if (instr.type2 == ArgumentType::REGISTER)
        address += 3;
    else if (instr.type2 == ArgumentType::NUM)
        address += 5;
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
    if (instr.type1 != ArgumentType::REGISTER ||  instr.type2 != ArgumentType::REGISTER)
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

    PrintWithTabs(out_stream, "vsubss %s, %s, %s\n",
            REGISTERS_STR[instr.arg1], REGISTERS_STR[instr.arg2], REGISTERS_STR[instr.arg1]);
},
{
    address += 4;
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

    PrintWithTabs(out_stream, "vaddss %s, %s, %s\n",
            REGISTERS_STR[instr.arg1], REGISTERS_STR[instr.arg2], REGISTERS_STR[instr.arg1]);
},
{
    address += 4;
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

    PrintWithTabs(out_stream, "vmulss %s, %s, %s\n",
            REGISTERS_STR[instr.arg1], REGISTERS_STR[instr.arg2], REGISTERS_STR[instr.arg1]);
},
{
    address += 4;
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

    PrintWithTabs(out_stream, "vdivss %s, %s, %s\n",
            REGISTERS_STR[instr.arg1], REGISTERS_STR[instr.arg2], REGISTERS_STR[instr.arg1]);
},
{
    address += 4;
})


DEF_CMD(SQRT,
{
    if (instr.type1 != ArgumentType::REGISTER ||  instr.type1 != ArgumentType::REGISTER)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    PrintWithTabs(out_stream, "vsqrtss %s, %s\n", REGISTERS_STR[instr.arg1], REGISTERS_STR[instr.arg1]);
},
{
    address += 4;
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

    PrintWithTabs(out_stream, "jmp %d\n", instr.arg1);
},
{
    address += 5;
})

DEF_CMD(JA,
{
    if (instr.type1 != ArgumentType::NUM)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    PrintWithTabs(out_stream, "ja %d\n", instr.arg1);
},
{
    address += 6;
})

DEF_CMD(JAE,
{
    if (instr.type1 != ArgumentType::NUM)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    PrintWithTabs(out_stream, "jae %d\n", instr.arg1);
},
{
    address += 6;
})

DEF_CMD(JB,
{
    if (instr.type1 != ArgumentType::NUM)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    PrintWithTabs(out_stream, "jb %d\n", instr.arg1);
},
{
    address += 6;
})

DEF_CMD(JBE,
{
    if (instr.type1 != ArgumentType::NUM)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    PrintWithTabs(out_stream, "jbe %d\n", instr.arg1);
},
{
    address += 6;
})

DEF_CMD(JNE,
{
    if (instr.type1 != ArgumentType::NUM)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    PrintWithTabs(out_stream, "jne %d\n", instr.arg1);
},
{
    address += 6;
})

DEF_CMD(JE,
{
    if (instr.type1 != ArgumentType::NUM)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    PrintWithTabs(out_stream, "je %d\n", instr.arg1);
},
{
    address += 6;
})

DEF_CMD(CALL,
{
    if (instr.type1 != ArgumentType::NUM)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    PrintWithTabs(out_stream, "call %d\n", instr.arg1);
},
{
    address += 5;
})

DEF_CMD(RET,
{
    PrintWithTabs(out_stream, "ret\n");
},
{
    address += 1;
})

// .............................................

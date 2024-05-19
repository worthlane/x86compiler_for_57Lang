#ifndef DEF_CMD
#define DEF_CMD(...) ;
#endif

// DEF_CMD(NAME, x86_asm, x86_elf)

DEF_CMD(NOP,
{
    // does nothing
},
{
    // does nothing
})

DEF_CMD(HLT,
{
    PrintWithTabs(out_stream, "call _hlt\n", instr.arg1);
},
{
    ByteCodePush(program_code, Opcodes::CALL, error);
    PushQuadByte(program_code, instr.arg1 - NUM_OPERAND_LEN, error);
})

// --------------------------------------------------------------------------

DEF_CMD(OUT,
{
    PrintWithTabs(out_stream, "call _out\n", instr.arg1);
},
{
    ByteCodePush(program_code, Opcodes::CALL, error);
    PushQuadByte(program_code, instr.arg1 - NUM_OPERAND_LEN, error);
})

// --------------------------------------------------------------------------

DEF_CMD(IN,
{
    PrintWithTabs(out_stream, "call _in\n", instr.arg1);
},
{
    ByteCodePush(program_code, Opcodes::CALL, error);
    PushQuadByte(program_code, instr.arg1 - NUM_OPERAND_LEN, error);
})

// --------------------------------------------------------------------------

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
    EncodePushPopREG(program_code, instr.arg1, InstructionCode::ID_POP, error);
})

// --------------------------------------------------------------------------

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
    {
        EncodePushPopREG(program_code, instr.arg1, InstructionCode::ID_PUSH, error);
    }
    else
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }
})

// --------------------------------------------------------------------------

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
    {
        EncodePushPopXMM(program_code, instr.arg1, InstructionCode::ID_POP_XMM, error);
    }
    else if (instr.type1 == ArgumentType::RAM)
    {
        EncodePushPopXMM(program_code, Registers::XMM5, InstructionCode::ID_POP_XMM, error);
        EncodeMOV_RAM(program_code, Registers::XMM5, instr.arg1, true, error);
    }
    else
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }
})

// --------------------------------------------------------------------------

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
    {
        EncodePushPopXMM(program_code, instr.arg1, InstructionCode::ID_PUSH_XMM, error);
    }
    else if (instr.type1 == ArgumentType::NUM)
    {
        // TODO mov
        EncodeMOV_REG_NUM(program_code, Registers::RBX, instr.arg1, error);
        EncodeMOV_RBX_to_XMM5(program_code, error);
        EncodePushPopXMM(program_code, Registers::XMM5, InstructionCode::ID_PUSH_XMM, error);
    }
    else if (instr.type1 == ArgumentType::RAM)
    {
        EncodeMOV_RAM(program_code, Registers::XMM5, instr.arg1, false, error);
        EncodePushPopXMM(program_code, Registers::XMM5, InstructionCode::ID_PUSH_XMM, error);
    }
    else
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }
})

// --------------------------------------------------------------------------

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
    {
        EncodeCMP(program_code, instr.arg1, instr.arg2, error);
    }
    else
    {
        EncodeMOV_REG_NUM(program_code, Registers::RBX, instr.arg2, error);
        EncodeMOV_RBX_to_XMM5(program_code, error);
        EncodeCMP(program_code, instr.arg1, Registers::XMM5, error);
    }
})

// --------------------------------------------------------------------------

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
    {
        EncodeMOV_REG_REG(program_code, instr.arg1, instr.arg2, error);
    }
    else if (instr.type2 == ArgumentType::NUM)
    {
        EncodeMOV_REG_NUM(program_code, instr.arg1, instr.arg2, error);
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
    EncodeXMMArithm(program_code, instr.arg1, instr.arg2, instr.arg1, InstructionCode::ID_SUB, error);
})

// --------------------------------------------------------------------------

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
    EncodeXMMArithm(program_code, instr.arg1, instr.arg2, instr.arg1, InstructionCode::ID_ADD, error);
})

// --------------------------------------------------------------------------

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
    EncodeXMMArithm(program_code, instr.arg1, instr.arg2, instr.arg1, InstructionCode::ID_MUL, error);
})

// --------------------------------------------------------------------------

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
    EncodeXMMArithm(program_code, instr.arg1, instr.arg2, instr.arg1, InstructionCode::ID_DIV, error);
})

// --------------------------------------------------------------------------

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
    EncodeSQRT(program_code, instr.arg1, instr.arg1, error);
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

    {
        char* cur_label = GetNameFromNameTable(labels, instr.refer_to);
        if (!cur_label)
        {
            error->code = (int) ERRORS::INVALID_IR;
            SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu) [NO LABEL] \n", i);
            return;
        }
        PrintWithTabs(out_stream, "jmp %s\n", cur_label);
    }
},
{
    ByteCodePush(program_code, Opcodes::JMP, error);
    PushQuadByte(program_code, instr.arg1 - NUM_OPERAND_LEN, error);
})

// --------------------------------------------------------------------------

DEF_CMD(JA,
{
    if (instr.type1 != ArgumentType::NUM)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    {
        char* cur_label = GetNameFromNameTable(labels, instr.refer_to);
        if (!cur_label)
        {
            error->code = (int) ERRORS::INVALID_IR;
            SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu) [NO LABEL] \n", i);
            return;
        }
        PrintWithTabs(out_stream, "ja %s\n", cur_label);
    }
},
{
    PushDoubleByte(program_code, Opcodes::JA, error);
    PushQuadByte(program_code, instr.arg1 - NUM_OPERAND_LEN, error);
})

// --------------------------------------------------------------------------

DEF_CMD(JAE,
{
    if (instr.type1 != ArgumentType::NUM)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    {
        char* cur_label = GetNameFromNameTable(labels, instr.refer_to);
        if (!cur_label)
        {
            error->code = (int) ERRORS::INVALID_IR;
            SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu) [NO LABEL] \n", i);
            return;
        }
        PrintWithTabs(out_stream, "jae %s\n", cur_label);
    }
},
{
    PushDoubleByte(program_code, Opcodes::JAE, error);
    PushQuadByte(program_code, instr.arg1 - NUM_OPERAND_LEN, error);
})

// --------------------------------------------------------------------------

DEF_CMD(JB,
{
    if (instr.type1 != ArgumentType::NUM)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    {
        char* cur_label = GetNameFromNameTable(labels, instr.refer_to);
        if (!cur_label)
        {
            error->code = (int) ERRORS::INVALID_IR;
            SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu) [NO LABEL] \n", i);
            return;
        }
        PrintWithTabs(out_stream, "jb %s\n", cur_label);
    }
},
{
    PushDoubleByte(program_code, Opcodes::JB, error);
    PushQuadByte(program_code, instr.arg1 - NUM_OPERAND_LEN, error);
})

// --------------------------------------------------------------------------

DEF_CMD(JBE,
{
    if (instr.type1 != ArgumentType::NUM)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    {
        char* cur_label = GetNameFromNameTable(labels, instr.refer_to);
        if (!cur_label)
        {
            error->code = (int) ERRORS::INVALID_IR;
            SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu) [NO LABEL] \n", i);
            return;
        }
        PrintWithTabs(out_stream, "jbe %s\n", cur_label);
    }
},
{
    PushDoubleByte(program_code, Opcodes::JBE, error);
    PushQuadByte(program_code, instr.arg1 - NUM_OPERAND_LEN, error);
})

// --------------------------------------------------------------------------

DEF_CMD(JNE,
{
    if (instr.type1 != ArgumentType::NUM)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    {
        char* cur_label = GetNameFromNameTable(labels, instr.refer_to);
        if (!cur_label)
        {
            error->code = (int) ERRORS::INVALID_IR;
            SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu) [NO LABEL] \n", i);
            return;
        }
        PrintWithTabs(out_stream, "jne %s\n", cur_label);
    }
},
{
    PushDoubleByte(program_code, Opcodes::JNE, error);
    PushQuadByte(program_code, instr.arg1 - NUM_OPERAND_LEN, error);
})

// --------------------------------------------------------------------------

DEF_CMD(JE,
{
    if (instr.type1 != ArgumentType::NUM)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    {
        char* cur_label = GetNameFromNameTable(labels, instr.refer_to);
        if (!cur_label)
        {
            error->code = (int) ERRORS::INVALID_IR;
            SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu) [NO LABEL] \n", i);
            return;
        }
        PrintWithTabs(out_stream, "je %s\n", cur_label);
    }
},
{
    PushDoubleByte(program_code, Opcodes::JE, error);
    PushQuadByte(program_code, instr.arg1 - NUM_OPERAND_LEN, error);
})

// --------------------------------------------------------------------------

DEF_CMD(CALL,
{
    if (instr.type1 != ArgumentType::NUM)
    {
        error->code = (int) ERRORS::INVALID_IR;
        SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu)\n", i);
        return;
    }

    {
        char* cur_label = GetNameFromNameTable(labels, instr.refer_to);
        if (!cur_label)
        {
            error->code = (int) ERRORS::INVALID_IR;
            SetErrorData(error, "INVALID INSTRUCTION IN IR (%lu) [NO LABEL] \n", i);
            return;
        }
        PrintWithTabs(out_stream, "call %s\n", cur_label);
    }
},
{
    ByteCodePush(program_code, Opcodes::CALL, error);
    PushQuadByte(program_code, instr.arg1 - NUM_OPERAND_LEN, error);
})

// --------------------------------------------------------------------------

DEF_CMD(RET,
{
    PrintWithTabs(out_stream, "ret\n");
},
{
    ByteCodePush(program_code, Opcodes::RET, error);
})

// .............................................

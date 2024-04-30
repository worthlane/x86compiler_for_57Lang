#ifndef DEF_CMD
#define DEF_CMD(...) ;
#endif

// DEF_CMD(NAME, x86_size)

DEF_CMD(HLT)
DEF_CMD(OUT)
DEF_CMD(IN)
DEF_CMD(POP)

// ====== ARITHMETICS CMDS =======

DEF_CMD(SUB)
DEF_CMD(ADD)
DEF_CMD(MUL)
DEF_CMD(DIV)

DEF_CMD(SQRT)
DEF_CMD(SIN)
DEF_CMD(COS)
DEF_CMD(TAN)
DEF_CMD(COT)

// ............. JMP ..............

DEF_CMD(JMP)

DEF_CMD(JA)
DEF_CMD(JAE)
DEF_CMD(JB)
DEF_CMD(JBE)
DEF_CMD(JNE)
DEF_CMD(JE)

DEF_CMD(CALL)

DEF_CMD(RET)


// .............................................

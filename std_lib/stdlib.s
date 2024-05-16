section .data

UNKNOWN_LETTER equ -1
MINUS_TRUE equ 1
neg_flag dq 0
BUFFER db '0000'

SIGN_BIT equ 80000000h
MULTIPLIER equ 1000
DecOutput  db '0123456789-.', 0x0A
SIGN_PRINT equ 10
DOT_PRINT  equ 11
NEW_LINE_PRINT equ 12

section .text

%include "std_lib/out.s"
%include "std_lib/in.s"
%include "std_lib/hlt.s"

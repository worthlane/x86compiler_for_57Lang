_out:       mov rbx, 0
            vcvtsi2ss xmm5, rbx
            vucomiss xmm0, xmm5

            jae Positive

            mov rdi, SIGN_PRINT                        ; print minus
            call PrintChar

            mov rbx, -1
            vcvtsi2ss xmm5, rbx
            vmulss xmm0, xmm0, xmm5

Positive:

            vcvttss2si rdi, xmm0            ; getting integrer part
            vcvtsi2ss xmm1, rdi             ; xmm1 = int_part
            call PrintDec                   ; print integrer part

            mov rdi, DOT_PRINT
            call PrintChar                  ; print dot

            vsubss xmm0, xmm0, xmm1         ; mod(xmm0) < 1

            mov rdi, MULTIPLIER
            vcvtsi2ss xmm1, rdi
            vmulss xmm0, xmm1, xmm0         ; getting first (MULTIPLIER / 10) digits
            vcvttss2si rdi, xmm0
            call PrintDec                   ; print digits

            mov rdi, NEW_LINE_PRINT
            call PrintChar                  ; print new line

            ret

;:================================================
; Prints dec number in console
; Entry: rdi - dec number
; Exit: rax - symbols printed
; Destr: rcx
;:================================================

PrintDec:           push rbx
                    push rdx
                    push rsi

                    xor rbx, rbx                        ; init printed counter

                    test edi, SIGN_BIT                  ; check if number is negative
                    jz PositiveDec
                    push rdi
                    ;mov rdi, SIGN_PRINT                        ; print minus if negative
                    ;call PrintChar
                    add rbx, rax
                    pop rdi

                    neg edi                             ; getting module

PositiveDec:        mov rax, rdi
                    xor rcx, rcx                        ; init digit counter

GetDecDigit:        xor rdx, rdx
                    mov rsi, 10                         ; the basis of the number system
                    div rsi

                    push rdx                            ; saving symbol in stack

                    inc rcx

                    test rax, rax
                    jnz GetDecDigit                     ; stop if rax == 0

                    add rbx, rcx

PrintDecDigits:     pop rdi
                    call PrintChar                      ; get digits from stack and print them

                    loop PrintDecDigits

                    mov rax, rbx

                    pop rsi
                    pop rdx
                    pop rbx
                    ret

;------------------------------------------------
; Prints char
;
; Entry: rdi - char
; Exit: rax - symbols printed
; Destr:
;------------------------------------------------

PrintChar:      push rdx
                push rbx
                push rcx
                push rsi

                mov rsi, DecOutput
                add rsi, rdi
                mov rax, 0x01                               ; write64 (rdi, rsi, rdx) ... r10, r8, r9
                mov rdi, 1                                  ; stdout
                mov rdx, 1                                  ; buffer size
                syscall

                pop rsi
                pop rcx
                pop rbx
                pop rdx
                ret




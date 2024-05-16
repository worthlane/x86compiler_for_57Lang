_in:        mov qword [neg_flag], 0
            mov rdx, 0
            vcvtsi2ss xmm0, rdx

            call CheckMinus                 ; flag changint is num is negative

            cmp rdx, UNKNOWN_LETTER         ; if first symbol is not digit and not minus - return 0
            je Return

            call GetDec                     ; get integrer part

            vcvtsi2ss xmm0, rdx             ; xmm0 = integrer part

            cmp rsi, '.'                    ; if no non-integrer part - return
            jne NormalExit

            xor rdx, rdx
            call GetDec                     ; get non-integrer part

            vcvtsi2ss xmm1, rdx             ; xmm1 = non-integrer part

            call Pow
            vcvtsi2ss xmm2, rax             ; xmm2 = 10^(non-integrer len)

            vdivss xmm1, xmm1, xmm2         ; xmm1 = non-integrer part / 10^(non-integrer len
            vaddss xmm0, xmm1, xmm0         ; sum integrer and non-integrer part

NormalExit: mov rsi, -1
            vcvtsi2ss xmm3, rsi             ; xmm3 = -1

            mov rsi, qword [neg_flag]
            cmp rsi, MINUS_TRUE
            jne Return

            vmulss xmm0, xmm0, xmm3         ; if neg_flag == MINUS_TRUE -> xmm0 *= -1

Return:     ret


; =============================
; Gets char ASCII
; Entry:
; Exit: rax - ASCII
; Destr:
; =============================

GetChar:        push rsi
                push rdi
                push rdx

                mov rsi, BUFFER
                mov rax, 0x00                               ; read64 (rdi, rsi, rdx) ... r10, r8, r9
                mov rdi, 0                                  ; stdin
                mov rdx, 1                                  ; buffer size
                syscall

                xor rax, rax
                mov al, [BUFFER]

                pop rdx
                pop rdi
                pop rsi
                ret


;:================================================
; Gets dec number from console
; Entry: rdx - first digit
; Exit: rax - number_len, rdx - number, rsi - last char
; Destr: rbx, ecx
;:================================================

GetDec:             xor rbx, rbx                        ; init printed counter

NewDigit:           call GetChar
                    mov rsi, rax                        ; rsi = char ASCII

                    cmp al, '0'
                    jl NotDigit

                    cmp al, '9'
                    jg NotDigit

                    push rax

                    mov eax, edx
                    mov ecx, 10
                    mul ecx                             ; EDX:EAX = EAX * 10

                    shl rdx, 32
                    add edx, eax                        ; RDX = EDX:EAX

                    pop rax

                    sub rax, '0'
                    add rdx, rax
                    inc rbx

                    jmp NewDigit

NotDigit:           mov rax, rbx
                    ret

;:================================================
; 10 pow
; Entry: ax - degree
; Exit: rax - 10^ax
; Destr:
;:================================================

Pow:            push rcx
                push rdx
                push rbx

                mov rcx, rax
                mov rdx, 1                          ; 10^0

PowLoop:        mov eax, edx
                mov ebx, 10
                mul ebx                             ; EDX:EAX = EAX * 10

                shl rdx, 32
                add edx, eax                        ; RDX = EDX:EAX

                loop PowLoop

                mov rax, rdx

                pop rbx
                pop rdx
                pop rcx
                ret

;:================================================
; Changes neg_flag if needed
; Entry:
; Exit: rdx - first digit of num (UNKNOWN_LETTER if error)
; Destr:
;:================================================


CheckMinus:     call GetChar
                cmp rax, '-'
                jne PlusNum

                inc qword [neg_flag]                ; change flag if minus
                xor rdx, rdx
                jmp CheckExit

PlusNum:        mov rdx, UNKNOWN_LETTER

                cmp rax, '0'                        ; error if not digit
                jl CheckExit

                cmp rax, '9'                        ; error if not digit
                jg CheckExit

                sub rax, '0'
                mov rdx, rax                        ; rdx = first num digit
CheckExit:
                ret



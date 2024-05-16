_hlt:       mov rax, 0x3c                               ; exit
            mov rdi, 0                                  ; err_code
            syscall

            ret

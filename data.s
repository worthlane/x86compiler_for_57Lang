%include "std_lib/stdlib.s"

global _start
_start:

				call 2038

				call -19 ; call _hlt

				push rbp

				mov rbp, rsp

				vmovss xmm5, [rbp + 40]
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm5, [rbp + 40]
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm1, [rsp]
				add rsp, 16

				vmovss xmm2, [rsp]
				add rsp, 16

				vmulss xmm1, xmm2, xmm1

				sub rsp, 16
				vmovss [rsp], xmm1

				mov rbx, 4
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm5, [rbp + 24]
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm1, [rsp]
				add rsp, 16

				vmovss xmm2, [rsp]
				add rsp, 16

				vmulss xmm1, xmm2, xmm1

				sub rsp, 16
				vmovss [rsp], xmm1

				vmovss xmm5, [rbp + 56]
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm1, [rsp]
				add rsp, 16

				vmovss xmm2, [rsp]
				add rsp, 16

				vmulss xmm1, xmm2, xmm1

				sub rsp, 16
				vmovss [rsp], xmm1

				vmovss xmm1, [rsp]
				add rsp, 16

				vmovss xmm2, [rsp]
				add rsp, 16

				vsubss xmm1, xmm2, xmm1

				sub rsp, 16
				vmovss [rsp], xmm1

				vmovss xmm5, [rbp - 16]
				sub rsp, 16
				vmovss [rsp], xmm5

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm1, [rsp]
				add rsp, 16

				vmovss xmm2, [rsp]
				add rsp, 16

				vucomiss xmm1, xmm2

				ja 28

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				jmp 23

				mov rbx, 1
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm1, [rsp]
				add rsp, 16

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				vucomiss xmm1, xmm5

				je 70

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm0, [rsp]
				add rsp, 16

				call -891 ; call _out

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm0, [rsp]
				add rsp, 16

				mov rsp, rbp

				pop rbp

				ret

				vmovss xmm5, [rbp - 16]
				sub rsp, 16
				vmovss [rsp], xmm5

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm1, [rsp]
				add rsp, 16

				vmovss xmm2, [rsp]
				add rsp, 16

				vucomiss xmm1, xmm2

				je 28

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				jmp 23

				mov rbx, 1
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm1, [rsp]
				add rsp, 16

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				vucomiss xmm1, xmm5

				je 257

				mov rbx, 1
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm0, [rsp]
				add rsp, 16

				call -1090 ; call _out

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm5, [rbp + 40]
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm1, [rsp]
				add rsp, 16

				vmovss xmm2, [rsp]
				add rsp, 16

				vsubss xmm1, xmm2, xmm1

				sub rsp, 16
				vmovss [rsp], xmm1

				mov rbx, 2
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm5, [rbp + 24]
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm1, [rsp]
				add rsp, 16

				vmovss xmm2, [rsp]
				add rsp, 16

				vmulss xmm1, xmm2, xmm1

				sub rsp, 16
				vmovss [rsp], xmm1

				vmovss xmm1, [rsp]
				add rsp, 16

				vmovss xmm2, [rsp]
				add rsp, 16

				vdivss xmm1, xmm2, xmm1

				sub rsp, 16
				vmovss [rsp], xmm1

				vmovss xmm5, [rbp - 32]
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm0, [rsp]
				add rsp, 16

				call -1277 ; call _out

				mov rbx, 1
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm0, [rsp]
				add rsp, 16

				mov rsp, rbp

				pop rbp

				ret

				mov rbx, 2
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm0, [rsp]
				add rsp, 16

				call -1343 ; call _out

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm5, [rbp + 40]
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm1, [rsp]
				add rsp, 16

				vmovss xmm2, [rsp]
				add rsp, 16

				vsubss xmm1, xmm2, xmm1

				sub rsp, 16
				vmovss [rsp], xmm1

				vmovss xmm5, [rbp - 16]
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm0, [rsp]
				add rsp, 16

				vsqrtss xmm0, xmm0

				sub rsp, 16
				vmovss [rsp], xmm0

				vmovss xmm1, [rsp]
				add rsp, 16

				vmovss xmm2, [rsp]
				add rsp, 16

				vaddss xmm1, xmm2, xmm1

				sub rsp, 16
				vmovss [rsp], xmm1

				mov rbx, 2
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm5, [rbp + 24]
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm1, [rsp]
				add rsp, 16

				vmovss xmm2, [rsp]
				add rsp, 16

				vmulss xmm1, xmm2, xmm1

				sub rsp, 16
				vmovss [rsp], xmm1

				vmovss xmm1, [rsp]
				add rsp, 16

				vmovss xmm2, [rsp]
				add rsp, 16

				vdivss xmm1, xmm2, xmm1

				sub rsp, 16
				vmovss [rsp], xmm1

				vmovss xmm5, [rbp - 32]
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm0, [rsp]
				add rsp, 16

				call -1597 ; call _out

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm5, [rbp + 40]
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm1, [rsp]
				add rsp, 16

				vmovss xmm2, [rsp]
				add rsp, 16

				vsubss xmm1, xmm2, xmm1

				sub rsp, 16
				vmovss [rsp], xmm1

				vmovss xmm5, [rbp - 16]
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm0, [rsp]
				add rsp, 16

				vsqrtss xmm0, xmm0

				sub rsp, 16
				vmovss [rsp], xmm0

				vmovss xmm1, [rsp]
				add rsp, 16

				vmovss xmm2, [rsp]
				add rsp, 16

				vsubss xmm1, xmm2, xmm1

				sub rsp, 16
				vmovss [rsp], xmm1

				mov rbx, 2
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm5, [rbp + 24]
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm1, [rsp]
				add rsp, 16

				vmovss xmm2, [rsp]
				add rsp, 16

				vmulss xmm1, xmm2, xmm1

				sub rsp, 16
				vmovss [rsp], xmm1

				vmovss xmm1, [rsp]
				add rsp, 16

				vmovss xmm2, [rsp]
				add rsp, 16

				vdivss xmm1, xmm2, xmm1

				sub rsp, 16
				vmovss [rsp], xmm1

				vmovss xmm5, [rbp - 48]
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm0, [rsp]
				add rsp, 16

				call -1851 ; call _out

				mov rbx, 2
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm0, [rsp]
				add rsp, 16

				mov rsp, rbp

				pop rbp

				ret

				push rbp

				mov rbp, rsp

				vmovss xmm5, [rbp + 24]
				sub rsp, 16
				vmovss [rsp], xmm5

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm1, [rsp]
				add rsp, 16

				vmovss xmm2, [rsp]
				add rsp, 16

				vucomiss xmm1, xmm2

				je 28

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				jmp 23

				mov rbx, 1
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm1, [rsp]
				add rsp, 16

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				vucomiss xmm1, xmm5

				je 369

				vmovss xmm5, [rbp + 40]
				sub rsp, 16
				vmovss [rsp], xmm5

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm1, [rsp]
				add rsp, 16

				vmovss xmm2, [rsp]
				add rsp, 16

				vucomiss xmm1, xmm2

				je 28

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				jmp 23

				mov rbx, 1
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm1, [rsp]
				add rsp, 16

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				vucomiss xmm1, xmm5

				je 37

				mov rbx, 8
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm0, [rsp]
				add rsp, 16

				call -2187 ; call _out

				vmovss xmm5, [rbp + 40]
				sub rsp, 16
				vmovss [rsp], xmm5

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm1, [rsp]
				add rsp, 16

				vmovss xmm2, [rsp]
				add rsp, 16

				vucomiss xmm1, xmm2

				jne 28

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				jmp 23

				mov rbx, 1
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm1, [rsp]
				add rsp, 16

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				vucomiss xmm1, xmm5

				je 37

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm0, [rsp]
				add rsp, 16

				call -2353 ; call _out

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm0, [rsp]
				add rsp, 16

				mov rsp, rbp

				pop rbp

				ret

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm5, [rbp + 40]
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm1, [rsp]
				add rsp, 16

				vmovss xmm2, [rsp]
				add rsp, 16

				vsubss xmm1, xmm2, xmm1

				sub rsp, 16
				vmovss [rsp], xmm1

				vmovss xmm5, [rbp + 24]
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm1, [rsp]
				add rsp, 16

				vmovss xmm2, [rsp]
				add rsp, 16

				vdivss xmm1, xmm2, xmm1

				sub rsp, 16
				vmovss [rsp], xmm1

				vmovss xmm5, [rbp - 16]
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm0, [rsp]
				add rsp, 16

				call -2523 ; call _out

				vmovss xmm5, [rbp - 16]
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm0, [rsp]
				add rsp, 16

				mov rsp, rbp

				pop rbp

				ret

				push rbp

				mov rbp, rsp

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				call -2426 ; call _in

				sub rsp, 16
				vmovss [rsp], xmm0

				vmovss xmm5, [rsp]
				add rsp, 16
				vmovss [rbp - 16], xmm5

				call -2454 ; call _in

				sub rsp, 16
				vmovss [rsp], xmm0

				vmovss xmm5, [rsp]
				add rsp, 16
				vmovss [rbp - 32], xmm5

				call -2482 ; call _in

				sub rsp, 16
				vmovss [rsp], xmm0

				vmovss xmm5, [rsp]
				add rsp, 16
				vmovss [rbp - 48], xmm5

				vmovss xmm5, [rbp - 16]
				sub rsp, 16
				vmovss [rsp], xmm5

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm1, [rsp]
				add rsp, 16

				vmovss xmm2, [rsp]
				add rsp, 16

				vucomiss xmm1, xmm2

				je 28

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				jmp 23

				mov rbx, 1
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm1, [rsp]
				add rsp, 16

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				vucomiss xmm1, xmm5

				je 68

				mov rdi, rsp

				vmovss xmm5, [rbp - 48]
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm5, [rbp - 32]
				sub rsp, 16
				vmovss [rsp], xmm5

				push rdi

				call -997

				pop rdi

				mov rsp, rdi

				sub rsp, 16
				vmovss [rsp], xmm0

				vmovss xmm5, [rsp]
				add rsp, 16
				vmovss [rbp - 64], xmm5

				vmovss xmm5, [rbp - 16]
				sub rsp, 16
				vmovss [rsp], xmm5

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm1, [rsp]
				add rsp, 16

				vmovss xmm2, [rsp]
				add rsp, 16

				vucomiss xmm1, xmm2

				jne 28

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				jmp 23

				mov rbx, 1
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm1, [rsp]
				add rsp, 16

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				vucomiss xmm1, xmm5

				je 82

				mov rdi, rsp

				vmovss xmm5, [rbp - 48]
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm5, [rbp - 32]
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm5, [rbp - 16]
				sub rsp, 16
				vmovss [rsp], xmm5

				push rdi

				call -2570

				pop rdi

				mov rsp, rdi

				sub rsp, 16
				vmovss [rsp], xmm0

				vmovss xmm5, [rsp]
				add rsp, 16
				vmovss [rbp - 64], xmm5

				vmovss xmm5, [rbp - 64]
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm0, [rsp]
				add rsp, 16

				mov rsp, rbp

				pop rbp

				ret


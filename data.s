%include "std_lib/stdlib.s"

global _start
_start:

				call 504

				call -19 ; call _hlt

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

				je 37

				mov rbx, 1
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm0, [rsp]
				add rsp, 16

				mov rsp, rbp

				pop rbp

				ret

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

				je 37

				mov rbx, 0
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm0, [rsp]
				add rsp, 16

				mov rsp, rbp

				pop rbp

				ret

				vmovss xmm5, [rbp + 24]
				sub rsp, 16
				vmovss [rsp], xmm5

				mov rdi, rsp

				vmovss xmm5, [rbp + 24]
				sub rsp, 16
				vmovss [rsp], xmm5

				mov rbx, 1
				vcvtsi2ss xmm5, rbx
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm1, [rsp]
				add rsp, 16

				vmovss xmm2, [rsp]
				add rsp, 16

				vsubss xmm1, xmm2, xmm1

				sub rsp, 16
				vmovss [rsp], xmm1

				push rdi

				call -419

				pop rdi

				mov rsp, rdi

				sub rsp, 16
				vmovss [rsp], xmm0

				vmovss xmm1, [rsp]
				add rsp, 16

				vmovss xmm2, [rsp]
				add rsp, 16

				vmulss xmm1, xmm2, xmm1

				sub rsp, 16
				vmovss [rsp], xmm1

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

				call -835 ; call _in

				sub rsp, 16
				vmovss [rsp], xmm0

				vmovss xmm5, [rsp]
				add rsp, 16
				vmovss [rbp - 16], xmm5

				mov rdi, rsp

				vmovss xmm5, [rbp - 16]
				sub rsp, 16
				vmovss [rsp], xmm5

				push rdi

				call -565

				pop rdi

				mov rsp, rdi

				sub rsp, 16
				vmovss [rsp], xmm0

				vmovss xmm5, [rbp - 32]
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm0, [rsp]
				add rsp, 16

				call -1132 ; call _out

				vmovss xmm5, [rbp - 32]
				sub rsp, 16
				vmovss [rsp], xmm5

				vmovss xmm0, [rsp]
				add rsp, 16

				mov rsp, rbp

				pop rbp

				ret


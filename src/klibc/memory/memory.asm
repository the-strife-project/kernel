BITS 64

; All arguments are caller saved :)
global memcpy
memcpy:
	mov rcx, rdx
	rep movsb
	ret

global memset
memset:
	mov rax, rsi
	mov rcx, rdx
	rep stosb
	ret

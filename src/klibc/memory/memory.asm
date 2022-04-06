BITS 64

global memcpy
global memset
global memmove

; All arguments are caller saved :)
memcpy:
memmove:
	mov rcx, rdx
    cld
	rep movsb
    mov rax, rdi
	ret

memset:
	mov rax, rsi
	mov rcx, rdx
    cld
	rep stosb
    mov rax, rdi
	ret

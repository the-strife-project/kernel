BITS 64

gdtptr:
	dw 0
	dq 0

global __loadGDT
__loadGDT:
	mov word [rel gdtptr], di
	mov qword [rel gdtptr+2], rsi
	lgdt [rel gdtptr]

	push 0x08
	push rsp
	pushf
	push 0x10

	mov rax, finish
	push rax
	iretq

finish:
	pop rax

	mov ax, 0x08
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov gs, ax
	mov fs, ax
	ret

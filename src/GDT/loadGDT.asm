BITS 64

%define KERNEL_CODE 0x08
%define KERNEL_DATA 0x10

gdtptr:
	dw 0
	dq 0

global __loadGDT
__loadGDT:
	mov word [rel gdtptr], di
	mov qword [rel gdtptr+2], rsi
	lgdt [rel gdtptr]

	push KERNEL_DATA
	push rsp
	pushf
	push KERNEL_CODE

	mov rax, finish
	push rax
	iretq

finish:
	pop rax

	mov ax, KERNEL_DATA
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov gs, ax
	mov fs, ax
	ret

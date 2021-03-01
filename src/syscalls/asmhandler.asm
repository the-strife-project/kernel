%define KDATA 0x10
%define UDATA 0x1b

global asm_syscall_handler
extern syscall_handler
asm_syscall_handler:
	mov rax, rsp
	push rax

	; Callee-saved are not saved.
	push rcx
	push rdx
	push rsi
	push rdi
	push r8
	push r9
	push r10
	push r11

	mov ax, KDATA
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	call syscall_handler

	mov ax, UDATA
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	pop r11
	pop r10
	pop r9
	pop r8
	pop rdi
	pop rsi
	pop rdx
	pop rcx

	pop rsp
	o64 sysret

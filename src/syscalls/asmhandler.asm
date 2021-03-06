%define KDATA 0x10
%define UDATA 0x1b
%define SYSCALL_RPC 4

extern privStacks
extern kpaging
extern whoami

global asmSyscallHandler
extern rpcSwitcher
extern syscallHandler
asmSyscallHandler:
	; RPC?
	cmp rdi, SYSCALL_RPC
	je rpcSwitcher
	; RPC here

	; At this point, the syscall is not RPC.
	; The page table will change to a private stack soon.

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

	; I'm gonna use this thank you very much
	push rbx
	push r12

	; Save page table
	mov r12, cr3

	call whoami
	shl eax, 3	; *8

	; Get address of the private stack
	lea rbx, qword [rel privStacks]
	add rax, rbx

	; Go into the kernel page table
	mov rbx, qword [rel kpaging]
	mov cr3, rbx

	; We can now dereference the private stack
	mov rbx, rsp
	mov rsp, qword [rax]
	push rbx

	; Save page table (now for real)
	push r12

	call syscallHandler

returnToAsm:
	; So we're back, get:
	pop rdi	; Page table
	pop rsi	; Old stack

	mov cr3, rdi
	mov rsp, rsi

	pop rbx
	pop r12

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

	o64 sysret

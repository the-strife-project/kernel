; This file contains the code that performs a remote procedure call
; It's possibly the most important part of the kernel, everything else leads to this

%define PIDs 0xffffffffc0000000
extern givenPIDs
extern badPID

; rdi <- Remote PID
; rsi <- Remote function ID
; rcx...r11 <- Parameters

global rpcSwitcher
rpcSwitcher:
	; Save caller-saved registers, we don't want the server looking at those
	push rbx
	push rbp
	push r12
	push r13
	push r14
	push r15

	xor rbx, rbx
	xor rbp, rbp
	xor r12, r12
	xor r13, r13
	xor r14, r14
	xor r15, r15

	; Is the PID valid?
	mov rax, qword [rel givenPIDs]
	cmp rdi, rax
	jge badPID

	

	; All callee-saved are set and must be shared
	; Now everything is set. Let's load the remote stack, and for that
	; let's change the page table.

	ret

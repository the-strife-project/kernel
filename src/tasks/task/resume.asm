BITS 64

global actuallyResumeTask

; Not user yet lmao
%define USER_DATA 0x08
%define USER_CODE 0x10

; rdi <- rsp
; rsi <- rip
; rdx <- GeneralRegisters*
; rcx <- rflags
actuallyResumeTask:
	mov ax, USER_DATA
	mov ds, ax
	mov es, ax
	mov gs, ax
	mov fs, ax

	; Stack frame for iretq
	push USER_DATA ; ss
	push rdi       ; rsp
	push rcx       ; rflags
	push USER_CODE ; cs
	push rsi       ; rip

	; General purpose registers
	mov r15, rdx
	mov rax, [r15 + 8*0]
	mov rbx, [r15 + 8*1]
	mov rcx, [r15 + 8*2]
	mov rdx, [r15 + 8*3]
	mov rsi, [r15 + 8*4]
	mov rdi, [r15 + 8*5]
	mov rbp, [r15 + 8*6]
	mov r8, [r15 + 8*7]
	mov r9, [r15 + 8*8]
	mov r10, [r15 + 8*9]
	mov r11, [r15 + 8*10]
	mov r12, [r15 + 8*11]
	mov r13, [r15 + 8*12]
	mov r14, [r15 + 8*13]
	mov r15, [r15 + 8*14]

	iretq

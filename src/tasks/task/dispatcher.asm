BITS 64

global asmRestoreKernel
global asmDispatchSaving
global asmDispatch
extern savedKernelState_rsp
extern savedKernelState
extern whoami
extern pubStacks

%define USER_CPL 3
%define USER_DATA (0x18 | USER_CPL)
%define USER_CODE (0x20 | USER_CPL)

; These two save/restore callee-saved
asmRestoreKernel:
    mov rsp, qword [rel savedKernelState_rsp]
    lea rax, [rel savedKernelState]
    mov rbp, qword [rax+8*0]
    mov rbx, qword [rax+8*1]
    mov r12, qword [rax+8*2]
    mov r13, qword [rax+8*3]
    mov r14, qword [rax+8*4]
    mov r15, qword [rax+8*5]

    ; State has been consumed
    mov qword [rel savedKernelState_rsp], 0
    ret
asmDispatchSaving:
    mov qword [rel savedKernelState_rsp], rsp
    lea rax, [rel savedKernelState]
    mov qword [rax+8*0], rbp
    mov qword [rax+8*1], rbx
    mov qword [rax+8*2], r12
    mov qword [rax+8*3], r13
    mov qword [rax+8*4], r14
    mov qword [rax+8*5], r15
    ; Fall to asmDispatch

; rdi <- rsp
; rsi <- rip
; rdx <- GeneralRegisters*
; rcx <- rflags
; r8  <- Paging
asmDispatch:
    ; Switch to a public stack, so when cr3 is changed, pop can be done
    call whoami
    mov r15, qword [rel pubStacks]
    mov rsp, qword [r15 + rax * 8]

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
    push qword [r15 + 8*7]    ; Saving this for later
    mov r9, [r15 + 8*8]
    mov r10, [r15 + 8*9]
    mov r11, [r15 + 8*10]
    mov r12, [r15 + 8*11]
    mov r13, [r15 + 8*12]
    mov r14, [r15 + 8*13]
    mov r15, [r15 + 8*14]

    ; Last thing is change the page table
    mov cr3, r8
    pop r8

    iretq

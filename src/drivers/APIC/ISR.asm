extern preempt
extern origRunning
extern whoami

global asmPreemption
asmPreemption:
    cli

    ; First of all, check if this happened in the kernel
    push rax
    ; Interrupt stack frame starts at [rsp + 8]
    mov rax, qword [rsp+8] ; That's RIP
    test rax, rax
    jns .notKernel

    ; It was indeed the kernel, just ignore it
    pop rax
    iretq

  .notKernel:
    pop rax

    ; This is preemption, have to save everything
    ; This is the same way the syscall routine does it
    ; I want rsp to be a SavedState*
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    push rbp
    push rdi
    push rsi
    push rdx
    push rcx
    push rbx
    push rax
    ; That's 15 values pushed. rsp=&rax, so interrupt stack frame starts
    ;   start at [rsp + 15*8]. Let's get and save the flags so SavedState
    ;   is complete.
    push qword [rsp + 15*8 + 2*8]

    ; Segment field; ignore, it's just used in syscalls
    mov rax, 0
    push rax

    ; And there's my SavedState*
    mov rdi, rsp

    ; Now, 16 values pushed. Interrupt stack frame is at [rsp + 15*8]
    mov rsi, qword [rsp + 17*8 + 0*8] ; rip
    mov rdx, qword [rsp + 17*8 + 3*8] ; rsp

    jmp preempt
    ; That doesn't return

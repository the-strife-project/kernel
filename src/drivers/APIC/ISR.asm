extern preempt
extern origRunning
extern whoami

global asmPreemption
asmPreemption:
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
    ;   start at [rsp + 15*8]. Let's get and save the flags.
    push qword [rsp + 15*8 + 2*8]

    ; Segment field; ignore, it's just used in syscalls
    mov rax, 0
    push rax

    ; And there's my SavedState*
    mov rdi, rsp

    ; Now, 16 values pushed. Interrupt stack frame is at [rsp + 15*8]
    mov rsi, qword [rsp + 17*8 + 0*8] ; rip
    mov rdx, qword [rsp + 17*8 + 3*8] ; rsp

    ; Save page table
    mov rax, cr3
    mov rbp, rax

    ; There we go
    call preempt ; This is in tasks/scheduler/preempt.cpp
    ; If it was a kernel preemption, this returns

    ; Reset page table
    mov rax, rbp
    mov cr3, rax

    add rsp, 2*8 ; Ignore segment and flags
    pop rax
    pop rbx
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    pop rbp
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15
    iretq

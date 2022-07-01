%define KDATA 0x10
%define UDATA 0x1b
%define SYSCALL_RPC 10

extern privStacks
extern kpaging
extern whoami

; These are pointers to the register values that are pushed below (registers.hpp)
global savedState
savedState times 4096 db 0

global asmSyscallHandler
extern rpcSwitcher
extern syscallHandler

%include "aux.asm"

asmSyscallHandler:
    ; Welcome to the syscall routine. This function is executed on each "syscall".

    ; RPC?
    cmp rdi, SYSCALL_RPC
    je rpcSwitcher

    ; At this point, the syscall is not RPC.
    ; The page table will change to a private stack soon.

    ; This is ring 0, about to write to user pages, SMAP off
    SMAPOFF

    ; Time to save everything. This is an actual SavedState repr.
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
    push rax ; No need to keep it since it will contain return value, but for completion
    push r11 ; Flags (again, yes)

    ; Save segment to return later (most of the time it's UDATA)
    mov ax, ds
    push rax

    mov ax, KDATA
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Save page table. r12 is not parameter so it can be trashed.
    mov r12, cr3

    ; rax is not parameter either
    call whoami
    shl eax, 3    ; *8

    ; Get address of the private stack. rbx is free to use
    lea rbx, [rel privStacks]
    mov rbx, qword [rbx]
    add rax, rbx

    ; Go into the kernel page table
    mov rbx, qword [rel kpaging]
    mov cr3, rbx

    ; Finished accesing user pages
    SMAPON

    ; We can now dereference the private stack
    mov rbx, rsp
    mov rsp, qword [rax]
    push rbx

    ; rbx has rsp pointing to saved flags, rax, rbx, rcx...
    ; It's time to save it somewhere so these registers can be accessed later,
    ;   so, in case "syscallHandler" doesn't return, the state can be restored.
    ; Where? Depends on the CPU this is being executed
    call whoami
    shl eax, 3  ; * 8
    lea r11, [rel savedState]
    add rax, r11
    mov qword [rax], rbx

    ; Save page table (now for real)
    push r12

    mov rcx, r10 ; Third argument -> Third parameter
    call syscallHandler

wereBack:
    ; So we're back. rax is set to returned value.
    pop rdi ; Page table
    pop rsi ; Old stack

; This might be executed from the outside, so rdi and rsi are parameters
returnToAsm:
    mov cr3, rdi
    mov rsp, rsi

    ; This is ring 0, about to read userspace pages. SMAP off
    SMAPOFF

    ; Restore segments
    pop rbx
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx

    add rsp, 8 ; Flags, ignoring (they're @ r11)
    pop rbx ; This is rax, but the value needs to be discarded
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

    ; Kernel syscall?
    cmp rdi, 0x0C
    jz .ret

    ; Finished touching userspace
    SMAPON

    ; Usual case, syscall from userspace
    o64 sysret

  .ret: ret

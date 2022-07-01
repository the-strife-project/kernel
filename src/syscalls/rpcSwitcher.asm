; This file contains the code that performs a remote procedure call
; It's possibly the most important part of the kernel, everything else leads to this

; The idea is simple: get remote handler from kernel's public memory,
;   get a stack for this CPU if there isn't one already, switch to the other
;   process' page table, and perform the jump.

; How to organize registers?
; Sys-V function call: rdi, rsi, rdx, rcx, r8, r9
; Syscall-tainted: rcx, r11
; Strife syscall always comes with rdi=syscall id. The rest is customizable.
; In an RPC, let's set rax=server PID, rsi=remote procedure ID.

; If server always expects first Sys-V argument (rdi) as client's PID, then
;   rsi (RPID) comes correctly set, no mov is necessary.
; Then, as rcx is tainted, if second parameter is given last (r10), one can make
;   rcx=r10, and then from rdx to the last (r9) are correctly set.

; Because of all the above, RPC parameters enter the handler as:
;   (rdi=client), (rsi=rpid), rdx, r10, r8, r9
; And changing from RPC syscall to Sys-V function call just takes rcx=r10.
; That's not bad, 4 register-sized arguments, which is 32 bytes.

; Task array is at last GB of virtual memory
%define TASKS ((1 << 64) - (1 << 30) - 1)
%define NTASKS (1 << 16)
%define SYS_RPC_MORE_STACKS 12

; SchedulerTask: offset: paging
%define Off_paging 0
; Task: offset: rpcEntry
%define Off_rpcEntry 0
; Task: offset: rpcFlags
%define Off_rpcFlags 8
; Task: offset: rpcStacks
%define Off_rpcStacks 16

extern privStacks
extern kpaging
extern whoami
extern RPCerr
extern generalTask
extern running
extern asmSyscallHandler

%include "aux.asm"

global rpcSwitcher
rpcSwitcher:
    ; Welcome to RPC :)
    ; The idea is simple: switch page tables, get a stack for this CPU if there
    ;   isn't one, get the remote handler, and jump.
    ; Critical: most of the time, RPC doesn't go through kernel's page table,
    ;   which gives only one context switch.

    ; This is ring 0, about to write to userspace pages
    SMAPOFF

    ; Save all registers. I didn't do this in the beginning, and let the
    ;   server painfully crash the client if it wanted to. However, some
    ;   callee-saved registers aren't being saved in regular non-malicious C++
    ;   and I don't want to deal with that. Save everything.
    push rbx
    push rcx
    push rdx
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    push rbp

    ; rbp contains server PID now
    mov rbp, rax

    ; Who is client?
    mov rbx, [rel running]
    call whoami
    mov r15, qword [rbx + rax*8]
    ; Client PID will always be in r15 now

    ; Server PID alright?
    test rbp, rbp
    jz badPIDnorelease
    cmp rbp, NTASKS
    jae badPIDnorelease

    ; Welcome to the fat critical section
    ; Lock both PIDs: client and server
    lea rax, [TASKS + r15*8]
    acquireTheLock
    lea rax, [TASKS + rbp*8]
    acquireTheLock

    ; Client has to be running. Otherwise, it would have not reached this.
    ; Server running?
    mov rax, qword [rax] ; SchedulerTask*
    btr rax, 0 ; &~1, in order to discard ProtPtr shenanigans
    test rax, rax
    jz badPID

    ; Switch to server's page table now
    mov rax, qword [rax + Off_paging]
    mov rbx, cr3 ; So we can get back
    mov cr3, rax
    ; Stack is now lost

    ; Now we can access generalTask
    mov rax, qword [rel generalTask] ; That's a Task*

    ; Before messing things up, is the remote available?
    mov rcx, qword [rax + Off_rpcEntry]
    test rcx, rcx
    jnz .rpcEntryGood
    ; It's not! Disgraceful
    mov cr3, rbx ; Going back
    jmp badPID

  .rpcEntryGood:
    ; Let's start looking for stacks
    xor ecx, ecx
  .findStack:
    ; In use?
    lea r11, [rax + Off_rpcStacks]
    mov r11, qword [r11 + rcx*8]
    test r11, r11
    jz .tryNextStack
    ; 1?
    dec r11
    test r11, r11
    jz .needMoreStacks
    ; Got it!
    inc r11 ; Valid stack again
    ; Mark as used
    lea rax, [rax + Off_rpcStacks]
    mov qword [rax + rcx*8], 0
    jmp .gotStack
  .needMoreStacks:
    ; All expectations on making this fast are now lost
    ; A new stack is needed. Have to return to kernel's context to allocate it
    ; For that, I'm using a custom syscall: RPC_MORE_STACKS
    ; I'm not actually syscalling since I'm in ring 0 and the instruction
    ;   always returns to ring 3. Instead, I just call the handler.
    ; Note that this call does not trash rcx and r11 as would be usual,
    ;   but rcx (syscall RIP) is checked to make sure it's ring 0 syscalling.
    ; Anyway, syscalling needs a stack, so let's return to previous
    mov rax, cr3
    mov cr3, rbx ; Return to previous page table
    ; Stack is now working

    mov r11, rsi ; Instead of pushing
    mov rdi, SYS_RPC_MORE_STACKS
    mov rsi, rbp ; rax at entry time
    push rax ; Remote cr3

    ; Temporally release client
    lea rax, [TASKS + r15*8]
    releaseTheLock

    mov rcx, $ ; Just anything that's higher half
    call asmSyscallHandler

    ; Acquire client again
    push rax
    lea rax, [TASKS + r15*8]
    acquireTheLock
    pop rax

    ; Did it fail?
    test rax, rax
    ; This is an attack vector. I don't have the time to do it right.
    jz badPID

    ; Go ahead again to the page table
    mov rsi, r11 ; Instead of popping
    pop r11 ; Remote cr3
    mov cr3, r11

    mov r11, rax ; There's my stack!
    ; Getting those values back
    jmp .gotStack
  .tryNextStack:
    inc rcx
    jmp .findStack
  .gotStack:
    ; Very nice, new one is @ r11
    xchg r11, rsp
    push r11
    ; Old stack pointer is now at the bottom of the new one
    push rbx ; Old page table too
  .stackSet:
    ; And there's our stack. The rest is a piece of cake

    ; First get rpcEntry (first field of Task*)
    mov rax, qword [rel generalTask]
    mov rcx, qword [rax + Off_rpcEntry] ; Ready for sysret

    ; Change "running" value to server
    mov rbx, [rel running]
    call whoami
    mov qword [rbx + rax*8], rbp
    ; Congrats, we are now running as remote PID

    ; Grab rflags before finishing
    mov r11, qword [rel generalTask]
    mov r11, qword [r11 + Off_rpcFlags] ; Ready for sysret

    ; rdi = client
    mov rdi, r15
    ; Saving both PIDs like a boss
    push rbp
    push r15

    ; Next stack page, which is user friendly
    mov rax, rsp
    and rsp, ~0xFFF
    add rsp, 8
    push rax

    ; This will have the famous rcx=r10 switch I was talking about when
    ;   in userspace
    push r10

    ; Release them locks
    lea rax, [TASKS + r15*8]
    releaseTheLock
    lea rax, [TASKS + rbp*8]
    releaseTheLock

    ; Finished touching userspace
    SMAPON

    ; There we go!
    o64 sysret
    ; That doesn't return

global rpcReturn
rpcReturn:
    ; So we're back, huh? That's cool
    ; We're on remote page table, and remote stack (top of first page)
    ; First page is kernel only and lower half, checked at page fault,
    ;   so it's safe to proceed.

    ; Ring 0, about to read from userspace pages
    SMAPOFF

    ; Get back to top of the return ticket
    mov r11, rsp ; Saving it for releasing later
    pop rsp

    ; Getting those PIDs back
    pop r15
    pop rbp

    ; Fat critical section, once again
    lea rax, [TASKS + rbp*8]
    acquireTheLock
    lea rax, [TASKS + r15*8]
    acquireTheLock

    ; Is client still alive?
    mov rax, qword [rax]
    btr rax, 0
    test rax, rax
    jz badRPCReturn ; It's dead!

    ; Ok, we're fine. Mark as running
    call whoami
    mov rcx, [rel running]
    mov qword [rcx + rax*8], r15
    ; We're now running as client once again

    ; Save stack to free
    mov rbx, rsp
    add rbx, 0x10

    ; Let's release the stack, look for a free slot
    mov rax, [rel generalTask]
    xor ecx, ecx
  .looking:
    ; In use?
    lea r11, [rax + Off_rpcStacks]
    mov r11, qword [r11 + rcx*8]
    test r11, r11
    jz .gotIt
    ; 1?
    dec r11
    test r11, r11
    jz .absolutelyImpossible
    ; Zamn
  .tryNextStack:
    inc ecx
    jmp .looking
  .absolutelyImpossible: hlt
  .gotIt:
    ; Got the index @ rcx
    lea r11, [rax + Off_rpcStacks]
    mov qword [r11 + rcx*8], rbx

    ; Stack has been freed. Go back to client.
    pop rcx ; Client page table
    pop rsp ; Client stack
    mov cr3, rcx

    ; Nothing else to do. Let's go back.
    lea rax, [TASKS + r15*8]
    releaseTheLock
    lea rax, [TASKS + rbp*8]
    releaseTheLock

    ; Return value, set @ rdx by PF.cpp
    mov rax, rdx

    ; Pop everything from client stack
    pop rbp
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rdx
    pop rcx
    pop rbx

    ; Finished reading from userspace
    SMAPON

    ; And get back
    o64 sysret

badRPCReturn:
    ; TODO
    hlt

; If something bad happened, this returns to kernel page table and stack,
;   much like asmSyscallHandler, except takes no care in keeping state resumable
badPID:
    lea rax, [TASKS + r15*8]
    releaseTheLock
    lea rax, [TASKS + rbp*8]
    releaseTheLock
    ; fall-through
badPIDnorelease:
    call whoami
    shl eax, 3
    lea rbx, [rel privStacks]
    mov rbx, qword [rbx]
    add rax, rbx
    mov rsp, qword [rbx]

    mov rbx, qword [rel kpaging]
    mov cr3, rbx
    SMAPON

    ; That's it, no need to save state
    jmp RPCerr

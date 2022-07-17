; Some auxiliary macros

%define SYSCALL_RPC 10
%define SYSCALL_RPC_MORE_STACKS 12



extern smapSupported

%macro SMAPOFF 0
    cmp byte [rel smapSupported], 0
    jz %%.noSMAP
    stac
  %%.noSMAP:
%endmacro

%macro SMAPON 0
    cmp byte [rel smapSupported], 0
    jz %%.noSMAP
    clac
  %%.noSMAP:
%endmacro



; Acquire a lock
; rax <- ptr to uint64_t to acquire
%macro acquireTheLock 0
  %%.acquire:
    lock bts qword [rax], 0
    jnc %%.acquired
  %%.spinloop:
    pause
    test qword [rax], 1
    jnz %%.spinloop
    jmp %%.acquire
  %%.acquired:
    ; Got it :)
%endmacro

; Release a lock
; rax <- ptr to uint64_t to release
%macro releaseTheLock 0
    btr qword [rax], 0
%endmacro

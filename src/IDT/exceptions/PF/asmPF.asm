extern catchPF
global asmPF
asmPF:
    pop rdi
    mov rsi, rsp ; iret struct
    mov rdx, rax ; Return value in case of RPC return
    call catchPF
    ; That might return!
    hlt

extern catchPF
global asmPF
asmPF:
    pop rdi
    mov rsi, rsp ; iret struct
    call catchPF
    ; That might return!
    hlt

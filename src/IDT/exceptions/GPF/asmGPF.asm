extern catchGPF
global asmGPF

asmGPF:
    pop rdi
    xchg bx, bx
    nop
    nop
    nop
    nop
    call catchGPF
    hlt

extern catchGPF
global asmGPF

asmGPF:
    pop rdi
    pop rsi
    call catchGPF
    hlt

global PIT_wait
global PIT_ISR

stack dq 0
ctr dq 0

PIT_wait:
    ; Reset counter
    mov qword [rel ctr], rdi

    ; Save stack
    lea rax, [rel stack]
    mov qword [rax], rsp
    sti ; Go!
    hlt

LAPIC_ADDR equ 0xFEE00000
EOI_OFFSET equ 0xB0
%macro EOI 0
    mov rax, LAPIC_ADDR + EOI_OFFSET
    mov dword [rax], 0
%endmacro

PIT_ISR:
    ; Interrupt received from the PIT
    cli

    lea rax, [rel ctr]
    dec qword [rax]
    jz .done

    ; And wait for the next one
    EOI
    mov rsp, qword [rel stack] ; So interrupt structures don't stack
    sti
    hlt

  .done:
    ; That's it
    cli
    EOI

    mov rsp, [rel stack]
    ret

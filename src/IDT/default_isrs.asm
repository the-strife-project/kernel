BITS 64

%define SEGMENT_SELECTOR_KDATA 0x10
%define INSTRUCTION_FLAG_MASK 0x200

extern defaultISR

; Macro for handling all interrupts by default
%assign i 0
%rep 256
default_ISR %+ i:
	mov rdi, i
	call defaultISR
	; That will not return
%assign i i+1
%endrep

global defaultISRs
defaultISRs:
	%assign i 0
	%rep 256
	dq default_ISR %+ i
	%assign i i+1
	%endrep

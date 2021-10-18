extern catchPF
global asmPF
asmPF:
	pop rdi
	call catchPF
	; That might return!
	hlt

extern catchPF
global asmPF
asmPF:
	pop rsi
	call catchPF
	; That might return!
	hlt

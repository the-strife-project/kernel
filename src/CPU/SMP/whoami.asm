global whoami
whoami:
	push rbx
	push rcx
	push rdx
	mov eax, 1
	cpuid
	mov eax, ebx
	pop rdx
	pop rcx
	pop rbx
	shr eax, 24
	ret

DEFAULT REL

section .text
global _start

_start:
	mov rcx, 0xFFFFFFFF
	sub r12, rcx
	pop rcx					;pop register at the end of rc4
	pop rdx
	pop rsi
	pop rdi
;	pop rax
;	pop rcx
	jmp 0xEEEEEEEE			;jump to orignal entry point

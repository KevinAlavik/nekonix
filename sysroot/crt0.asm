section .text
global _start

extern main

_start:
    ; call the entry point
    call main

    ; exit using syscall 1 (exit)
    mov rdi, rax
    mov rax, 1
    int 0x80
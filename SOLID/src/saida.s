.section .bss     
.section .text    
.globl _start     

main:
    push %rbp     
    mov %rsp, %rbp
    call retorna10
    pop %rbp
    ret
retorna10:
    push %rbp
    mov %rsp, %rbp
    mov $10, %rax
    pop %rbp
    ret
_start:
    call main
    mov %rax, %rdi
    call imprime_num
    call sair

.include "runtime.s"

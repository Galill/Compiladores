.section .bss
.section .text
.globl _start

dobro:
    push %rbp
    mov %rsp, %rbp
    mov 16(%rbp), %rax
    push %rax
    mov $2, %rax
    pop %rbx
    imul %rbx, %rax
    pop %rbp
    ret
main:
    push %rbp
    mov %rsp, %rbp
    mov $10, %rax
    push %rax
    call dobro
    add $8, %rsp
    mov %rax, 0(%rbp)
    mov 0(%rbp), %rax
    pop %rbp
    ret
_start:
    call main
    mov %rax, %rdi
    call imprime_num
    call sair

.include "runtime.s"

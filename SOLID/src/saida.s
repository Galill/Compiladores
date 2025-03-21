.section .text
.globl _start
_start:
   mov $427, %rax
   push %rax
   mov $7, %rax
   mov %rax, %rbx
   pop %rax
   div %rbx, %rax
   push %rax
   mov $11, %rax
   push %rax
   mov $231, %rax
   push %rax
   mov $5, %rax
   pop %rbx
   add %rbx, %rax
   pop %rbx
   imul %rbx, %rax
   pop %rbx
   add %rbx, %rax

call imprime_num
call sair
.include "runtime.s"

  #
  # modelo de saida para o compilador
  #

  .section .text
  .globl _start

_start:
 mov $72, %RAX
 sub $101, %RAX
 mov $4, %RBX
 IMUL %RBX
 mov %RAX, %RCX

 mov $14, %RAX
 mov $77, %RBX
 MUL %RBX
 
 add %RCX, %RAX

 call imprime_num
 call sair

 .include "runtime.s"
  

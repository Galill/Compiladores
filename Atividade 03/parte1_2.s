  #
  # modelo de saida para o compilador
  #

  .section .text
  .globl _start

_start:

#mov $7, %RAX
#mov $2, %RBX
#DIV %RBX


  mov $7, %RAX
  mov $6, %RBX
  MUL %RBX

  mov $5, %RBX
  MUL %RBX

  mov %RAX, %RCX

  mov $4, %RAX
  mov $3, %RBX
  MUL %RBX
  mov $2, %RBX
  MUL %RBX

  xor %RDX, %RDX
  mov %RAX, %RBX
  mov %RCX, %RAX
  DIV %RBX  

  call imprime_num
  call sair

  .include "runtime.s"
  

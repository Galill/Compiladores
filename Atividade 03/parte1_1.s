  #
  # modelo de saida para o compilador
  #

  .section .text
  .globl _start

_start:
  mov $8 , %RDX
  mov $11, %RAX
  MUL %RDX
  
  mov %RAX, %RCX
  
  xor %RDX, %RDX

  mov $12, %RAX
  mov $9, %RBX
  mul %RBX
  sub %RAX, %RCX

  xor %RDX, %RDX
  
  mov $112, %RBX
  sub $19, %RBX

  add %RBX, %RCX
  mov %RCX, %RAX

  call imprime_num
  call sair

  .include "runtime.s"
  

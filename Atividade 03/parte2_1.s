      #
  # modelo de saida para o compilador
  #

  .section .text
  .globl _start

_start:
  mov $7374, %RBX
  mov $657, %RAX
  MUL %RBX
  mov %RAX, %RCX

  mov $13121517, %RBX
  mov $256, %RAX
  MUL %RBX

  add %RCX, %RAX
  mov $4294979641, %RBX
  add %RBX, %RAX

  call imprime_num
  call sair

  .include "runtime.s"
  

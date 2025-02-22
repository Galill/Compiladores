  #
  # modelo de saida para o compilador
  #

  .section .text
  .globl _start

_start:
  mov $512, %RAX
  mov $65, %RBX
  MUL %RBX
  mov %RAX, %RCX

  mov $5657, %RAX
  mov $23, %RBX
  MUL %RBX

  sub %RAX, %RCX
  mov %RCX, %RAX
  mov $-1, %RBX # Multiplicando por -1 para mostrar que o valor em RAX está correto.
  MUL %RBX 
  
  call imprime_num
  call sair

  .include "runtime.s"
  

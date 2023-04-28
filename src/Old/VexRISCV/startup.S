.global crtStart
.global main
.global _handle_trap

    .section	.start_jump,"ax",@progbits
crtStart:
  //long jump to allow crtInit to be anywhere
  //do it always in 12 bytes
  lui x2,       %hi(crtInit)
  addi x2, x2,  %lo(crtInit)
  jalr x1,x2
  nop

.section .text

.global  trap_entry
.align 5
trap_entry:
  addi sp,sp,-31*4
  sw x1,   1*4(sp)
  sw x2,   2*4(sp)
  sw x3,   3*4(sp)
  sw x4,   4*4(sp)
  sw x5,   5*4(sp)
  sw x6,   6*4(sp)
  sw x7,   7*4(sp)
  sw x8,   8*4(sp)
  sw x9,   9*4(sp)
  sw x10,   10*4(sp)
  sw x11,   11*4(sp)
  sw x12,   12*4(sp)
  sw x13,   13*4(sp)
  sw x14,   14*4(sp)
  sw x15,   15*4(sp)
  sw x16,   16*4(sp)
  sw x17,   17*4(sp)
  sw x18,   18*4(sp)
  sw x19,   19*4(sp)
  sw x20,   20*4(sp)
  sw x21,   21*4(sp)
  sw x22,   22*4(sp)
  sw x23,   23*4(sp)
  sw x24,   24*4(sp)
  sw x25,   25*4(sp)
  sw x26,   26*4(sp)
  sw x27,   27*4(sp)
  sw x28,   28*4(sp)
  sw x29,   29*4(sp)
  sw x30,   30*4(sp)
  sw x31,   31*4(sp)
  call _handle_trap
  lw x1,   1*4(sp)
  lw x2,   2*4(sp)
  lw x3,   3*4(sp)
  lw x4,   4*4(sp)
  lw x5,   5*4(sp)
  lw x6,   6*4(sp)
  lw x7,   7*4(sp)
  lw x8,   8*4(sp)
  lw x9,   9*4(sp)
  lw x10,   10*4(sp)
  lw x11,   11*4(sp)
  lw x12,   12*4(sp)
  lw x13,   13*4(sp)
  lw x14,   14*4(sp)
  lw x15,   15*4(sp)
  lw x16,   16*4(sp)
  lw x17,   17*4(sp)
  lw x18,   18*4(sp)
  lw x19,   19*4(sp)
  lw x20,   20*4(sp)
  lw x21,   21*4(sp)
  lw x22,   22*4(sp)
  lw x23,   23*4(sp)
  lw x24,   24*4(sp)
  lw x25,   25*4(sp)
  lw x26,   26*4(sp)
  lw x27,   27*4(sp)
  lw x28,   28*4(sp)
  lw x29,   29*4(sp)
  lw x30,   30*4(sp)
  lw x31,   31*4(sp)
  addi sp,sp,31*4
  mret
  .text


crtInit:
  .option push
  .option norelax
  la gp, __global_pointer$
  .option pop
  la sp, _stack_start

bss_init:
  la a0, _bss_start
  la a1, _bss_end
bss_loop:
  beq a0,a1,bss_done
  sw zero,0(a0)
  add a0,a0,4
  j bss_loop
bss_done:

ctors_init:
  la a0, _ctors_start
  addi sp,sp,-4
ctors_loop:
  la a1, _ctors_end
  beq a0,a1,ctors_done
  lw a3,0(a0)
  add a0,a0,4
  sw a0,0(sp)
  jalr  a3
  lw a0,0(sp)
  j ctors_loop
ctors_done:
  addi sp,sp,4
  li a0, 0x880     //880 enable timer + external interrupts
  csrw mie,a0
  li a0, 0x1808     //1808 enable interrupts
  csrw mstatus,a0

  call main
infinitLoop:
  j infinitLoop

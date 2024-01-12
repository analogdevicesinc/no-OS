.section .crt0
.global _start
.global main
.weak   irq_callback
.weak   esr_callback

_start:
  j crtInit

.global  trap_entry
.align 4
trap_entry:
  sw x1,  - 1*4(sp)
  sw x5,  - 2*4(sp)
  sw x6,  - 3*4(sp)
  sw x7,  - 4*4(sp)
  sw x10, - 5*4(sp)
  sw x11, - 6*4(sp)
  sw x12, - 7*4(sp)
  sw x13, - 8*4(sp)
  sw x14, - 9*4(sp)
  sw x15, -10*4(sp)
  sw x16, -11*4(sp)
  sw x17, -12*4(sp)
  sw x28, -13*4(sp)
  sw x29, -14*4(sp)
  sw x30, -15*4(sp)
  sw x31, -16*4(sp)
  addi sp,sp,-16*4
  csrr a0, mcause    // load mcause value as parameter
  bltz a0, __handle_isr
__handle_esr:
  csrr a1, mepc
  mv   a2, sp
  call esr_callback
  csrw mepc, a0
  j    __handle_exit
__handle_isr:
  call irq_callback
__handle_exit:
  lw x1 , 15*4(sp)
  lw x5,  14*4(sp)
  lw x6,  13*4(sp)
  lw x7,  12*4(sp)
  lw x10, 11*4(sp)
  lw x11, 10*4(sp)
  lw x12,  9*4(sp)
  lw x13,  8*4(sp)
  lw x14,  7*4(sp)
  lw x15,  6*4(sp)
  lw x16,  5*4(sp)
  lw x17,  4*4(sp)
  lw x28,  3*4(sp)
  lw x29,  2*4(sp)
  lw x30,  1*4(sp)
  lw x31,  0*4(sp)
  addi sp,sp,16*4
  mret
  .text


crtInit:
  la t0, trap_entry
  csrw mtvec, t0
  csrwi mstatus, 0
  csrwi mie, 0

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

//ctors_init:
//  la a0, _ctors_start
//  addi sp,sp,-4
//ctors_loop:
//  la a1, _ctors_end
//  beq a0,a1,ctors_done
//  lw a3,0(a0)
//  add a0,a0,4
//  sw a0,0(sp)
//  jalr  a3
//  lw a0,0(sp)
//  j ctors_loop
//ctors_done:
//  addi sp,sp,4


//  li a0, 0x880     //880 enable timer + external interrupts
//  csrw mie,a0
//  li a0, 0x1808     //1808 enable interrupts
//  csrw mstatus,a0

  call main
  call _exit
infinitLoop:
  j infinitLoop

irq_callback:
  ret

esr_callback:
  j esr_callback

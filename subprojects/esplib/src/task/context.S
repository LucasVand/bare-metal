
#define CTX_SIZE 116

.global __context_switch
.align 4
.section .text
// void __context_switch(void **old_sp, void **new_sp)
__context_switch:
  addi sp, sp, -CTX_SIZE 

  sw x1, 0(sp)
  // Not storing sp
  sw x4, 4(sp)
  sw x5, 8(sp)
  sw x6, 12(sp)
  sw x7, 16(sp)
  sw x8, 20(sp)
  sw x9, 24(sp)
  sw x10, 28(sp)
  sw x11, 32(sp)
  sw x12, 36(sp)
  sw x13, 40(sp)
  sw x14, 44(sp)
  sw x15, 48(sp)
  sw x16, 52(sp)
  sw x17, 56(sp)
  sw x18, 60(sp)
  sw x19, 64(sp)
  sw x20, 68(sp)
  sw x21, 72(sp)
  sw x22, 76(sp)
  sw x23, 80(sp)
  sw x24, 84(sp)
  sw x25, 88(sp)
  sw x26, 92(sp)
  sw x27, 96(sp)
  sw x28, 100(sp)
  sw x29, 104(sp)
  sw x30, 108(sp)
  sw x31, 112(sp)
  
  sw sp, 0(a0) # store the current sp at the old_sp location
  lw sp, 0(a1) # load the new sp from the new_sp location

  lw x1, 0(sp)
  // Not loading sp
  // not loading gp
  lw x4, 4(sp)
  lw x5, 8(sp)
  lw x6, 12(sp)
  lw x7, 16(sp)
  lw x8, 20(sp)
  lw x9, 24(sp)
  lw x10, 28(sp)
  lw x11, 32(sp)
  lw x12, 36(sp)
  lw x13, 40(sp)
  lw x14, 44(sp)
  lw x15, 48(sp)
  lw x16, 52(sp)
  lw x17, 56(sp)
  lw x18, 60(sp)
  lw x19, 64(sp)
  lw x20, 68(sp)
  lw x21, 72(sp)
  lw x22, 76(sp)
  lw x23, 80(sp)
  lw x24, 84(sp)
  lw x25, 88(sp)
  lw x26, 92(sp)
  lw x27, 96(sp)
  lw x28, 100(sp)
  lw x29, 104(sp)
  lw x30, 108(sp)
  lw x31, 112(sp)

  addi sp, sp, CTX_SIZE
  jr ra
  
.global __bootstrap_task 
.align 4
.section .text
// void __bootstrap_task()
__bootstrap_task:
  lw t0, 0(sp)

  addi sp, sp, 4
  // jump unto the task
  jalr ra, t0, 0
  
  // set the current status to cancelled
  li a1, 0
  jal ra, task_current_status_set

  // yield the task
  j task_yield
   


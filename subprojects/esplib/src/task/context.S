

#include "coreasm.h"

.equ CTX_SIZE, 80

// a2 - &old_task->sp, a3 - &new_task->sp
// void __context_switch(void *old_sp, void *new_sp)
.text
.global __context_switch
.type __context_switch, @function
.align 4
__context_switch:
  // enter and create frame for current registers
  entry a1, 32


  // save the current window
  addi a1, a1, -CTX_SIZE

  s32i    a0, a1, 0                   // Save Return Address (PC / a0)
  s32i    a2, a1, 8                   // Save a2
  s32i    a3, a1, 12                  // Save a3
  s32i    a4, a1, 16                  // Save a4
  s32i    a5, a1, 20                  // Save a5
  s32i    a6, a1, 24                  // Save a6
  s32i    a7, a1, 28                  // Save a7
  s32i    a8, a1, 32                  // Save a8
  s32i    a9, a1, 36                  // Save a9
  s32i    a10, a1, 40                 // Save a10
  s32i    a11, a1, 44                 // Save a11
  s32i    a12, a1, 48                 // Save a12
  s32i    a13, a1, 52                 // Save a13
  s32i    a14, a1, 56                 // Save a14
  s32i    a15, a1, 60                 // Save a15

  // Save crucial system registers
  rsr     a4, PS                      // Read Processor State (Interrupt level, etc.)
  s32i    a4, a1, 64                  // Save PS
  rsr     a4, SAR                     // Read Shift Amount Register (used in bitwise ops)
  s32i    a4, a1, 68                  // Save SAR

  // spill all the registers 
  call4 __global_spill_all

  // save the old sp and load the new tasks sp
  s32i    a1, a2, 0 // save current sp in &old_task->sp
  l32i    a1, a3, 0 // load new sp in &new_task->sp


  l32i    a4, a1, 64                  // Restore PS
  wsr     a4, PS
  l32i    a4, a1, 68                  // Restore SAR
  wsr     a4, SAR

  l32i    a0, a1, 0                   // Restore Return Address (PC)
  // Note: skipped a1 because it is our active Stack Pointer
  l32i    a2, a1, 8                   // Restore a2
  l32i    a3, a1, 12                  // Restore a3
  l32i    a4, a1, 16                  // Restore a4
  l32i    a5, a1, 20                  // Restore a5
  l32i    a6, a1, 24                  // Restore a6
  l32i    a7, a1, 28                  // Restore a7
  l32i    a8, a1, 32                  // Restore a8
  l32i    a9, a1, 36                  // Restore a9
  l32i    a10, a1, 40                 // Restore a10
  l32i    a11, a1, 44                 // Restore a11
  l32i    a12, a1, 48                 // Restore a12
  l32i    a13, a1, 52                 // Restore a13
  l32i    a14, a1, 56                 // Restore a14
  l32i    a15, a1, 60                 // Restore a15

  addi    a1, a1, CTX_SIZE            // Clean up context frame from new task's stack 
  
  // on first task entry there is no registers to load when the window underflows so i need to 
  // figure out how to make it think there are registers to load, maybe increasing the inital stack space but
  // its going to loop at the old 
  retw
 

.text
.align  4
.global __global_spill_all
.type   __global_spill_all,@function
__global_spill_all:
    entry a1, 48
    window_spill4
    window_spill8
    window_spill12
    retw

// Crucial: Instantiate the common macro code at the end of the file
.align 4
window_spill_common 

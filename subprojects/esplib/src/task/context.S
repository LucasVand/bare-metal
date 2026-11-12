#include "coreasm.h"
#include "corebits.h"

.macro SPILL_ALL_WINDOWS
#if XCHAL_NUM_AREGS == 64
  and a12, a12, a12
  rotw 3
  and a12, a12, a12
  rotw 3
  and a12, a12, a12
  rotw 3
  and a12, a12, a12
  rotw 3
  and a12, a12, a12
  rotw 4
#elif XCHAL_NUM_AREGS == 32
  and a12, a12, a12
  rotw 3
  and a12, a12, a12
  rotw 3
  and a4, a4, a4
  rotw 2
#else
#error Unrecognized XCHAL_NUM_AREGS
#endif
.endm

.equ CTX_A0, 0
.equ CTX_PS, 4
.equ CTX_SAR, 8
.equ CTX_OLD_SP, 12
.equ CTX_NEW_SP, 16
.equ CTX_SIZE, 48

// a2 - &old_task->sp, a3 - &new_task->sp
// void __context_switch(void **old_sp, void **new_sp)
.text
.global __context_switch
.type __context_switch, @function
.align 4
__context_switch:
  entry a1, CTX_SIZE

  s32i a0, a1, CTX_A0
  rsr a0, PS
  s32i a0, a1, CTX_PS
  rsr a0, SAR
  s32i a0, a1, CTX_SAR
  s32i a2, a1, CTX_OLD_SP
  s32i a3, a1, CTX_NEW_SP

  rsr a2, PS
  movi a0, PS_INTLEVEL_MASK
  and a3, a2, a0
  bgeui a3, XCHAL_EXCM_LEVEL, 1f
  movi a3, XCHAL_EXCM_LEVEL
1:
  movi a0, PS_UM | PS_WOE
  or a3, a3, a0
  wsr a3, PS
  rsync
  rsr a0, EPC1

  SPILL_ALL_WINDOWS

  wsr a2, PS
  rsync
  wsr a0, EPC1

  l32i a2, a1, CTX_OLD_SP
  s32i a1, a2, 0
  l32i a3, a1, CTX_NEW_SP
  l32i a1, a3, 0

  l32i a2, a1, CTX_PS
  l32i a0, a1, CTX_A0
  l32i a3, a1, CTX_SAR
  wsr a2, PS
  wsr a3, SAR
  rsync

  retw
.text
.align 4
.global __task_bootstrap
.type __task_bootstrap, @function
__task_bootstrap:
  mov a10, a2
  callx8 a3
  call4 task_exit
1:
  j 1b

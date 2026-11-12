
#include "mem.h"
#include "stdio.h"
#include "sys_start.h"
#include "task_int.h"
#include "timg.h"
#include <stddef.h>

extern void main(void *);

void call_start_cpu0(void) {
  printf("Starting system...\n\r");

  start_bss();

  rwdt_unlock();
  RTC_CNTL->wdtconfig0.bits.en = 0;
  RTC_CNTL->wdtconfig0.bits.flashboot_mod_en = 0;
  rwdt_lock();

  mwdt_unlock(TIMG0);
  TIMG0->wdtconfig0.bits.en = 0;
  TIMG0->wdtconfig0.bits.flashboot_mod_en = 0;
  mwdt_lock(TIMG0);

  task_handle_t handle = task_create(main, NULL);
  if (handle == NULL) {
    printf("(FATAL) Unable to create main task\r\n");
  } else {
    scheduler_loop();
  }

  printf("All tasks completed\r\n");
  while (1) {
    asm volatile("nop");
  }
}

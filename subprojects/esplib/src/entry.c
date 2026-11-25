
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
  RTC_WDT->wdtconfig0.bits.en = 0;
  RTC_WDT->wdtconfig0.bits.flashboot_mod_en = 0;
  rwdt_lock();

  mwdt_unlock(TIMG0);
  TIMG0->wdtconfig0.bits.en = 0;
  TIMG0->wdtconfig0.bits.flashboot_mod_en = 0;
  mwdt_conf_update(TIMG0);
  mwdt_lock(TIMG0);

  scheduler_allocators_start();

  TaskHandle handle = task_create(main, NULL, 0);
  if (handle == NULL) {
    printf("(FATAL) Unable to create main task\r\n");
  } else {
    scheduler_start();
  }

  printf("All tasks completed\r\n");
  while (1) {
    asm volatile("nop");
  }
}

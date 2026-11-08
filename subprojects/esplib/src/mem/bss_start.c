
#include "mem.h"
#include "stdio.h"
#include <stdint.h>

extern void *__bss_start;
extern char __bss_size;

void start_bss() {
  void *bss_addr = (void *)&__bss_start;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"
#pragma clang diagnostic ignored "-Wpointer-to-int-cast"
  uint32_t bss_size = (uint32_t)&__bss_size;

  printf("Clearing .bss (%p) (0x%lx bytes)\r\n", bss_addr, bss_size);
  memset(bss_addr, 0, bss_size);
#pragma clang diagnostic pop
}

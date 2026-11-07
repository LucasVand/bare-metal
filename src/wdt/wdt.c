#include "wdt.h"
#include <stdint.h>

volatile uint32_t *wdt_reg(uint32_t base, uint32_t offset) {
  return (volatile uint32_t *)(base + offset);
}

void rwdt_feed() {
  volatile uint32_t *feed = wdt_reg(RTC_CNTL_BASE, RWDT_FEED_REG);
  rwdt_unlock();
  *feed = 1u << 31;
  rwdt_lock();
}

void mwdt_feed(uint32_t tmg_base) {
  volatile uint32_t *feed = wdt_reg(RTC_CNTL_BASE, MWDT_FEED_REG);
  mwdt_unlock(tmg_base);
  *feed = 1u << 31;
  mwdt_lock(tmg_base);
}

void rwdt_unlock() {
  volatile uint32_t *wprotect = wdt_reg(RTC_CNTL_BASE, RWDT_WPROTECT_REG);
  *wprotect = WDT_WPROTECT_KEY;
}
void rwdt_lock() {
  volatile uint32_t *wprotect = wdt_reg(RTC_CNTL_BASE, RWDT_WPROTECT_REG);
  *wprotect = 0;
}
void mwdt_unlock(uint32_t tmg_base) {
  volatile uint32_t *wprotect = wdt_reg(tmg_base, MWDT_WPROTECT_REG);
  *wprotect = WDT_WPROTECT_KEY;
}
void mwdt_lock(uint32_t tmg_base) {
  volatile uint32_t *wprotect = wdt_reg(tmg_base, MWDT_WPROTECT_REG);
  *wprotect = 0;
}

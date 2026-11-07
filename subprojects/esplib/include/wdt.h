#pragma once

#include "wdt/wdt_registers.h"
#include <stdint.h>

void mwdt_feed(uint32_t mwdt_base);
void rwdt_feed();

void rwdt_unlock();
void rwdt_lock();
void mwdt_unlock(uint32_t tmg_base);
void mwdt_lock(uint32_t tmg_base);

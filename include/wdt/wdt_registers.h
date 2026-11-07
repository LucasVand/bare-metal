
#pragma once

#include <stdint.h>

/**
 * ===========================================================================
 * Watchdog timer (WDT) register offsets — ESP32 TRM v5.9
 *   MWDT (Main System Watchdog, one per timer group)
 *       Chapter 10 Timer Group (TIMG), registers 10.10–10.23 (p. 229–233)
 *   RWDT (RTC Watchdog Timer)
 *       Chapter 9 Low-Power Management (RTC_CNTL), registers 9.30–9.33 (p.
 * 217–218) Stage machine, feeding, write protection, flash-boot protection
 *       Chapter 11 Watchdog Timers (WDT), p. 234–235
 * ===========================================================================
 */

/**
 * Peripheral base addresses — TRM Table 3.3-6
 *   TIMG0_BASE hosts MWDT0, TIMG1_BASE hosts MWDT1 — identical offsets on both,
 *   so any "TIMGn_" register below works with either base.
 *   RTC_CNTL is also mapped at 0x60008000 when accessed through the AHB bus.
 */
#define TIMG0_BASE 0x3FF5F000
#define TIMG1_BASE 0x3FF60000
#define RTC_CNTL_BASE 0x3FF48000

volatile uint32_t *wdt_reg(uint32_t base, uint32_t offset);

/**
 * WDT_REG — full register pointer from a base address and a register offset
 *   e.g. WDT_REG(RTC_CNTL_BASE, RWDT_WDTWPROTECT_REG)
 */
#define WDT_REG(base, offset) ((volatile unsigned int *)((base) + (offset)))

/**
 * Write-protection key — TRM 11.3.3
 *   WDTWPROTECT resets to this value (= writes unlocked). As soon as the
 *   register holds ANY other value, writes to every register of that watchdog
 *   are ignored — including the feed register.
 *   Required sequence: write key → modify/feed → write anything != key.
 */
#define WDT_WPROTECT_KEY 0x50D83AA1

/**
 * ===========================================================================
 * MWDT registers — offsets relative to TIMG0_BASE or TIMG1_BASE
 * ===========================================================================
 */

/**
 * TIMGn_WDTCONFIG0_REG (0x48) — TRM 10.10, watchdog configuration (R/W)
 *   Reset: 0x00004000 (disabled, all stages off, flash-boot protection on)
 *   [31]     WDT_EN           — 1: MWDT enabled (reset 0)
 *   [30:29]  WDT_STG0         — stage 0 action (reset 0)
 *   [28:27]  WDT_STG1         — stage 1 action (reset 0)
 *   [26:25]  WDT_STG2         — stage 2 action (reset 0)
 *   [24:23]  WDT_STG3         — stage 3 action (reset 0)
 *     action values: 0 = off, 1 = interrupt, 2 = reset CPU, 3 = reset system
 *     (MWDT0 CPU reset hits PRO CPU only, MWDT1 hits APP CPU only)
 *   [22]     WDT_EDGE_INT_EN  — edge-type interrupt when an interrupt stage
 * expires [21]     WDT_LEVEL_INT_EN — level-type interrupt when an interrupt
 * stage expires [20:18]  WDT_CPU_RESET_LENGTH — CPU reset pulse width (reset 1
 * = 200 ns) [17:15]  WDT_SYS_RESET_LENGTH — system reset pulse width (reset 1 =
 * 200 ns) values: 0:100ns 1:200ns 2:300ns 3:400ns 4:500ns 5:800ns 6:1.6µs
 * 7:3.2µs [14]     WDT_FLASHBOOT_MOD_EN — flash-boot protection (reset 1);
 * clear it after boot to reconfigure the MWDT (TRM 11.3.4) [13:0]   reserved
 */
#define MWDT_CONFIG0_REG 0x48

#define MWDT_EN (1u << 31)
#define MWDT_STG0_SHIFT 29
#define MWDT_STG1_SHIFT 27
#define MWDT_STG2_SHIFT 25
#define MWDT_STG3_SHIFT 23
#define MWDT_STG_MASK 0x3u
#define MWDT_EDGE_INT_EN (1u << 22)
#define MWDT_LEVEL_INT_EN (1u << 21)
#define MWDT_CPU_RESET_LENGTH_SHIFT 18
#define MWDT_SYS_RESET_LENGTH_SHIFT 15
#define MWDT_FLASHBOOT_MOD_EN (1u << 14)

/**
 * TIMGn_WDTCONFIG1_REG (0x4C) — TRM 10.11, prescaler (R/W)
 *   [31:16]  WDT_CLK_PRESCALE — MWDT clock prescale (reset 0x0001)
 *                                MWDT tick = APB_CLK (80 MHz) / prescale
 *   [15:0]   reserved
 *   stage_period = WDTCONFIGn × prescale / 80 MHz
 */
#define MWDT_CONFIG1_REG 0x4C
#define MWDT_CLK_PRESCALE_SHIFT 16

/**
 * TIMGn_WDTCONFIG2_REG (0x50) — TRM 10.12, stage 0 timeout (R/W)
 *   [31:0]   Stage 0 timeout value in MWDT clock cycles (reset 26000000
 *            = 0.325 s at prescale 1)
 */
#define MWDT_CONFIG2_REG 0x50

/**
 * TIMGn_WDTCONFIG3_REG (0x54) — TRM 10.13, stage 1 timeout (R/W)
 *   [31:0]   Stage 1 timeout value in MWDT clock cycles (reset 0x07FFFFFF)
 */
#define MWDT_CONFIG3_REG 0x54

/**
 * TIMGn_WDTCONFIG4_REG (0x58) — TRM 10.14, stage 2 timeout (R/W)
 *   [31:0]   Stage 2 timeout value in MWDT clock cycles (reset 0x000FFFFF)
 */
#define MWDT_CONFIG4_REG 0x58

/**
 * TIMGn_WDTCONFIG5_REG (0x5C) — TRM 10.15, stage 3 timeout (R/W)
 *   [31:0]   Stage 3 timeout value in MWDT clock cycles (reset 0x000FFFFF)
 */
#define MWDT_CONFIG5_REG 0x5C

/**
 * TIMGn_WDTFEED_REG (0x60) — TRM 10.16, feed register (WO)
 *   Write any value to feed the MWDT: returns the stage machine to stage 0
 *   and restarts the expiry counter. Ignored while write protection is on.
 */
#define MWDT_FEED_REG 0x60

/**
 * TIMGn_WDTWPROTECT_REG (0x64) — TRM 10.17, write-protection key (R/W)
 *   Reset 0x50D83AA1 (= unlocked). Any other value enables write protection
 *   for all MWDT registers, including WDTFEED.
 */
#define MWDT_WPROTECT_REG 0x64

/**
 * TIMGn_INT_ENA_REG (0x98) — TRM 10.20, interrupt enable bits (R/W)
 *   [2]      TIMGn_INT_WDT_INT_ENA — enable the watchdog interrupt
 *                                (raised when a stage set to action 1 expires)
 *   [1]      TIMGn_INT_T1_INT_ENA
 *   [0]      TIMGn_INT_T0_INT_ENA
 */
#define MWDT_INT_ENA_REG 0x98
#define MWDT_INT_ENA_BIT (1u << 2)

/**
 * TIMGn_INT_RAW_REG (0x9C) — TRM 10.21, raw interrupt status (RO, unmasked)
 *   [2]      TIMGn_INT_WDT_INT_RAW
 */
#define MWDT_INT_RAW_REG 0x9C

/**
 * TIMGn_INT_ST_REG (0xA0) — TRM 10.22, masked interrupt status (RO)
 *   [2]      TIMGn_INT_WDT_INT_ST — raw status AND its enable bit
 */
#define MWDT_INT_ST_REG 0xA0

/**
 * TIMGn_INT_CLR_REG (0xA4) — TRM 10.23, interrupt clear bits (WO, write 1 to
 * clear) [2]      TIMGn_INT_WDT_INT_CLR
 */
#define MWDT_INT_CLR_REG 0xA4
#define MWDT_INT_CLR_BIT (1u << 2)

/**
 * ===========================================================================
 * RWDT registers — offsets relative to RTC_CNTL_BASE
 * ===========================================================================
 */

/**
 * RTC_CNTL_RESET_STATE_REG (0x34) — TRM 9.12, reset state (mixed)
 *   [13]     PROCPU_STAT_VECTOR_SEL (reset 1)
 *   [12]     APPCPU_STAT_VECTOR_SEL (reset 1)
 *   [11:6]   RTC_CNTL_RESET_CAUSE_APPCPU — reset reason of APP CPU (RO)
 *   [5:0]    RTC_CNTL_RESET_CAUSE_PROCPU — reset reason of PRO CPU (RO)
 *     values (TRM Table 7.1-1, p. 166):
 *       0x01 chip power-on (system)   0x10 RWDT (system)   0x0F brown-out
 *       0x03 software system          0x07 MWDT0 global    0x08 MWDT1 global
 *       0x09 RWDT (core)              0x0D RWDT CPU        0x05 deep sleep
 */
#define RTC_CNTL_RESET_STATE_REG 0x34
#define RTC_CNTL_RESET_CAUSE_PROCPU_SHIFT 0
#define RTC_CNTL_RESET_CAUSE_APPCPU_SHIFT 6

/**
 * RTC_CNTL_INT_ENA_REG (0x3C) — TRM 9.14, interrupt enable bits (R/W)
 *   [4]      RTC_CNTL_TIME_VALID_INT_ENA
 *   [3]      RTC_CNTL_WDT_INT_ENA — enable the RWDT interrupt
 *   [1]      RTC_CNTL_SLP_REJECT_INT_ENA
 *   [0]      RTC_CNTL_SLP_WAKEUP_INT_ENA
 *   ([8] BROWN_OUT, [7] TOUCH, [6] ULP_CP, [5] SAR, [2] SDIO_IDLE)
 */
#define RTC_CNTL_INT_ENA_REG 0x3C
#define RTC_CNTL_WDT_INT_ENA (1u << 3)

/**
 * RTC_CNTL_INT_RAW_REG (0x40) — TRM 9.15, raw interrupt status (RO, unmasked)
 *   [3]      RTC_CNTL_WDT_INT_RAW
 */
#define RTC_CNTL_INT_RAW_REG 0x40
#define RTC_CNTL_WDT_INT_RAW (1u << 3)

/**
 * RTC_CNTL_INT_ST_REG (0x44) — TRM 9.16, masked interrupt status (RO)
 *   [3]      RTC_CNTL_WDT_INT_ST — raw status AND its enable bit
 */
#define RTC_CNTL_INT_ST_REG 0x44
#define RTC_CNTL_WDT_INT_ST (1u << 3)

/**
 * RTC_CNTL_INT_CLR_REG (0x48) — TRM 9.17, interrupt clear bits (WO, write 1 to
 * clear) [3]      RTC_CNTL_WDT_INT_CLR
 */
#define RTC_CNTL_INT_CLR_REG 0x48
#define RTC_CNTL_WDT_INT_CLR (1u << 3)

/**
 * RTC_CNTL_WDTCONFIG0_REG (0x8C) — TRM 9.30, RWDT configuration (R/W)
 *   Reset: 0x00041C00 (disabled, stages off, flash-boot protection on,
 *                      paused in sleep)
 *   [31]     WDT_EN              — 1: RWDT enabled (reset 0)
 *   [30:28]  WDT_STG0            — stage 0 action (reset 0)
 *   [27:25]  WDT_STG1            — stage 1 action (reset 0)
 *   [24:22]  WDT_STG2            — stage 2 action (reset 0)
 *   [21:19]  WDT_STG3            — stage 3 action (reset 0)
 *     action values: 1 = interrupt, 2 = reset CPU, 3 = reset system,
 *                    4 = reset main system + RTC (RWDT only)
 *                    (0 = stage disabled, per TRM 11.3.2)
 *   [17]     reserved
 *   [16:14]  WDT_CPU_RESET_LENGTH  — CPU reset pulse length (reset 1),
 *                                    0–7 RTC_SLOW_CLK cycles
 *   [13:11]  WDT_SYS_RESET_LENGTH  — system reset pulse length (reset 1),
 *                                    0–7 RTC_SLOW_CLK cycles
 *   [10]     WDT_FLASHBOOT_MOD_EN  — flash-boot protection (reset 1); clear it
 *                                    after boot (TRM 11.3.4)
 *   [9]      WDT_PROCPU_RESET_EN   — CPU-reset stages reset the PRO CPU
 *   [8]      WDT_APPCPU_RESET_EN   — CPU-reset stages reset the APP CPU
 *   [7]      WDT_PAUSE_IN_SLP      — pause RWDT in sleep (reset 1)
 *   [6:0]    reserved
 */
#define RWDT_CONFIG0_REG 0x8C
#define RWDT_EN (1u << 31)
#define RWDT_STG0_SHIFT 28
#define RWDT_STG1_SHIFT 25
#define RWDT_STG2_SHIFT 22
#define RWDT_STG3_SHIFT 19
#define RWDT_STG_MASK 0x7u
#define RWDT_CPU_RESET_LENGTH_SHIFT 14
#define RWDT_SYS_RESET_LENGTH_SHIFT 11
#define RWDT_FLASHBOOT_MOD_EN (1u << 10)
#define RWDT_PROCPU_RESET_EN (1u << 9)
#define RWDT_APPCPU_RESET_EN (1u << 8)
#define RWDT_PAUSE_IN_SLP (1u << 7)

/**
 * RTC_CNTL_WDTCONFIGn_REG (n: 1-4) (0x8C + 4×n) — TRM 9.31, stage hold times
 * (R/W) [31:0]   Hold cycles for WDT stage n, in RTC_SLOW_CLK cycles (reset
 * 0x00000FFF = 4095 ≈ 27 ms at the 150 kHz RC clock) WDTCONFIG1 (0x90) …
 * WDTCONFIG4 (0x9C) — the four per-stage timeouts
 */
#define RWDT_CONFIG1_REG 0x90
#define RWDT_CONFIG2_REG 0x94
#define RWDT_CONFIG3_REG 0x98
#define RWDT_CONFIG4_REG 0x9C

/**
 * RTC_CNTL_WDTFEED_REG (0xA0) — TRM 9.32, feed register (WO)
 *   [31]     RTC_CNTL_WDT_FEED — write any value to feed the RWDT: returns the
 *            stage machine to stage 0 and restarts the expiry counter.
 *            Ignored while write protection is on.
 */
#define RWDT_FEED_REG 0xA0

/**
 * RTC_CNTL_WDTWPROTECT_REG (0xA4) — TRM 9.33, write-protection key (R/W)
 *   Reset 0x50D83AA1 (= unlocked). Any other value enables write protection
 *   for all RWDT registers, including WDTFEED.
 */
#define RWDT_WPROTECT_REG 0xA4

/**
 * ===========================================================================
 * Stage actions and timing — shared by MWDT and RWDT (TRM 11.3.2)
 *   The four stages run in a loop 0 → 1 → 2 → 3 → 0; feeding returns to
 *   stage 0. Each stage fires its action after its own timeout.
 * ===========================================================================
 */
#define WDT_STAGE_OFF 0     /* no effect on the system */
#define WDT_STAGE_INT 1     /* trigger an interrupt */
#define WDT_STAGE_CPU_RST 2 /* reset the designated CPU core(s) */
#define WDT_STAGE_SYS_RST 3 /* reset main system (RTC keeps running) */
#define WDT_STAGE_RTC_RST 4 /* reset main system + RTC — RWDT only */

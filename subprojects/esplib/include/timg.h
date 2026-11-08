#pragma once

#include <stddef.h>
#include <stdint.h>

/**
 * ===========================================================================
 * Timer Group (TIMG) registers — ESP32 TRM v5.9
 *
 *   Hardware timers T0/T1, 16-bit prescaler, alarms   Chapter 10.1-10.2 (p.
 * 223) RTC slow-clock calibration (per group)             TRM 10.18-10.19 MWDT
 * (one main watchdog per group)                TRM 10.10-10.17 RWDT (RTC
 * watchdog, lives in the RTC_CNTL block)   TRM 9.30-9.33 (p. 217) Interrupts
 * (per group)                            TRM 10.20-10.23 Shared stage machine /
 * write protection /         Chapter 11 (p. 234) flash-boot protection
 *
 * Access — every fielded register is a union: `.bits` addresses single
 * fields, `.val` accesses the raw word:
 *     TIMG0->timer[0].config.bits.en = 1;       // one field, RMW on the word
 *     TIMG0->timer[0].config.val = 0x40010000;  // whole word, single write
 * Plain 32-bit registers (timeouts, load values, ...) are bare uint32_t:
 *     TIMG0->timer[0].alarmlo = 80000000u;
 *
 * RMW warning — a `.bits.field = x` assignment compiles to read-modify-write.
 * Never use it on:
 *   - write-1-to-clear registers (int_clr, ...)
 *   - trigger registers where the read has a side effect
 *   - fields that self-clear (alarm_en, ...)
 * Write those with `.val` instead, e.g. TIMG0->int_clr.val = 1u << 2;
 *
 * Layout safety — the static assertions at the bottom of this file pin every
 * struct to the TRM offsets at compile time; a layout mistake fails the build.
 * ===========================================================================
 */

/**
 * Peripheral base addresses — TRM Table 3.3-6
 *   TIMG0_BASE hosts MWDT0, TIMG1_BASE hosts MWDT1 — identical register
 *   offsets on both, so every "TIMGn_" offset below works with either base.
 */
#define TIMG0_BASE 0x3FF5F000
#define TIMG1_BASE 0x3FF60000
#define RTC_CNTL_BASE 0x3FF48000

/**
 * ===========================================================================
 * Register offsets — for code that works with raw addresses/pointers
 * ===========================================================================
 */

/**
 * Timer x registers (x: 0-1) — base 0x00 + 0x24*x (TRM 10.3)
 *   TxCONFIG   0x00 + 0x24*x  timer configuration        (TRM 10.1)
 *   TxLO/TxHI  0x04/0x08      latched counter value      (TRM 10.2/10.3)
 *   TxUPDATE   0x0C           latch counter into LO/HI   (TRM 10.4)
 *   TxALARMLO/HI 0x10/0x14    alarm target value         (TRM 10.5/10.6)
 *   TxLOADLO/HI  0x18/0x1C    reload value               (TRM 10.7/10.8)
 *   TxLOAD     0x20 + 0x24*x  write any value to reload  (TRM 10.9)
 */
#define TIMG_TXCONFIG_REG(x) (0x00 + 0x24 * (x))
#define TIMG_TXLO_REG(x) (0x04 + 0x24 * (x))
#define TIMG_TXHI_REG(x) (0x08 + 0x24 * (x))
#define TIMG_TXUPDATE_REG(x) (0x0C + 0x24 * (x))
#define TIMG_TXALARMLO_REG(x) (0x10 + 0x24 * (x))
#define TIMG_TXALARMHI_REG(x) (0x14 + 0x24 * (x))
#define TIMG_TXLOADLO_REG(x) (0x18 + 0x24 * (x))
#define TIMG_TXLOADHI_REG(x) (0x1C + 0x24 * (x))
#define TIMG_TXLOAD_REG(x) (0x20 + 0x24 * (x))

/**
 * MWDT, RTC calibration and interrupt registers — relative to TIMG0_BASE or
 * TIMG1_BASE (TRM 10.3)
 */
#define TIMG_WDTCONFIG0_REG 0x48  /* TRM 10.10 */
#define TIMG_WDTCONFIG1_REG 0x4C  /* TRM 10.11, prescaler */
#define TIMG_WDTCONFIG2_REG 0x50  /* TRM 10.12, stage 0 timeout */
#define TIMG_WDTCONFIG3_REG 0x54  /* TRM 10.13, stage 1 timeout */
#define TIMG_WDTCONFIG4_REG 0x58  /* TRM 10.14, stage 2 timeout */
#define TIMG_WDTCONFIG5_REG 0x5C  /* TRM 10.15, stage 3 timeout */
#define TIMG_WDTFEED_REG 0x60     /* TRM 10.16 */
#define TIMG_WDTWPROTECT_REG 0x64 /* TRM 10.17 */
#define TIMG_RTCCALICFG_REG 0x68  /* TRM 10.18 */
#define TIMG_RTCCALICFG1_REG 0x6C /* TRM 10.19 */
#define TIMG_INT_ENA_REG 0x98     /* TRM 10.20 */
#define TIMG_INT_RAW_REG 0x9C     /* TRM 10.21 */
#define TIMG_INT_ST_REG 0xA0      /* TRM 10.22 */
#define TIMG_INT_CLR_REG 0xA4     /* TRM 10.23 */

/**
 * RWDT registers — relative to RTC_CNTL_BASE (TRM 9.12-9.17, 9.30-9.33)
 */
#define RTC_CNTL_RESET_STATE_REG 0x34
#define RTC_CNTL_INT_ENA_REG 0x3C
#define RTC_CNTL_INT_RAW_REG 0x40
#define RTC_CNTL_INT_ST_REG 0x44
#define RTC_CNTL_INT_CLR_REG 0x48
#define RTC_CNTL_WDTCONFIG0_REG 0x8C  /* TRM 9.30 */
#define RTC_CNTL_WDTCONFIG1_REG 0x90  /* TRM 9.31, stage 0 timeout */
#define RTC_CNTL_WDTCONFIG2_REG 0x94  /* TRM 9.31, stage 1 timeout */
#define RTC_CNTL_WDTCONFIG3_REG 0x98  /* TRM 9.31, stage 2 timeout */
#define RTC_CNTL_WDTCONFIG4_REG 0x9C  /* TRM 9.31, stage 3 timeout */
#define RTC_CNTL_WDTFEED_REG 0xA0     /* TRM 9.32 */
#define RTC_CNTL_WDTWPROTECT_REG 0xA4 /* TRM 9.33 */

/**
 * Write-protection key — TRM 11.3.3
 *   WDTWPROTECT resets to this value (= writes unlocked). As soon as the
 *   register holds ANY other value, writes to every register of that watchdog
 *   are ignored — including the feed register.
 *   Required sequence: write key -> modify/feed -> write anything != key.
 */
#define WDT_WPROTECT_KEY 0x50D83AA1

/**
 * ===========================================================================
 * Stage actions — shared by MWDT and RWDT (TRM 11.3.2)
 *   The four stages run in a loop 0 -> 1 -> 2 -> 3 -> 0; feeding returns to
 *   stage 0. Each stage fires its action after its own timeout.
 * ===========================================================================
 */
#define WDT_STAGE_OFF 0     /* no effect on the system */
#define WDT_STAGE_INT 1     /* trigger an interrupt */
#define WDT_STAGE_CPU_RST 2 /* reset the designated CPU core(s) */
#define WDT_STAGE_SYS_RST 3 /* reset main system (RTC keeps running) */
#define WDT_STAGE_RTC_RST 4 /* reset main system + RTC — RWDT only */

/**
 * RTC calibration clock sources — TRM 10.18, RTCCALICFG [14:13]
 */
#define TIMG_RTC_CALI_CLK_SLOW 0       /* RTC_SLOW_CLK */
#define TIMG_RTC_CALI_CLK_RCFAST_DIV 1 /* RC_FAST_CLK / 256 */
#define TIMG_RTC_CALI_CLK_XTAL32K 2    /* external 32.768 kHz crystal */

/**
 * ===========================================================================
 * Register types
 * ===========================================================================
 */

/**
 * TIMGn_TxCONFIG_REG (0x00 + 0x24*x) — TRM 10.1, timer x configuration (R/W)
 *   Reset: 0x60002000 (disabled, counts up, autoreload on, divider field 1
 *                      = divide APB_CLK by 2)
 */
typedef union {
  struct {
    uint32_t reserved_0 : 10;
    /** [10] alarm enable (reset 0) — auto-cleared when the alarm fires */
    uint32_t alarm_en : 1;
    /** [11] level-type interrupt when an alarm occurs */
    uint32_t level_int_en : 1;
    /** [12] edge-type interrupt when an alarm occurs */
    uint32_t edge_int_en : 1;
    /** [28:13] prescaler divisor (reset 0x0001). Divides APB_CLK (80 MHz)
     *  to the time-base clock: 1 or 2 -> divide by 2, 0 -> divide by 65536,
     *  any other value n -> divide by n (TRM 10.2.1).
     *  Disable the timer (en = 0) before changing it — changing the divisor
     *  on an enabled timer gives unpredictable results. */
    uint32_t divider : 16;
    /** [29] reload from loadlo/loadhi when the alarm fires (reset 1) */
    uint32_t autoreload : 1;
    /** [30] 1: count up, 0: count down (reset 1) */
    uint32_t increase : 1;
    /** [31] enable the time-base counter (reset 0) — clearing it freezes
     *  the counter at its current value */
    uint32_t en : 1;
  } bits;
  uint32_t val;
} timg_timer_config_t;

/**
 * One 64-bit hardware timer — 9 registers, 0x24 bytes (TRM 10.1-10.9)
 */
typedef struct {
  timg_timer_config_t config; /* 0x00 TxCONFIG  — TRM 10.1 */
  uint32_t lo;      /* 0x04 TxLO      — TRM 10.2, latched low 32 bits (RO) */
  uint32_t hi;      /* 0x08 TxHI      — TRM 10.3, latched high 32 bits (RO) */
  uint32_t update;  /* 0x0C TxUPDATE  — TRM 10.4, write any value to latch the
                     *   live 64-bit counter into lo/hi (WO) */
  uint32_t alarmlo; /* 0x10 TxALARMLO — TRM 10.5, alarm target low 32 bits */
  uint32_t alarmhi; /* 0x14 TxALARMHI — TRM 10.6, alarm target high 32 bits */
  uint32_t loadlo;  /* 0x18 TxLOADLO  — TRM 10.7, reload value low 32 bits */
  uint32_t loadhi;  /* 0x1C TxLOADHI  — TRM 10.8, reload value high 32 bits */
  uint32_t load;    /* 0x20 TxLOAD    — TRM 10.9, write any value for an
                     *   instant software reload (WO) */
} timg_timer_t;

/**
 * TIMGn_WDTCONFIG0_REG (0x48) — TRM 10.10, MWDT configuration (R/W)
 *   Reset: 0x0004C800 (disabled, all stages off, both reset lengths 1,
 *                      flash-boot protection on)
 */
typedef union {
  struct {
    uint32_t reserved_0 : 14;
    /** [14] flash-boot protection (reset 1) — during boot stage 0 is armed
     *  to reset the system; clear this bit after boot (TRM 11.3.4) */
    uint32_t flashboot_mod_en : 1;
    /** [17:15] system reset pulse length (reset 1 = 200 ns):
     *  0:100ns 1:200ns 2:300ns 3:400ns 4:500ns 5:800ns 6:1.6us 7:3.2us */
    uint32_t sys_reset_length : 3;
    /** [20:18] CPU reset pulse length (same encoding as sys_reset_length) */
    uint32_t cpu_reset_length : 3;
    /** [21] level-type interrupt when a stage set to WDT_STAGE_INT expires */
    uint32_t level_int_en : 1;
    /** [22] edge-type interrupt when a stage set to WDT_STAGE_INT expires */
    uint32_t edge_int_en : 1;
    /** [24:23] stage 3 action (reset 0) — WDT_STAGE_*; only 0-3 valid here */
    uint32_t stg3 : 2;
    /** [26:25] stage 2 action (reset 0) */
    uint32_t stg2 : 2;
    /** [28:27] stage 1 action (reset 0) */
    uint32_t stg1 : 2;
    /** [30:29] stage 0 action (reset 0) — MWDT0 CPU reset hits PRO CPU,
     *  MWDT1 hits APP CPU (TRM 11.3.2) */
    uint32_t stg0 : 2;
    /** [31] enable the MWDT (reset 0) */
    uint32_t en : 1;
  } bits;
  uint32_t val;
} timg_wdt_config0_t;

/**
 * TIMGn_RTCCALICFG_REG (0x68) — TRM 10.18, RTC calibration configuration (R/W)
 */
typedef union {
  struct {
    uint32_t reserved_0 : 12;
    /** [12] start cycling mode (reset 1, TRM marks the field reserved) */
    uint32_t start_cycling : 1;
    /** [14:13] clock source: TIMG_RTC_CALI_CLK_* (reset 1) */
    uint32_t clk_sel : 2;
    /** [15] calibration ready (RO, reset 0) */
    uint32_t rdy : 1;
    /** [30:16] maximum calibration cycle count (reset 1) */
    uint32_t max : 15;
    /** [31] start calibration (reset 0) */
    uint32_t start : 1;
  } bits;
  uint32_t val;
} timg_rtccalicfg_t;

/**
 * TIMGn_INT_{ENA,RAW,ST,CLR}_REG — TRM 10.20-10.23
 *   Same three bits in all four registers; access differs:
 *   int_ena R/W, int_raw/int_st RO, int_clr WO (write 1 to clear — use .val).
 *   TRM reserves [31:3]; Espressif's register headers name bit [3] LACT.
 */
typedef union {
  struct {
    /** timer 0 alarm interrupt */
    uint32_t t0 : 1;
    /** timer 1 alarm interrupt */
    uint32_t t1 : 1;
    /** watchdog interrupt (raised when a stage set to WDT_STAGE_INT expires) */
    uint32_t wdt : 1;
    uint32_t reserved_3 : 29;
  } bits;
  uint32_t val;
} timg_int_t;

/**
 * RTC_CNTL_RESET_STATE_REG (0x34) — TRM 9.12, reset state (mixed access)
 */
typedef union {
  struct {
    /** [5:0] reset cause of PRO CPU (RO) — values TRM Table 7.1-1, p. 166:
     *  0x01 chip power-on (system)   0x10 RWDT (system)   0x0F brown-out
     *  0x03 software system          0x07 MWDT0 global    0x08 MWDT1 global
     *  0x09 RWDT (core)              0x0D RWDT CPU        0x05 deep sleep */
    uint32_t cause_procpu : 6;
    /** [11:6] reset cause of APP CPU (RO), same encoding */
    uint32_t cause_appcpu : 6;
    /** [12] APP CPU state vector select (reset 1) */
    uint32_t appcpu_vector_sel : 1;
    /** [13] PRO CPU state vector select (reset 1) */
    uint32_t procpu_vector_sel : 1;
    uint32_t reserved_14 : 18;
  } bits;
  uint32_t val;
} rtc_cntl_reset_state_t;

/**
 * RTC_CNTL_INT_{ENA,RAW,ST,CLR}_REG — TRM 9.14-9.17
 *   Same bits in all four registers; access differs: int_ena R/W,
 *   int_raw/int_st RO, int_clr WO (write 1 to clear — use .val).
 */
typedef union {
  struct {
    /** sleep wakeup interrupt */
    uint32_t slp_wakeup : 1;
    /** sleep reject interrupt */
    uint32_t slp_reject : 1;
    /** SDIO idle interrupt */
    uint32_t sdio_idle : 1;
    /** RWDT interrupt (raised when a stage set to WDT_STAGE_INT expires) */
    uint32_t wdt : 1;
    /** RTC time valid interrupt */
    uint32_t time_valid : 1;
    /** ULP coprocessor interrupt */
    uint32_t ulp_cp : 1;
    /** touch interrupt */
    uint32_t touch : 1;
    /** brown-out interrupt */
    uint32_t brown_out : 1;
    /** RTC main timer interrupt */
    uint32_t main_timer : 1;
    uint32_t reserved_9 : 23;
  } bits;
  uint32_t val;
} rtc_cntl_int_t;

/**
 * RTC_CNTL_WDTCONFIG0_REG (0x8C) — TRM 9.30, RWDT configuration (R/W)
 *   Reset: 0x00004C80 (disabled, stages off, both reset lengths 1,
 *                      flash-boot protection on, paused in sleep)
 *   Stages here are 3-bit because the RWDT can also reset the RTC itself.
 */
typedef union {
  struct {
    uint32_t reserved_0 : 7;
    /** [7] pause the RWDT in sleep (reset 1) */
    uint32_t pause_in_slp : 1;
    /** [8] CPU-reset stages also reset the APP CPU (reset 1) */
    uint32_t appcpu_reset_en : 1;
    /** [9] CPU-reset stages also reset the PRO CPU (reset 1) */
    uint32_t procpu_reset_en : 1;
    /** [10] flash-boot protection (reset 1) — during boot stage 0 is armed
     *  to reset the RTC; clear this bit after boot (TRM 11.3.4) */
    uint32_t flashboot_mod_en : 1;
    /** [13:11] system reset pulse length, RTC_SLOW_CLK cycles (reset 1) */
    uint32_t sys_reset_length : 3;
    /** [16:14] CPU reset pulse length, RTC_SLOW_CLK cycles (reset 1) */
    uint32_t cpu_reset_length : 3;
    /** [18:17] reserved (reset 0) — Espressif's register headers name these
     *  bits level_int_en/edge_int_en; TRM v5.9 marks them reserved */
    uint32_t reserved_17 : 2;
    /** [21:19] stage 3 action (reset 0) — WDT_STAGE_* (0-4 valid) */
    uint32_t stg3 : 3;
    /** [24:22] stage 2 action (reset 0) */
    uint32_t stg2 : 3;
    /** [27:25] stage 1 action (reset 0) */
    uint32_t stg1 : 3;
    /** [30:28] stage 0 action (reset 0) — action 4 (reset main system +
     *  RTC) exists only on the RWDT (TRM 11.3.2) */
    uint32_t stg0 : 3;
    /** [31] enable the RWDT (reset 0) */
    uint32_t en : 1;
  } bits;
  uint32_t val;
} rtc_cntl_wdt_config0_t;

/**
 * ===========================================================================
 * Device maps
 * ===========================================================================
 */

/**
 * TIMGn register block — 0x00-0xFF at TIMG0_BASE/TIMG1_BASE (TRM 10.3-10.4)
 *   volatile applies to the whole block: every access through these
 *   pointers is a volatile (uncached) access, as required for MMIO.
 */
typedef volatile struct {
  timg_timer_t timer[2]; /* 0x00-0x47, timers T0 (0x00) and T1 (0x24) */

  /** 0x48 TIMGn_WDTCONFIG0_REG — TRM 10.10, MWDT configuration */
  timg_wdt_config0_t wdtconfig0;
  /** 0x4C TIMGn_WDTCONFIG1_REG — TRM 10.11, prescaler (R/W)
   *  [31:16] WDT_CLK_PRESCALE (reset 0x0001): MWDT tick = APB_CLK / prescale,
   *  so a stage lasts wdtconfigN * prescale / 80 MHz (prescale 12.5 ns).
   *  [15:0] reserved */
  uint32_t wdtconfig1;
  /** 0x50 TIMGn_WDTCONFIG2_REG — TRM 10.12, stage 0 timeout in MWDT clock
   *  cycles (reset 26000000 = 0.325 s at prescale 1) */
  uint32_t wdtconfig2;
  /** 0x54 TIMGn_WDTCONFIG3_REG — TRM 10.13, stage 1 timeout (reset 0x07FFFFFF)
   */
  uint32_t wdtconfig3;
  /** 0x58 TIMGn_WDTCONFIG4_REG — TRM 10.14, stage 2 timeout (reset 0x000FFFFF)
   */
  uint32_t wdtconfig4;
  /** 0x5C TIMGn_WDTCONFIG5_REG — TRM 10.15, stage 3 timeout (reset 0x000FFFFF)
   */
  uint32_t wdtconfig5;
  /** 0x60 TIMGn_WDTFEED_REG — TRM 10.16 (WO): write any value to feed the
   *  MWDT — the stage machine returns to stage 0 and the expiry counter
   *  restarts. Ignored while write protection is on. */
  uint32_t wdtfeed;
  /** 0x64 TIMGn_WDTWPROTECT_REG — TRM 10.17: reset WDT_WPROTECT_KEY
   *  (= unlocked); any other value write-protects every MWDT register. */
  uint32_t wdtwprotect;

  /** 0x68 TIMGn_RTCCALICFG_REG — TRM 10.18, RTC calibration configuration */
  timg_rtccalicfg_t rtccalicfg;
  /** 0x6C TIMGn_RTCCALICFG1_REG — TRM 10.19 (RO): [31:7] CALI_VALUE,
   *  [6:0] reserved */
  uint32_t rtccalicfg1;

  /** 0x70-0x94 not defined in TRM v5.9 (Espressif's register headers map a
   *  LACT block here); kept as reserved padding */
  uint32_t reserved_cali[10];

  /** 0x98 TIMGn_INT_ENA_REG — TRM 10.20, interrupt enable bits (R/W) */
  timg_int_t int_ena;
  /** 0x9C TIMGn_INT_RAW_REG — TRM 10.21, raw interrupt status (RO) */
  timg_int_t int_raw;
  /** 0xA0 TIMGn_INT_ST_REG — TRM 10.22, masked status = raw AND enable (RO) */
  timg_int_t int_st;
  /** 0xA4 TIMGn_INT_CLR_REG — TRM 10.23, interrupt clear (WO, write 1 to
   *  clear): TIMG0->int_clr.val = 1u << 2; */
  timg_int_t int_clr;

  /** 0xA8-0xF7 reserved */
  uint32_t reserved_tail[20];

  /** 0xF8 version/date register, not in TRM v5.9 (per Espressif's register
   *  headers; reset 0x01907261) */
  uint32_t timers_date;
  /** 0xFC register clock gate, not in TRM v5.9: [31] CLK_EN — 1: register
   *  access allowed, 0: register access blocked */
  uint32_t regclk;
} timg_dev_t;

/**
 * RTC_CNTL register block — the WATCHDOG slice of RTC_CNTL_BASE
 * (TRM 9.12-9.33). This is a partial map: unnamed regions (sleep, power,
 * wakeup) are reserved padding here. Full block: ~0xA8 is enough for all RWDT
 * and reset/interrupt registers above.
 */
typedef volatile struct {
  uint32_t reserved_00[13]; /* 0x00-0x33 sleep/power/wakeup (not mapped) */

  /** 0x34 RTC_CNTL_RESET_STATE_REG — TRM 9.12, reset state */
  rtc_cntl_reset_state_t reset_state;
  uint32_t reserved_38; /* 0x38 wakeup state (TRM 9.13, not mapped) */

  /** 0x3C RTC_CNTL_INT_ENA_REG — TRM 9.14, interrupt enable bits (R/W) */
  rtc_cntl_int_t int_ena;
  /** 0x40 RTC_CNTL_INT_RAW_REG — TRM 9.15, raw interrupt status (RO) */
  rtc_cntl_int_t int_raw;
  /** 0x44 RTC_CNTL_INT_ST_REG — TRM 9.16, masked status (RO) */
  rtc_cntl_int_t int_st;
  /** 0x48 RTC_CNTL_INT_CLR_REG — TRM 9.17, interrupt clear (WO, write 1 to
   *  clear): RTC_CNTL->int_clr.val = 1u << 3; */
  rtc_cntl_int_t int_clr;

  uint32_t reserved_4C[16]; /* 0x4C-0x8B power/wakeup (not mapped) */

  /** 0x8C RTC_CNTL_WDTCONFIG0_REG — TRM 9.30, RWDT configuration */
  rtc_cntl_wdt_config0_t wdtconfig0;
  /** 0x90 RTC_CNTL_WDTCONFIG1_REG — TRM 9.31, stage 0 hold time in
   *  RTC_SLOW_CLK cycles (reset 0x00000FFF = 4095 ≈ 27 ms at 150 kHz) */
  uint32_t wdtconfig1;
  /** 0x94 RTC_CNTL_WDTCONFIG2_REG — TRM 9.31, stage 1 hold time */
  uint32_t wdtconfig2;
  /** 0x98 RTC_CNTL_WDTCONFIG3_REG — TRM 9.31, stage 2 hold time */
  uint32_t wdtconfig3;
  /** 0x9C RTC_CNTL_WDTCONFIG4_REG — TRM 9.31, stage 3 hold time */
  uint32_t wdtconfig4;
  /** 0xA0 RTC_CNTL_WDTFEED_REG — TRM 9.32 (WO): bit [31] WDT_FEED — write
   *  1<<31 to feed the RWDT (ignored while write protection is on). */
  uint32_t wdtfeed;
  /** 0xA4 RTC_CNTL_WDTWPROTECT_REG — TRM 9.33: reset WDT_WPROTECT_KEY
   *  (= unlocked); any other value write-protects every RWDT register. */
  uint32_t wdtwprotect;
} rtc_cntl_t;

/* Layout guards — pinned to the TRM offsets above */
_Static_assert(sizeof(timg_timer_t) == 0x24, "timer block must be 0x24 bytes");
_Static_assert(sizeof(timg_dev_t) == 0x100, "TIMG map must span 0x00-0xFF");
_Static_assert(offsetof(timg_dev_t, wdtconfig0) == 0x48, "MWDT config at 0x48");
_Static_assert(offsetof(timg_dev_t, rtccalicfg) == 0x68, "cali at 0x68");
_Static_assert(offsetof(timg_dev_t, int_ena) == 0x98, "INT_ENA at 0x98");
_Static_assert(offsetof(timg_dev_t, int_clr) == 0xA4, "INT_CLR at 0xA4");
_Static_assert(offsetof(timg_dev_t, timers_date) == 0xF8,
               "timers_date at 0xF8");
_Static_assert(sizeof(rtc_cntl_t) == 0xA8,
               "RTC_CNTL slice must span 0x00-0xA7");
_Static_assert(offsetof(rtc_cntl_t, reset_state) == 0x34,
               "RESET_STATE at 0x34");
_Static_assert(offsetof(rtc_cntl_t, int_ena) == 0x3C, "INT_ENA at 0x3C");
_Static_assert(offsetof(rtc_cntl_t, wdtconfig0) == 0x8C, "RWDT config at 0x8C");
_Static_assert(offsetof(rtc_cntl_t, wdtwprotect) == 0xA4, "WPROTECT at 0xA4");

/**
 * Peripheral instances
 */
#define TIMG0 ((timg_dev_t *)TIMG0_BASE)
#define TIMG1 ((timg_dev_t *)TIMG1_BASE)
#define RTC_CNTL ((rtc_cntl_t *)RTC_CNTL_BASE)

/**
 * ===========================================================================
 * Watchdog helpers — unlock/feed/lock around WDT_WPROTECT_KEY (TRM 11.3.3)
 *   Feeding automatically returns the stage machine to stage 0.
 * ===========================================================================
 */

static inline void mwdt_unlock(timg_dev_t *tmg) {
  tmg->wdtwprotect = WDT_WPROTECT_KEY;
}

static inline void mwdt_lock(timg_dev_t *tmg) { tmg->wdtwprotect = 0; }

static inline void mwdt_feed(timg_dev_t *tmg) {
  mwdt_unlock(tmg);
  tmg->wdtfeed = 1u;
  mwdt_lock(tmg);
}

static inline void rwdt_unlock(void) {
  RTC_CNTL->wdtwprotect = WDT_WPROTECT_KEY;
}

static inline void rwdt_lock(void) { RTC_CNTL->wdtwprotect = 0; }

static inline void rwdt_feed(void) {
  rwdt_unlock();
  RTC_CNTL->wdtfeed = 1u << 31;
  rwdt_lock();
}

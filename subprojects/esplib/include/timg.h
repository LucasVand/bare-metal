#pragma once

#include <stddef.h>
#include <stdint.h>

/**
 * ===========================================================================
 * Timer Group (TIMG) registers — ESP32-C6 TRM v1.2
 *
 *   Hardware timer T0 (one per group, 54-bit counter, 16-bit prescaler,
 *  alarms)                                   Chapter 14.1-14.4 (p. 518) RTC
 *  slow-clock calibration (per group)                 TRM 14.3.6, 14.4.5 MWDT
 *  (one main watchdog per group)                      TRM 14.5-14.6, 15.1-15.4
 *  RWDT (RTC watchdog, its own peripheral)            TRM 15.1-15.2 (p. 543),
 *  15.5-15.6 Super watchdog (SWD)                            TRM 15.3
 *  Interrupts (per group)                             TRM 14.3.7
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
 *   - trigger registers where the read has a side effect or reads as 0
 *     (update, load, wdtfeed, divcnt_rst, conf_update_en, ...)
 *   - fields that self-clear (alarm_en, ...)
 * Write those with `.val` instead, e.g. TIMG0->int_clr.val = 1u << 0;
 *
 * MWDT configuration update — the C6 MWDT latches WDTCONFIG0-5 and WDT_EN
 * only when TIMG_WDT_CONF_UPDATE_EN (wdtconfig0 bit 22, write-trigger) is
 * set after the last change (TRM 14.10); see mwdt_conf_update() below.
 *
 * Clock sources are selected outside this block, in the PCR registers
 * (0x6009_6000): T0 via PCR_TG0_TIMER_CLK_SEL (0: XTAL, 1: PLL_F80M,
 * 2: RC_FAST) and PCR_TG0_TIMER_CLK_EN; the MWDT via PCR_TG0_WDT_CLK_SEL /
 * PCR_TG0_WDT_CLK_EN (TRM 14.3.1, 15.2.2.1).
 *
 * Layout safety — the static assertions at the bottom of this file pin every
 * struct to the TRM offsets at compile time; a layout mistake fails the build.
 * ===========================================================================
 */

/**
 * Peripheral base addresses — TRM Table 5.3-2 (p. 175)
 *   TIMG0_BASE hosts MWDT0, TIMG1_BASE hosts MWDT1 — identical register
 *   offsets on both, so every "TIMGn_" offset below works with either base.
 *   The RWDT lives in its own peripheral (RTC_WDT), not inside a timer group.
 */
#define TIMG0_BASE 0x60008000
#define TIMG1_BASE 0x60009000
#define RTC_WDT_BASE 0x600B1C00

/**
 * ===========================================================================
 * Register offsets — for code that works with raw addresses/pointers
 * ===========================================================================
 */

/**
 * Timer x registers (x: 0 — one timer per group) — base 0x00 + 0x24*x
 * (TRM 14.5)
 *   TxCONFIG   0x00 + 0x24*x  timer configuration        (TRM 14.1)
 *   TxLO/TxHI  0x04/0x08      latched counter value, low 32 / high 22 bits
 *                             (TRM 14.2/14.3)
 *   TxUPDATE   0x0C           latch counter into LO/HI   (TRM 14.4)
 *   TxALARMLO/HI 0x10/0x14    alarm target value         (TRM 14.5/14.6)
 *   TxLOADLO/HI  0x18/0x1C    reload value               (TRM 14.7/14.8)
 *   TxLOAD     0x20 + 0x24*x  write any value to reload  (TRM 14.9)
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
 * MWDT and RTC calibration registers — relative to TIMG0_BASE or TIMG1_BASE
 * (TRM 14.5)
 */
#define TIMG_WDTCONFIG0_REG 0x48  /* TRM 14.10 */
#define TIMG_WDTCONFIG1_REG 0x4C  /* TRM 14.11, prescaler */
#define TIMG_WDTCONFIG2_REG 0x50  /* TRM 14.12, stage 0 timeout */
#define TIMG_WDTCONFIG3_REG 0x54  /* TRM 14.13, stage 1 timeout */
#define TIMG_WDTCONFIG4_REG 0x58  /* TRM 14.14, stage 2 timeout */
#define TIMG_WDTCONFIG5_REG 0x5C  /* TRM 14.15, stage 3 timeout */
#define TIMG_WDTFEED_REG 0x60     /* TRM 14.16 */
#define TIMG_WDTWPROTECT_REG 0x64 /* TRM 14.17 */
#define TIMG_RTCCALICFG_REG 0x68  /* TRM 14.18 */
#define TIMG_RTCCALICFG1_REG 0x6C /* TRM 14.19 */
#define TIMG_INT_ENA_TIMERS_REG 0x70  /* TRM 14.21 */
#define TIMG_INT_RAW_TIMERS_REG 0x74  /* TRM 14.22 */
#define TIMG_INT_ST_TIMERS_REG 0x78   /* TRM 14.23 */
#define TIMG_INT_CLR_TIMERS_REG 0x7C  /* TRM 14.24 */
#define TIMG_RTCCALICFG2_REG 0x80 /* TRM 14.20, calibration timeout */

/**
 * RWDT registers — relative to RTC_WDT_BASE (TRM 15.5, 15.1-15.14)
 */
#define RTC_WDT_CONFIG0_REG 0x00   /* TRM 15.1 */
#define RTC_WDT_CONFIG1_REG 0x04   /* TRM 15.2, stage 0 timeout */
#define RTC_WDT_CONFIG2_REG 0x08   /* TRM 15.3, stage 1 timeout */
#define RTC_WDT_CONFIG3_REG 0x0C   /* TRM 15.4, stage 2 timeout */
#define RTC_WDT_CONFIG4_REG 0x10   /* TRM 15.5, stage 3 timeout */
#define RTC_WDT_FEED_REG 0x14      /* TRM 15.6 */
#define RTC_WDT_WPROTECT_REG 0x18  /* TRM 15.7 */
#define RTC_WDT_SWD_CONFIG_REG 0x1C    /* TRM 15.8, super watchdog */
#define RTC_WDT_SWD_WPROTECT_REG 0x20  /* TRM 15.9 */
#define RTC_WDT_INT_RAW_REG 0x24   /* TRM 15.10 */
#define RTC_WDT_INT_ST_REG 0x28    /* TRM 15.11 */
#define RTC_WDT_INT_ENA_REG 0x2C   /* TRM 15.12 */
#define RTC_WDT_INT_CLR_REG 0x30   /* TRM 15.13 */
#define RTC_WDT_DATE_REG 0x3FC     /* TRM 15.14 */

/**
 * Write-protection key — TRM 15.2.2.3
 *   WDTWPROTECT/WPROTECT: write this value to unlock every register of that
 *   watchdog (feed included); ANY other value re-enables write protection.
 *   Required sequence: write key -> modify/feed -> write anything != key.
 */
#define WDT_WPROTECT_KEY 0x50D83AA1

/**
 * ===========================================================================
 * Stage actions — shared by MWDT and RWDT (TRM 15.2.2.2, Table 15.2-1)
 *   The four stages run in a loop 0 -> 1 -> 2 -> 3 -> 0; feeding returns to
 *   stage 0. Each stage fires its action after its own timeout.
 * ===========================================================================
 */
#define WDT_STAGE_OFF 0     /* no effect on the system */
#define WDT_STAGE_INT 1     /* trigger an interrupt */
#define WDT_STAGE_CPU_RST 2 /* reset the CPU core */
#define WDT_STAGE_CORE_RST 3    /* reset the main system (PMU and RTC
                                 *  peripherals keep running) */
#define WDT_STAGE_SYS_RST 4     /* reset main system + PMU + RTC — RWDT only */

/**
 * RTC calibration clock sources — TRM 14.18, RTCCALICFG [14:13]
 *   RC_FAST_CLK_DIV32 is derived from RC_FAST_CLK by a fixed 1/32 divider
 *   (TRM 14.3.6).
 */
#define TIMG_RTC_CALI_CLK_RC_SLOW 0      /* RC_SLOW_CLK */
#define TIMG_RTC_CALI_CLK_RCFAST_DIV32 1 /* RC_FAST_CLK / 32 */
#define TIMG_RTC_CALI_CLK_XTAL32K 2      /* external 32.768 kHz crystal */

/**
 * ===========================================================================
 * Register types
 * ===========================================================================
 */

/**
 * TIMGn_T0CONFIG_REG (0x00 + 0x24*x) — TRM 14.1, timer configuration (R/W)
 *   Reset: 0x60002000 (disabled, counts up, autoreload on, divider field 1
 *                      = divide the selected T0_clk by 2)
 */
typedef union {
  struct {
    uint32_t reserved_0 : 10;
    /** [10] alarm enable (reset 0, R/W/SC) — auto-cleared when the alarm
     *  fires */
    uint32_t alarm_en : 1;
    uint32_t reserved_11 : 1;
    /** [12] write 1 to reset the timer's clock-divider counter (WT) — do
     *  this after changing the divider, with the timer disabled (TRM 14.3.1) */
    uint32_t divcnt_rst : 1;
    /** [28:13] prescaler divisor (reset 0x0001). Divides the selected T0_clk
     *  to the time-base clock: 0 -> divide by 65536, 1 or 2 -> divide by 2,
     *  any other value n -> divide by n (TRM 14.3.1).
     *  Disable the timer (en = 0) before changing it, then set divcnt_rst —
     *  changing the divisor on an enabled timer gives unpredictable
     *  results. */
    uint32_t divider : 16;
    /** [29] reload from loadlo/loadhi when the alarm fires (reset 1) */
    uint32_t autoreload : 1;
    /** [30] 1: count up, 0: count down (reset 1) */
    uint32_t increase : 1;
    /** [31] enable the time-base counter (reset 0, R/W/SS/SC) — clearing it
     *  freezes the counter at its current value */
    uint32_t en : 1;
  } bits;
  uint32_t val;
} timg_timer_config_t;

/**
 * One 64-bit hardware timer (54-bit counter) — 9 registers, 0x24 bytes
 * (TRM 14.1-14.9)
 */
typedef struct {
  timg_timer_config_t config; /* 0x00 TxCONFIG  — TRM 14.1 */
  uint32_t lo;      /* 0x04 TxLO      — TRM 14.2, latched low 32 bits (RO) */
  uint32_t hi;      /* 0x08 TxHI      — TRM 14.3, latched high 22 bits (RO) */
  uint32_t update;  /* 0x0C TxUPDATE  — TRM 14.4 (R/W/SC): write any value
                     *   to latch the live 54-bit counter into lo/hi; it
                     *   reads 0 again once the latch is complete */
  uint32_t alarmlo; /* 0x10 TxALARMLO — TRM 14.5, alarm target low 32 bits */
  uint32_t alarmhi; /* 0x14 TxALARMHI — TRM 14.6, alarm target high 22 bits */
  uint32_t loadlo;  /* 0x18 TxLOADLO  — TRM 14.7, reload value low 32 bits */
  uint32_t loadhi;  /* 0x1C TxLOADHI  — TRM 14.8, reload value high 22 bits */
  uint32_t load;    /* 0x20 TxLOAD    — TRM 14.9 (WT), write any value for
                     *   an instant software reload */
} timg_timer_t;

/**
 * TIMGn_WDTCONFIG0_REG (0x48) — TRM 14.10, MWDT configuration (R/W)
 *   Reset: 0x00064000 (disabled, all stages off, both reset lengths 1,
 *                      flash-boot protection on)
 *   Config changes only take effect after conf_update_en is set once.
 */
typedef union {
  struct {
    uint32_t reserved_0 : 12;
    /** [12] unmask the CPU reset generated by the MWDT (reset 0 = masked) */
    uint32_t appcpu_reset_en : 1;
    /** [13] unmask the CPU reset generated by the MWDT (reset 0 = masked) */
    uint32_t procpu_reset_en : 1;
    /** [14] flash-boot protection (reset 1) — during boot stage 0 is armed
     *  to reset the core; clear this bit after boot (TRM 15.2.2.4) */
    uint32_t flashboot_mod_en : 1;
    /** [17:15] system (core) reset pulse length in mwdt_clk cycles
     *  (reset 1): 0:8 1:16 2:24 3:32 4:40 5:64 6:128 7:256 */
    uint32_t sys_reset_length : 3;
    /** [20:18] CPU reset pulse length in mwdt_clk cycles (same encoding) */
    uint32_t cpu_reset_length : 3;
    uint32_t reserved_21 : 1;
    /** [22] write-trigger (WT): set to 1 to apply the WDT configuration
     *  registers written before it */
    uint32_t conf_update_en : 1;
    /** [24:23] stage 3 action (reset 0) — WDT_STAGE_*; only 0-3 valid here */
    uint32_t stg3 : 2;
    /** [26:25] stage 2 action (reset 0) */
    uint32_t stg2 : 2;
    /** [28:27] stage 1 action (reset 0) */
    uint32_t stg1 : 2;
    /** [30:29] stage 0 action (reset 0) */
    uint32_t stg0 : 2;
    /** [31] enable the MWDT (reset 0) */
    uint32_t en : 1;
  } bits;
  uint32_t val;
} timg_wdt_config0_t;

/**
 * TIMGn_RTCCALICFG_REG (0x68) — TRM 14.18, RTC calibration configuration (R/W)
 *   Reset: 0x00011000 (periodic mode on, source RC_SLOW_CLK, max 1)
 */
typedef union {
  struct {
    uint32_t reserved_0 : 12;
    /** [12] periodic frequency calculation (reset 1); 0 = one-shot */
    uint32_t start_cycling : 1;
    /** [14:13] clock source: TIMG_RTC_CALI_CLK_* (reset 0 = RC_SLOW_CLK) */
    uint32_t clk_sel : 2;
    /** [15] one-shot calibration ready (RO, reset 0) */
    uint32_t rdy : 1;
    /** [30:16] calibration time in XTAL_CLK cycles (reset 1) */
    uint32_t max : 15;
    /** [31] start one-shot calibration (reset 0) */
    uint32_t start : 1;
  } bits;
  uint32_t val;
} timg_rtccalicfg_t;

/**
 * TIMGn_RTCCALICFG2_REG (0x80) — TRM 14.20, RTC calibration timeout (R/W)
 *   Reset: 0xFFFFFF98 (thres 0x1FFFFFF, rst_cnt 3)
 */
typedef union {
  struct {
    /** [0] calibration timeout flag (RO, reset 0) */
    uint32_t timeout : 1;
    uint32_t reserved_1 : 2;
    /** [6:3] timeout counter reset cycles in XTAL_CLK (reset 3) */
    uint32_t timeout_rst_cnt : 4;
    /** [31:7] timeout threshold in XTAL_CLK cycles (reset 0x1FFFFFF) */
    uint32_t timeout_thres : 25;
  } bits;
  uint32_t val;
} timg_rtccalicfg2_t;

/**
 * TIMGn_INT_{ENA,RAW,ST,CLR}_TIMERS_REG — TRM 14.21-14.24
 *   Same two bits in all four registers; access differs:
 *   int_ena R/W, int_raw/int_st RO, int_clr WO (write 1 to clear — use .val).
 *   The C6 timer group has one timer, so there is no T1 bit.
 */
typedef union {
  struct {
    /** timer 0 alarm interrupt */
    uint32_t t0 : 1;
    /** watchdog interrupt (raised when a stage set to WDT_STAGE_INT expires) */
    uint32_t wdt : 1;
    uint32_t reserved_2 : 30;
  } bits;
  uint32_t val;
} timg_int_t;

/**
 * RTC_WDT_CONFIG0_REG (0x00) — TRM 15.1, RWDT configuration (R/W)
 *   Reset: 0x00013200 (disabled, stages off, both reset lengths 1,
 *                      flash-boot protection on, paused in sleep)
 *   Stages here are 3-bit because the RWDT can also reset the RTC itself
 *   (system reset, action 4).
 */
typedef union {
  struct {
    uint32_t reserved_0 : 9;
    /** [9] pause the RWDT in sleep (reset 1) */
    uint32_t pause_in_slp : 1;
    uint32_t reserved_10 : 1;
    /** [11] enable RWDT CPU resets (reset 0) */
    uint32_t procpu_reset_en : 1;
    /** [12] flash-boot protection (reset 1) — during boot stage 0 is armed
     *  to reset the system; clear this bit after boot (TRM 15.2.2.4) */
    uint32_t flashboot_mod_en : 1;
    /** [15:13] system reset pulse length, RTC_DYN_FAST_CLK cycles (reset 1) */
    uint32_t sys_reset_length : 3;
    /** [18:16] CPU reset pulse length, RTC_DYN_FAST_CLK cycles (reset 1) */
    uint32_t cpu_reset_length : 3;
    /** [21:19] stage 3 action (reset 0) — WDT_STAGE_* (0-4 valid) */
    uint32_t stg3 : 3;
    /** [24:22] stage 2 action (reset 0) */
    uint32_t stg2 : 3;
    /** [27:25] stage 1 action (reset 0) */
    uint32_t stg1 : 3;
    /** [30:28] stage 0 action (reset 0) — action 4 (system reset) exists
     *  only on the RWDT (TRM 15.2.2.2) */
    uint32_t stg0 : 3;
    /** [31] enable the RWDT (reset 0) */
    uint32_t en : 1;
  } bits;
  uint32_t val;
} rtc_wdt_config0_t;

/**
 * RTC_WDT_SWD_CONFIG_REG (0x1C) — TRM 15.8, super watchdog configuration
 */
typedef union {
  struct {
    /** [0] SWD reset signal was generated (RO, reset 0) */
    uint32_t reset_flag : 1;
    uint32_t reserved_1 : 17;
    /** [18] feed the SWD automatically by hardware (reset 0) */
    uint32_t auto_feed_en : 1;
    /** [19] write 1 to clear the SWD reset flag (WT) */
    uint32_t rst_flag_clr : 1;
    /** [29:20] SWD signal length to the analog circuit in RTC_DYN_FAST_CLK
     *  cycles (reset 300) */
    uint32_t signal_width : 10;
    /** [30] disable the SWD (reset 0) */
    uint32_t disable : 1;
    /** [31] write 1 to feed the SWD (WT) */
    uint32_t feed : 1;
  } bits;
  uint32_t val;
} rtc_wdt_swd_config_t;

/**
 * RTC_WDT_INT_{RAW,ST,ENA,CLR}_REG — TRM 15.10-15.13
 *   Same two bits in all four registers; access differs:
 *   int_ena R/W, int_raw R/WTC/SS, int_st RO, int_clr WO (write 1 to clear
 *   — use .val).
 */
typedef union {
  struct {
    uint32_t reserved_0 : 30;
    /** SWD about-to-timeout interrupt */
    uint32_t swd_int : 1;
    /** RWDT timeout interrupt (stage set to WDT_STAGE_INT expired) */
    uint32_t wdt_int : 1;
  } bits;
  uint32_t val;
} rtc_wdt_int_t;

/**
 * ===========================================================================
 * Device maps
 * ===========================================================================
 */

/**
 * TIMGn register block — 0x00-0xFF at TIMG0_BASE/TIMG1_BASE
 * (TRM 14.5, 14.6)
 *   volatile applies to the whole block: every access through these
 *   pointers is a volatile (uncached) access, as required for MMIO.
 */
typedef volatile struct {
  timg_timer_t timer[1]; /* 0x00-0x23, timer T0 — the C6 group has one timer */
  uint32_t reserved_24[9]; /* 0x24-0x47 reserved */

  /** 0x48 TIMGn_WDTCONFIG0_REG — TRM 14.10, MWDT configuration */
  timg_wdt_config0_t wdtconfig0;
  /** 0x4C TIMGn_WDTCONFIG1_REG — TRM 14.11, prescaler (R/W)
   *  [31:16] WDT_CLK_PRESCALE (reset 0x0001): MWDT clock period =
   *  clock source period * prescale (TRM 15.2.2.1).
   *  [0] WDT_DIVCNT_RST (WT): write 1 to reset the prescaler counter.
   *  [15:1] reserved */
  uint32_t wdtconfig1;
  /** 0x50 TIMGn_WDTCONFIG2_REG — TRM 14.12, stage 0 timeout in mwdt_clk
   *  cycles (reset 26000000) */
  uint32_t wdtconfig2;
  /** 0x54 TIMGn_WDTCONFIG3_REG — TRM 14.13, stage 1 timeout (reset 0x7FFFFFF)
   */
  uint32_t wdtconfig3;
  /** 0x58 TIMGn_WDTCONFIG4_REG — TRM 14.14, stage 2 timeout (reset 0x0FFFFF)
   */
  uint32_t wdtconfig4;
  /** 0x5C TIMGn_WDTCONFIG5_REG — TRM 14.15, stage 3 timeout (reset 0x0FFFFF)
   */
  uint32_t wdtconfig5;
  /** 0x60 TIMGn_WDTFEED_REG — TRM 14.16 (WT): write any value to feed the
   *  MWDT — the stage machine returns to stage 0 and the counter
   *  restarts. Ignored while write protection is on. */
  uint32_t wdtfeed;
  /** 0x64 TIMGn_WDTWPROTECT_REG — TRM 14.17: reset WDT_WPROTECT_KEY
   *  (= unlocked); any other value write-protects every MWDT register. */
  uint32_t wdtwprotect;

  /** 0x68 TIMGn_RTCCALICFG_REG — TRM 14.18, RTC calibration configuration */
  timg_rtccalicfg_t rtccalicfg;
  /** 0x6C TIMGn_RTCCALICFG1_REG — TRM 14.19 (RO): [31:7] CALI_VALUE,
   *  [0] CYCLING_DATA_VLD, [6:1] reserved */
  uint32_t rtccalicfg1;

  /** 0x70 TIMGn_INT_ENA_TIMERS_REG — TRM 14.21, interrupt enable (R/W) */
  timg_int_t int_ena;
  /** 0x74 TIMGn_INT_RAW_TIMERS_REG — TRM 14.22, raw interrupt status */
  timg_int_t int_raw;
  /** 0x78 TIMGn_INT_ST_TIMERS_REG — TRM 14.23, masked status =
   *  raw AND enable (RO) */
  timg_int_t int_st;
  /** 0x7C TIMGn_INT_CLR_TIMERS_REG — TRM 14.24, interrupt clear (WO, write
   *  1 to clear): TIMG0->int_clr.val = 1u << 0; */
  timg_int_t int_clr;

  /** 0x80 TIMGn_RTCCALICFG2_REG — TRM 14.20, calibration timeout */
  timg_rtccalicfg2_t rtccalicfg2;

  /** 0x84-0xF7 reserved */
  uint32_t reserved_84[29];

  /** 0xF8 TIMGn_NTIMERS_DATE_REG — TRM 14.25, version register
   *  (reset 0x02206072) */
  uint32_t ntimers_date;
  /** 0xFC TIMGn_REGCLK_REG — TRM 14.26, register clock gate:
   *  [31] CLK_EN — 0: force register clock on, 1: clock only while
   *  registers are accessed (reset 0); [30/29] TIMER/WDT_CLK_IS_ACTIVE
   *  (reset 1); [28] ETM_EN (reset 1) */
  uint32_t regclk;
} timg_dev_t;

/**
 * RTC_WDT register block at RTC_WDT_BASE (TRM 15.5-15.6). This is a partial
 * map: 0x34-0x3FB is reserved.
 */
typedef volatile struct {
  /** 0x00 RTC_WDT_CONFIG0_REG — TRM 15.1, RWDT configuration */
  rtc_wdt_config0_t wdtconfig0;
  /** 0x04 RTC_WDT_CONFIG1_REG — TRM 15.2, stage 0 hold time in
   *  RTC_DYN_SLOW_CLK cycles (reset 200000; the effective stage 0 hold is
   *  shifted left by EFUSE_WDT_DELAY_SEL + 1, TRM 15.2.2.2) */
  uint32_t wdtconfig1;
  /** 0x08 RTC_WDT_CONFIG2_REG — TRM 15.3, stage 1 hold time (reset 80000) */
  uint32_t wdtconfig2;
  /** 0x0C RTC_WDT_CONFIG3_REG — TRM 15.4, stage 2 hold time (reset 0xFFF) */
  uint32_t wdtconfig3;
  /** 0x10 RTC_WDT_CONFIG4_REG — TRM 15.5, stage 3 hold time (reset 0xFFF) */
  uint32_t wdtconfig4;
  /** 0x14 RTC_WDT_FEED_REG — TRM 15.6 (WT): bit [0] WDT_FEED — write 1 to
   *  feed the RWDT (ignored while write protection is on). */
  uint32_t wdtfeed;
  /** 0x18 RTC_WDT_WPROTECT_REG — TRM 15.7: resets to 0 (locked); write
   *  WDT_WPROTECT_KEY to unlock, any other value locks again. */
  uint32_t wdtwprotect;

  /** 0x1C RTC_WDT_SWD_CONFIG_REG — TRM 15.8, super watchdog configuration */
  rtc_wdt_swd_config_t swdconfig;
  /** 0x20 RTC_WDT_SWD_WPROTECT_REG — TRM 15.9: write WDT_WPROTECT_KEY to
   *  unlock the SWD config, any other value locks it. */
  uint32_t swdwprotect;

  /** 0x24 RTC_WDT_INT_RAW_REG — TRM 15.10, raw interrupt status */
  rtc_wdt_int_t int_raw;
  /** 0x28 RTC_WDT_INT_ST_REG — TRM 15.11, masked status (RO) */
  rtc_wdt_int_t int_st;
  /** 0x2C RTC_WDT_INT_ENA_REG — TRM 15.12, interrupt enable (R/W) */
  rtc_wdt_int_t int_ena;
  /** 0x30 RTC_WDT_INT_CLR_REG — TRM 15.13, interrupt clear (WO, write 1 to
   *  clear): RTC_WDT->int_clr.val = 1u << 31; */
  rtc_wdt_int_t int_clr;

  uint32_t reserved_34[242]; /* 0x34-0x3FB reserved */

  /** 0x3FC RTC_WDT_DATE_REG — TRM 15.14, version register
   *  ([30:0] reset 0x2112080; [31] CLK_EN reserved) */
  uint32_t date;
} rtc_wdt_t;

/* Layout guards — pinned to the TRM offsets above */
_Static_assert(sizeof(timg_timer_t) == 0x24, "timer block must be 0x24 bytes");
_Static_assert(sizeof(timg_dev_t) == 0x100, "TIMG map must span 0x00-0xFF");
_Static_assert(offsetof(timg_dev_t, wdtconfig0) == 0x48, "MWDT config at 0x48");
_Static_assert(offsetof(timg_dev_t, rtccalicfg) == 0x68, "cali at 0x68");
_Static_assert(offsetof(timg_dev_t, int_ena) == 0x70, "INT_ENA at 0x70");
_Static_assert(offsetof(timg_dev_t, int_clr) == 0x7C, "INT_CLR at 0x7C");
_Static_assert(offsetof(timg_dev_t, rtccalicfg2) == 0x80, "cali2 at 0x80");
_Static_assert(offsetof(timg_dev_t, ntimers_date) == 0xF8,
               "ntimers_date at 0xF8");
_Static_assert(sizeof(rtc_wdt_t) == 0x400,
               "RTC_WDT map must span 0x00-0x3FF");
_Static_assert(offsetof(rtc_wdt_t, wdtfeed) == 0x14, "WDT_FEED at 0x14");
_Static_assert(offsetof(rtc_wdt_t, int_clr) == 0x30, "INT_CLR at 0x30");
_Static_assert(offsetof(rtc_wdt_t, date) == 0x3FC, "DATE at 0x3FC");

/**
 * Peripheral instances
 */
#define TIMG0 ((timg_dev_t *)TIMG0_BASE)
#define TIMG1 ((timg_dev_t *)TIMG1_BASE)
#define RTC_WDT ((rtc_wdt_t *)RTC_WDT_BASE)

/**
 * ===========================================================================
 * Watchdog helpers — unlock/feed/lock around WDT_WPROTECT_KEY (TRM 15.2.2.3)
 *   Feeding automatically returns the stage machine to stage 0.
 *   The RWDT write-key register resets to 0 (locked), the MWDT one to
 *   WDT_WPROTECT_KEY (unlocked) — write the key before touching either.
 * ===========================================================================
 */

static inline void mwdt_unlock(timg_dev_t *tmg) {
  tmg->wdtwprotect = WDT_WPROTECT_KEY;
}

static inline void mwdt_lock(timg_dev_t *tmg) { tmg->wdtwprotect = 0; }

/** Apply pending MWDT configuration changes — required after writing
 *  wdtconfig0-5 or the en bit (TRM 14.10, WDT_CONF_UPDATE_EN). */
static inline void mwdt_conf_update(timg_dev_t *tmg) {
  tmg->wdtconfig0.val |= 1u << 22;
}

static inline void mwdt_feed(timg_dev_t *tmg) {
  mwdt_unlock(tmg);
  tmg->wdtfeed = 1u;
  mwdt_lock(tmg);
}

static inline void rwdt_unlock(void) {
  RTC_WDT->wdtwprotect = WDT_WPROTECT_KEY;
}

static inline void rwdt_lock(void) { RTC_WDT->wdtwprotect = 0; }

static inline void rwdt_feed(void) {
  rwdt_unlock();
  RTC_WDT->wdtfeed = 1u;
  rwdt_lock();
}

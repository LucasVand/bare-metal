#pragma once

#include <stdint.h>

/**
 * ===========================================================================
 * UART controller base addresses — ESP32-C6 TRM (v1.2), Table 5.3-2, §5.3.5.
 * The ESP32-C6 has two HP UART controllers (no UART2).
 * ===========================================================================
 */
#define UART0_BASE 0x60000000
#define UART1_BASE 0x60001000

/**
 * UART_REG — full register pointer from a base address and a register offset
 *   e.g. UART_REG(UART0_BASE, UART_STATUS_REG)
 */
#define UART_REG(base, offset) ((volatile unsigned int *)((base) + (offset)))

volatile uint32_t *uart_reg(uint32_t base, uint32_t offset);

/**
 * ===========================================================================
 * UART register offsets — ESP32-C6 TRM §27.6.1, shared by both UART instances.
 * Build a full register address with UART_REG(<uart>_BASE, <REG>).
 *
 * Registers whose name carries the _SYNC suffix are clocked in the UART
 * Core's domain: after configuring them, write 1 to UART_REG_UPDATE_REG to
 * synchronize the values into the core clock domain (TRM §27.5.1).
 * ===========================================================================
 */

/**
 * UART_FIFO_REG (0x00) — TRM §27.7.1 Reg 27.1, FIFO data register
 *   Reads return the next byte from the RX FIFO:
 *     [7:0]  UART_RXFIFO_RD_BYTE — data read from the RX FIFO (RO)
 *   Writes are diverted by hardware into the TX FIFO (bypass write, the
 *   RXFIFO_RD_BYTE field itself is untouched):
 *     [7:0]  byte to store into the TX FIFO
 *   [31:8] reserved
 */
#define UART_FIFO_REG 0x00

/**
 * UART_INT_RAW_REG (0x04) — TRM §27.7.1 Reg 27.3, raw interrupt status
 * (R/WTC/SS), unmasked. Interrupt bits (same positions in INT_ST / INT_ENA /
 * INT_CLR):
 *   [0]  RXFIFO_FULL_INT      — RX data >= UART_RXFIFO_FULL_THRHD
 *   [1]  TXFIFO_EMPTY_INT     — TX data < UART_TXFIFO_EMPTY_THRHD
 *   [2]  PARITY_ERR_INT       — parity error detected in received data
 *   [3]  FRM_ERR_INT          — frame format error detected by receiver
 *   [4]  RXFIFO_OVF_INT       — receiver got more data than the FIFO can store
 *   [5]  DSR_CHG_INT          — edge detected on the DSRn input
 *   [6]  CTS_CHG_INT          — edge detected on the CTSn input
 *   [7]  BRK_DET_INT          — break detected (line low for a full NULL char
 *                                after stop bits)
 *   [8]  RXFIFO_TOUT_INT      — bus idle > RX_TOUT_THRHD after >= 1 byte
 *                                received
 *   [9]  SW_XON_INT           — XON char received (SW_FLOW_CON_EN=1)
 *   [10] SW_XOFF_INT          — XOFF char received (SW_FLOW_CON_EN=1)
 *   [11] GLITCH_DET_INT       — receiver detects a START bit (glitch filter)
 *   [12] TX_BRK_DONE_INT      — transmitter finished the NULL chars sent after
 *                                TX data
 *   [13] TX_BRK_IDLE_DONE_INT — idle held >= TX_IDLE_NUM after the last data
 *   [14] TX_DONE_INT          — all data in the TX FIFO has been sent
 *   [15] RS485_PARITY_ERR_INT — parity error in RS-485 mode
 *   [16] RS485_FRM_ERR_INT    — frame error in RS-485 mode
 *   [17] RS485_CLASH_INT      — TX/RX collision detected in RS-485 mode
 *   [18] AT_CMD_CHAR_DET_INT  — configured at_cmd char sequence detected
 *                                (see §27.4.5)
 *   [19] WAKEUP_INT           — UART woke the chip from Light-sleep
 *   [31:20] reserved
 */
#define UART_INT_RAW_REG 0x04

/**
 * UART_INT_ST_REG (0x08) — TRM §27.7.1 Reg 27.4, masked interrupt status (RO)
 *   Same bit layout as UART_INT_RAW_REG; each bit = raw status AND its enable
 *   bit
 */
#define UART_INT_ST_REG 0x08

/**
 * UART_INT_ENA_REG (0x0C) — TRM §27.7.1 Reg 27.5, interrupt enable bits (R/W)
 *   Same bit layout as UART_INT_RAW_REG; set a bit to let that interrupt pass
 *   to INT_ST
 */
#define UART_INT_ENA_REG 0x0C

/**
 * UART_INT_CLR_REG (0x10) — TRM §27.7.1 Reg 27.6, interrupt clear bits (WT,
 * write 1 to clear) Same bit layout as UART_INT_RAW_REG
 */
#define UART_INT_CLR_REG 0x10

/**
 * UART_CLKDIV_SYNC_REG (0x14) — TRM §27.7.1 Reg 27.7, clock divider
 * configuration (R/W, _SYNC)
 *   [11:0]  UART_CLKDIV      — integral part of the divider (reset 0x2B6)
 *   [19:12] reserved
 *   [23:20] UART_CLKDIV_FRAG — decimal part of the divider (reset 0x0)
 *   [31:24] reserved
 *   Baud rate = source clock / (CLKDIV + CLKDIV_FRAG/16)
 */
#define UART_CLKDIV_SYNC_REG 0x14

/**
 * UART_RX_FILT_REG (0x18) — TRM §27.7.1 Reg 27.8, RX filter configuration
 * (R/W)
 *   [7:0] UART_GLITCH_FILT   — pulses narrower than this are ignored (reset
 *                              0x8); unit: UART core clock cycles
 *   [8]   UART_GLITCH_FILT_EN — enable the RX signal filter
 *   [31:9] reserved
 */
#define UART_RX_FILT_REG 0x18

/**
 * UART_STATUS_REG (0x1C) — TRM §27.7.1 Reg 27.21, UART status register (RO)
 *   [31]     UART_TXD          — level of the internal TxD signal (reset 1)
 *   [30]     UART_RTSN         — level of the internal RTS (rtsn_out) signal
 *                              (reset 1)
 *   [29]     UART_DTRN         — level of the internal DTR (dtrn_out) signal
 *                              (reset 1)
 *   [28:24]  reserved
 *   [23:16]  UART_TXFIFO_CNT   — number of valid data bytes in the TX FIFO
 *   [15]     UART_RXD          — level of the internal RxD signal (reset 1)
 *   [14]     UART_CTSN         — level of the internal CTS (ctsn_in) signal
 *                              (reset 1)
 *   [13]     UART_DSRN         — level of the internal DSR (dsrn_in) signal
 *   [12:8]   reserved
 *   [7:0]    UART_RXFIFO_CNT   — number of valid data bytes in the RX FIFO
 */
#define UART_STATUS_REG 0x1C

/**
 * UART_CONF0_SYNC_REG (0x20) — TRM §27.7.1 Reg 27.9, configuration register 0
 * (R/W, _SYNC)
 *   [0]      UART_PARITY          — parity mode: 0 even, 1 odd
 *   [1]      UART_PARITY_EN       — enable parity generation/checking
 *   [3:2]    UART_BIT_NUM         — data length: 0=5, 1=6, 2=7, 3=8 bits
 *   [5:4]    UART_STOP_BIT_NUM    — stop bits: 0 invalid, 1=1, 2=1.5, 3=2
 *   [6]      UART_TXD_BRK         — send NULL chars (break) after TX data
 *                                   completes
 *   [7]      UART_IRDA_DPLX       — enable IrDA loopback mode
 *   [8]      UART_IRDA_TX_EN      — enable the IrDA transmitter
 *   [9]      UART_IRDA_WCTL       — 1: IrDA TX 11th bit copies 10th bit; 0:
 *                                   11th bit = 0
 *   [10]     UART_IRDA_TX_INV     — invert the IrDA transmitter level
 *   [11]     UART_IRDA_RX_INV     — invert the IrDA receiver level
 *   [12]     UART_LOOPBACK        — loopback test mode (txd→rxd, rtsn→ctsn,
 *                                   dtrn→dsrn)
 *   [13]     UART_TX_FLOW_EN      — enable transmitter flow control
 *   [14]     UART_IRDA_EN         — enable the IrDA protocol
 *   [15]     UART_RXD_INV         — invert the RxD signal level
 *   [16]     UART_TXD_INV         — invert the TxD signal level
 *   [17]     UART_DIS_RX_DAT_OVF  — 1: disable RX overflow detection
 *   [18]     UART_ERR_WR_MASK     — 1: do not store received data with errors
 *                                   into the FIFO
 *   [19]     UART_AUTOBAUD_EN     — enable baud rate detection
 *   [20]     UART_MEM_CLK_EN      — enable clock gating for UART memory
 *                                   (reset 1)
 *   [21]     UART_SW_RTS          — software RTS signal used in flow control;
 *                                   1: transmitter may send data
 *   [22]     UART_RXFIFO_RST      — reset the receive-FIFO
 *   [23]     UART_TXFIFO_RST      — reset the transmit-FIFO
 *   [31:24]  reserved
 */
#define UART_CONF0_SYNC_REG 0x20

/**
 * UART_CONF1_REG (0x24) — TRM §27.7.1 Reg 27.10, configuration register 1
 * (R/W)
 *   [7:0]    UART_RXFIFO_FULL_THRHD  — RX threshold that raises
 *                                      RXFIFO_FULL_INT (reset 0x60)
 *   [15:8]   UART_TXFIFO_EMPTY_THRHD — TX threshold that raises
 *                                      TXFIFO_EMPTY_INT (reset 0x60)
 *   [16]     UART_CTS_INV            — invert the CTS signal level
 *   [17]     UART_DSR_INV            — invert the DSR signal level
 *   [18]     UART_RTS_INV            — invert the RTS signal level
 *   [19]     UART_DTR_INV            — invert the DTR signal level
 *   [20]     UART_SW_DTR             — software DTR signal used in software
 *                                      flow control
 *   [21]     UART_CLK_EN             — 1: force the register clock on; 0:
 *                                      clock only while registers are written
 *   [31:22]  reserved
 */
#define UART_CONF1_REG 0x24

/**
 * UART_HWFC_CONF_SYNC_REG (0x2C) — TRM §27.7.1 Reg 27.11, hardware flow
 * control configuration (R/W, _SYNC)
 *   [7:0] UART_RX_FLOW_THRHD — when RX data > this and RX_FLOW_EN=1, rtsn_out
 *                              is pulled high (reset 0x00)
 *   [8]   UART_RX_FLOW_EN    — 1: auto rtsn_out from RX_FLOW_THRHD; 0: rtsn_out
 *                              follows UART_SW_RTS instead
 *   [31:9] reserved
 */
#define UART_HWFC_CONF_SYNC_REG 0x2C

/**
 * UART_SLEEP_CONF0_REG (0x30) — TRM §27.7.1 Reg 27.12, wakeup character 1..4
 * (R/W)
 *   [7:0]   UART_WK_CHAR1 — wakeup character 1 (reset 0x0)
 *   [15:8]  UART_WK_CHAR2 — wakeup character 2 (reset 0x0)
 *   [23:16] UART_WK_CHAR3 — wakeup character 3 (reset 0x0)
 *   [31:24] UART_WK_CHAR4 — wakeup character 4 (reset 0x0)
 */
#define UART_SLEEP_CONF0_REG 0x30

/**
 * UART_SLEEP_CONF1_REG (0x34) — TRM §27.7.1 Reg 27.13, wakeup character 0
 * (R/W)
 *   [7:0]   UART_WK_CHAR0 — wakeup character 0 (reset 0x0)
 *   [31:8]  reserved
 */
#define UART_SLEEP_CONF1_REG 0x34

/**
 * UART_SLEEP_CONF2_REG (0x38) — TRM §27.7.1 Reg 27.14, wakeup mode
 * configuration (R/W)
 *   [9:0]    UART_ACTIVE_THRESHOLD  — RXD edge changes needed to wake the chip
 *                                      in wakeup mode 0 (reset 0x0F0)
 *   [17:10]  UART_RX_WAKE_UP_THRHD  — RX bytes needed to wake the chip in
 *                                      wakeup mode 1 (reset 0x1)
 *   [20:18]  UART_WK_CHAR_NUM       — number of wakeup characters (reset 0x5)
 *   [25:21]  UART_WK_CHAR_MASK      — mask of wakeup characters used
 *                                      (reset 0x0)
 *   [27:26]  UART_WK_MODE_SEL       — wakeup mode 0..3 (reset 0x0)
 *   [31:28]  reserved
 */
#define UART_SLEEP_CONF2_REG 0x38

/**
 * UART_SWFC_CONF0_SYNC_REG (0x3C) — TRM §27.7.1 Reg 27.15, software
 * flow-control character config (R/W, _SYNC; XON/XOFF chars vary)
 *   [7:0]    UART_XON_CHAR            — XON flow-control char (reset 0x11,
 *                                         DC1)
 *   [15:8]   UART_XOFF_CHAR           — XOFF flow-control char (reset 0x13,
 *                                         DC3)
 *   [16]     UART_XON_XOFF_STILL_SEND — transmitter may send XON/XOFF when
 *                                         otherwise disabled
 *   [17]     UART_SW_FLOW_CON_EN      — enable software flow control
 *   [18]     UART_XONOFF_DEL          — strip flow-control chars out of
 *                                         received data
 *   [19]     UART_FORCE_XON           — let the transmitter continue
 *   [20]     UART_FORCE_XOFF          — stop the transmitter
 *   [21]     UART_SEND_XON            — send an XON char after current TX (HW
 *                                         auto-clears)
 *   [22]     UART_SEND_XOFF           — send an XOFF char after current TX (HW
 *                                         auto-clears)
 *   [31:23]  reserved
 */
#define UART_SWFC_CONF0_SYNC_REG 0x3C

/**
 * UART_SWFC_CONF1_REG (0x40) — TRM §27.7.1 Reg 27.16, software flow-control
 * thresholds (R/W)
 *   [7:0]   UART_XON_THRESHOLD  — RX FIFO below this => send XON (reset 0x00)
 *   [15:8]  UART_XOFF_THRESHOLD — RX FIFO above this => send XOFF (reset 0xE0)
 *   [31:16] reserved
 */
#define UART_SWFC_CONF1_REG 0x40

/**
 * UART_TXBRK_CONF_SYNC_REG (0x44) — TRM §27.7.1 Reg 27.17, TX break character
 * configuration (R/W, _SYNC)
 *   [7:0]   UART_TX_BRK_NUM — number of NULL chars sent after TX data when
 *                             TXD_BRK=1 (reset 0x0A)
 *   [31:8]  reserved
 */
#define UART_TXBRK_CONF_SYNC_REG 0x44

/**
 * UART_IDLE_CONF_SYNC_REG (0x48) — TRM §27.7.1 Reg 27.18, frame-end idle
 * configuration (R/W, _SYNC)
 *   [9:0]    UART_RX_IDLE_THRHD — RX taking longer than this to receive a byte
 *                                 raises a frame-end signal (reset 0x100);
 *                                 unit: bit time
 *   [19:10]  UART_TX_IDLE_NUM   — minimum idle duration between transfers;
 *                                 >= this after last data raises
 *                                 TX_BRK_IDLE_DONE_INT (reset 0x100)
 *   [31:20]  reserved
 */
#define UART_IDLE_CONF_SYNC_REG 0x48

/**
 * UART_RS485_CONF_SYNC_REG (0x4C) — TRM §27.7.1 Reg 27.19, RS485 mode
 * configuration (R/W, _SYNC)
 *   [0]     UART_RS485_EN        — select RS-485 mode
 *   [1]     UART_DL0_EN          — add a 1-bit turnaround delay before the
 *                                  start bit
 *   [2]     UART_DL1_EN          — add a 1-bit turnaround delay after the stop
 *                                  bit
 *   [3]     UART_RS485TX_RX_EN   — loop the transmitter output back to the
 *                                  receiver input
 *   [4]     UART_RS485RXBY_TX_EN — 1: TX may send while its RX line is busy; 0:
 *                                  it may not
 *   [5]     UART_RS485_RX_DLY_NUM — delay of the receiver's internal data
 *                                  signal
 *   [9:6]   UART_RS485_TX_DLY_NUM — delay of the transmitter's internal data
 *                                  signal
 *   [31:10] reserved
 */
#define UART_RS485_CONF_SYNC_REG 0x4C

/**
 * UART_AT_CMD_PRECNT_SYNC_REG (0x50) — TRM §27.7.1 Reg 27.26, at_cmd
 * pre-sequence timing (R/W, _SYNC)
 *   [15:0]  UART_PRE_IDLE_NUM — idle time required before the first at_cmd
 *                               char; a shorter gap means the next data is not
 *                               taken as at_cmd (reset 0x901); unit: bit time
 *   [31:16] reserved
 */
#define UART_AT_CMD_PRECNT_SYNC_REG 0x50

/**
 * UART_AT_CMD_POSTCNT_SYNC_REG (0x54) — TRM §27.7.1 Reg 27.27, at_cmd
 * post-sequence timing (R/W, _SYNC)
 *   [15:0]  UART_POST_IDLE_NUM — duration required between the last at_cmd
 *                                char and the next data; a shorter gap means
 *                                the previous data is not taken as at_cmd
 *                                (reset 0x901)
 *   [31:16] reserved
 */
#define UART_AT_CMD_POSTCNT_SYNC_REG 0x54

/**
 * UART_AT_CMD_GAPTOUT_SYNC_REG (0x58) — TRM §27.7.1 Reg 27.28, at_cmd gap
 * timeout (R/W, _SYNC)
 *   [15:0]  UART_RX_GAP_TOUT — max interval between consecutive at_cmd chars;
 *                              a larger gap breaks the sequence. Should be set
 *                              above half the baud rate (reset 0xB)
 *   [31:16] reserved
 */
#define UART_AT_CMD_GAPTOUT_SYNC_REG 0x58

/**
 * UART_AT_CMD_CHAR_SYNC_REG (0x5C) — TRM §27.7.1 Reg 27.29, at_cmd char
 * detection config (R/W, _SYNC)
 *   [7:0]   UART_AT_CMD_CHAR — the at_cmd character value (reset 0x2B, '+')
 *   [15:8]  UART_CHAR_NUM    — number of continuous at_cmd chars required to
 *                              raise AT_CMD_CHAR_DET_INT (reset 0x03)
 *   [31:16] reserved
 */
#define UART_AT_CMD_CHAR_SYNC_REG 0x5C

/**
 * UART_TOUT_CONF_SYNC_REG (0x64) — TRM §27.7.1 Reg 27.2, receiver threshold
 * and allocation configuration (R/W, _SYNC)
 *   [0]     UART_RX_TOUT_EN    — enable the receive-timeout function
 *   [1]     reserved
 *   [11:2]  UART_RX_TOUT_THRHD — bus idle time before the receiver times out
 *                                (reset 0xa); unit: bit time
 *   [31:12] reserved
 */
#define UART_TOUT_CONF_SYNC_REG 0x64

/**
 * UART_MEM_TX_STATUS_REG (0x68) — TRM §27.7.1 Reg 27.22, TX FIFO write/read
 * offsets (RO)
 *   [7:0]   UART_TX_SRAM_WADDR — offset address to write the TX FIFO
 *   [8]     reserved
 *   [16:9]  UART_TX_SRAM_RADDR — offset address to read the TX FIFO
 *   [31:17] reserved
 */
#define UART_MEM_TX_STATUS_REG 0x68

/**
 * UART_MEM_RX_STATUS_REG (0x6C) — TRM §27.7.1 Reg 27.23, RX FIFO write/read
 * offsets (RO)
 *   [7:0]   UART_RX_SRAM_RADDR — offset address to read the RX FIFO (reset
 *                                0x80)
 *   [8]     reserved
 *   [16:9]  UART_RX_SRAM_WADDR — offset address to write the RX FIFO (reset
 *                                0x80)
 *   [31:17] reserved
 */
#define UART_MEM_RX_STATUS_REG 0x6C

/**
 * UART_FSM_STATUS_REG (0x70) — TRM §27.7.1 Reg 27.24, transmit/receive FSM
 * status (RO)
 *   [3:0]   UART_ST_URX_OUT — RX FSM state: 0 RX_IDLE, 1 RX_STRT, 2..9
 *                             RX_DAT0..7, 10 RX_PRTY, 11 RX_STP1, 12 RX_STP2
 *   [7:4]   UART_ST_UTX_OUT — TX FSM state: 0 TX_IDLE, 1 TX_STRT, 2..9
 *                             TX_DAT0..7, 10 TX_PRTY, 11 TX_STP1, 12 TX_STP2
 *   [31:8]  reserved
 */
#define UART_FSM_STATUS_REG 0x70

/**
 * UART_POSPULSE_REG (0x74) — TRM §27.7.1 Reg 27.30, autobaud positive edge
 * count (RO)
 *   [11:0]  UART_POSEDGE_MIN_CNT — minimum pulse width between two rising
 *                                  edges (reset 0xFFF)
 *   [31:12] reserved
 */
#define UART_POSPULSE_REG 0x74

/**
 * UART_NEGPULSE_REG (0x78) — TRM §27.7.1 Reg 27.31, autobaud negative edge
 * count (RO)
 *   [11:0]  UART_NEGEDGE_MIN_CNT — minimum pulse width between two falling
 *                                  edges (reset 0xFFF)
 *   [31:12] reserved
 */
#define UART_NEGPULSE_REG 0x78

/**
 * UART_LOWPULSE_REG (0x7C) — TRM §27.7.1 Reg 27.32, autobaud minimum low
 * pulse duration (RO)
 *   [11:0]  UART_LOWPULSE_MIN_CNT — minimum low-level pulse width seen
 *                                   (reset 0xFFF)
 *   [31:12] reserved
 */
#define UART_LOWPULSE_REG 0x7C

/**
 * UART_HIGHPULSE_REG (0x80) — TRM §27.7.1 Reg 27.33, autobaud minimum high
 * pulse duration (RO)
 *   [11:0]  UART_HIGHPULSE_MIN_CNT — minimum high-level pulse width seen
 *                                    (reset 0xFFF)
 *   [31:12] reserved
 *   Software computes the baud rate from this + UART_LOWPULSE_MIN_CNT
 */
#define UART_HIGHPULSE_REG 0x80

/**
 * UART_RXD_CNT_REG (0x84) — TRM §27.7.1 Reg 27.34, autobaud edge change count
 * (RO)
 *   [9:0]   UART_RXD_EDGE_CNT — count of RxD edge changes (reset 0x000)
 *   [31:10] reserved
 */
#define UART_RXD_CNT_REG 0x84

/**
 * UART_CLK_CONF_REG (0x88) — TRM §27.7.1 Reg 27.20, UART core clock
 * configuration (R/W)
 *   [23:0]  reserved
 *   [24]    UART_TX_SCLK_EN — enable the UART TX core clock (reset 1)
 *   [25]    UART_RX_SCLK_EN — enable the UART RX core clock (reset 1)
 *   [26]    UART_TX_RST_CORE — write 1 then 0 to reset the UART TX core
 *   [27]    UART_RX_RST_CORE — write 1 then 0 to reset the UART RX core
 *   [31:28] reserved
 */
#define UART_CLK_CONF_REG 0x88

/**
 * UART_DATE_REG (0x8C) — TRM §27.7.1 Reg 27.35, version control register
 * (R/W, reset 0x2201260)
 */
#define UART_DATE_REG 0x8C

/**
 * UART_AFIFO_STATUS_REG (0x90) — TRM §27.7.1 Reg 27.25, asynchronous FIFO
 * status (RO)
 *   [0]    UART_TX_AFIFO_FULL  — APB TX async FIFO full
 *   [1]    UART_TX_AFIFO_EMPTY — APB TX async FIFO empty (reset 1)
 *   [2]    UART_RX_AFIFO_FULL  — APB RX async FIFO full
 *   [3]    UART_RX_AFIFO_EMPTY — APB RX async FIFO empty (reset 1)
 *   [31:4] reserved
 */
#define UART_AFIFO_STATUS_REG 0x90

/**
 * UART_REG_UPDATE_REG (0x98) — TRM §27.7.1 Reg 27.36, register configuration
 * update (R/W/SC, auto-cleared by HW)
 *   [0]    UART_REG_UPDATE — write 1 to synchronize all _SYNC registers into
 *                            the UART core clock domain
 *   [31:1] reserved
 */
#define UART_REG_UPDATE_REG 0x98

/**
 * UART_ID_REG (0x9C) — TRM §27.7.1 Reg 27.37, UART ID (R/W, reset 0x500)
 */
#define UART_ID_REG 0x9C

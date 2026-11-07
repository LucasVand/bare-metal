#pragma once

#include <stdint.h>

#define UART0_BASE 0x3FF40000
#define UART1_BASE 0x3FF50000
#define UART2_BASE 0x3FF6E000

/**
 * UART_REG — full register pointer from a base address and a register offset
 *   e.g. UART_REG(UART0_BASE, UART_STATUS_REG)
 */
#define UART_REG(base, offset) ((volatile unsigned int *)((base) + (offset)))

volatile uint32_t *uart_reg(uint32_t base, uint32_t offset);

/**
 * ===========================================================================
 * UART register offsets — TRM §19.5.1, shared by all three UART instances.
 * Build a full register address with UART_REG(<uart>_BASE, <REG>).
 * ===========================================================================
 */

/**
 * UART_FIFO_REG (0x00) — TRM 19.1, FIFO data register (R/W)
 *   [7:0]   UART_RXFIFO_RD_BYTE — UARTn accesses the FIFO via this register
 *   [31:8]  reserved
 */
#define UART_FIFO_REG 0x00

/**
 * UART_INT_RAW_REG (0x04) — TRM 19.2, raw interrupt status (RO), unmasked
 *   Interrupt bits (same positions in INT_ST / INT_ENA / INT_CLR):
 *     [0]  RXFIFO_FULL_INT      — RX data exceeds (rx_flow_thrhd_h3,
 * rxfifo_full_thrhd) [1]  TXFIFO_EMPTY_INT     — TX data below
 * (tx_mem_empty_thrhd, txfifo_empty_thrhd) [2]  PARITY_ERR_INT       — parity
 * error detected in received data [3]  FRM_ERR_INT          — frame format
 * error detected by receiver [4]  RXFIFO_OVF_INT       — receiver got more data
 * than the FIFO can store [5]  DSR_CHG_INT          — edge detected on the DSRn
 * input [6]  CTS_CHG_INT          — edge detected on the CTSn input [7]
 * BRK_DET_INT          — break detected (line low for a full NULL char after
 * stop bits) [8]  RXFIFO_TOUT_INT      — bus idle > RX_TOUT_THRHD after >=1
 * byte received [9]  SW_XON_INT           — XON char received
 * (SW_FLOW_CON_EN=1) [10] SW_XOFF_INT          — XOFF char received
 * (SW_FLOW_CON_EN=1) [11] GLITCH_DET_INT       — receiver detects a START bit
 * (glitch filter) [12] TX_BRK_DONE_INT      — transmitter finished the NULL
 * chars sent after TX data [13] TX_BRK_IDLE_DONE_INT — idle held >= TX_IDLE_NUM
 * after the last data [14] TX_DONE_INT          — all data in the transmit-FIFO
 * has been sent [15] RS485_PARITY_ERR_INT — parity error in RS-485 mode [16]
 * RS485_FRM_ERR_INT    — frame error in RS-485 mode [17] RS485_CLASH_INT      —
 * TX/RX collision detected in RS-485 mode [18] AT_CMD_CHAR_DET_INT  —
 * configured at_cmd char sequence detected (see §19.3.6) [31:19] reserved
 */
#define UART_INT_RAW_REG 0x04

/**
 * UART_INT_ST_REG (0x08) — TRM 19.3, masked interrupt status (RO)
 *   Same bit layout as UART_INT_RAW_REG; each bit = raw status AND its enable
 * bit
 */
#define UART_INT_ST_REG 0x08

/**
 * UART_INT_ENA_REG (0x0C) — TRM 19.4, interrupt enable bits (R/W)
 *   Same bit layout as UART_INT_RAW_REG; set a bit to let that interrupt pass
 * to INT_ST
 */
#define UART_INT_ENA_REG 0x0C

/**
 * UART_INT_CLR_REG (0x10) — TRM 19.5, interrupt clear bits (WO, write 1 to
 * clear) Same bit layout as UART_INT_RAW_REG [8]  only clearable while
 * rxfifo_cnt and rx_mem_cnt are both 0 [0]  only clearable while RX FIFO holds
 * less than RXFIFO_FULL_THRHD
 */
#define UART_INT_CLR_REG 0x10

/**
 * UART_CLKDIV_REG (0x14) — TRM 19.6, clock divider configuration (R/W)
 *   [19:0]  UART_CLKDIV      — integral part of the divider (reset 0x2B6)
 *   [23:20] UART_CLKDIV_FRAG — decimal part of the divider (reset 0x0)
 *   [31:24] reserved
 *   Baud rate = source clock / (CLKDIV + CLKDIV_FRAG/16)
 */
#define UART_CLKDIV_REG 0x14

/**
 * UART_AUTOBAUD_REG (0x18) — TRM 19.7, autobaud configuration (R/W)
 *   [0]      UART_AUTOBAUD_EN — enable baud rate detection
 *   [15:8]   UART_GLITCH_FILT — pulses narrower than this are ignored (reset
 * 0x10) [7:1], [31:16] reserved
 */
#define UART_AUTOBAUD_REG 0x18

/**
 * UART_STATUS_REG (0x1C) — TRM 19.8, UART status register (RO)
 *   [31]     UART_TXD          — level of the internal TxD signal
 *   [30]     UART_RTSN         — level of the internal RTS (rtsn_out) signal
 *   [29]     UART_DTRN         — level of the internal DTR (dtrn_out) signal
 *   [28]     reserved
 *   [27:24]  UART_ST_UTX_OUT   — TX FSM state: 0 TX_IDLE, 1 TX_STRT, 2..9
 * TX_DAT0..7, 10 TX_PRTY, 11 TX_STP1, 12 TX_STP2, 13 TX_DL0, 14 TX_DL1 [23:16]
 * UART_TXFIFO_CNT   — low 8 bits (txfifo_cnt) of the TX byte count; the 3 high
 *                                bits (tx_mem_cnt) live in
 * UART_MEM_CNT_STATUS_REG[5:3] [15]     UART_RXD          — level of the
 * internal RxD signal [14]     UART_CTSN         — level of the internal CTS
 * (ctsn_in) signal [13]     UART_DSRN         — level of the internal DSR
 * (dsrn_in) signal [12]     reserved [11:8]   UART_ST_URX_OUT   — RX FSM state:
 * 0 RX_IDLE, 1 RX_STRT, 2..9 RX_DAT0..7, 10 RX_PRTY, 11 RX_STP1, 12 RX_STP2, 13
 * RX_DL1 [7:0]    UART_RXFIFO_CNT   — low 8 bits (rxfifo_cnt) of the RX byte
 * count; the 3 high bits (rx_mem_cnt) live in UART_MEM_CNT_STATUS_REG[2:0]
 */
#define UART_STATUS_REG 0x1C

/**
 * UART_CONF0_REG (0x20) — TRM 19.9, configuration register 0 (R/W)
 *   [0]      UART_PARITY            — parity mode: 0 even, 1 odd
 *   [1]      UART_PARITY_EN         — enable parity generation/checking
 *   [3:2]    UART_BIT_NUM           — data length: 0=5, 1=6, 2=7, 3=8 bits
 *   [5:4]    UART_STOP_BIT_NUM      — stop bits: 0 invalid, 1=1, 2=1.5, 3=2
 *   [6]      UART_SW_RTS            — drive rtsn_out low (HW flow ctrl when
 * RX_FLOW_EN=0) [7]      UART_SW_DTR            — software DTR signal used in
 * software flow control [8]      UART_TXD_BRK           — send NULL chars
 * (break) after TX data completes [9]      UART_IRDA_DPLX         — enable IrDA
 * loopback mode [10]     UART_IRDA_TX_EN        — start enable of the IrDA
 * transmitter [11]     UART_IRDA_WCTL         — 1: IrDA TX 11th bit copies 10th
 * bit; 0: 11th bit = 0 [12]     UART_IRDA_TX_INV       — invert the IrDA
 * transmitter level [13]     UART_IRDA_RX_INV       — invert the IrDA receiver
 * level [14]     UART_LOOPBACK          — loopback test mode (txd→rxd,
 * rtsn→ctsn, dtrn→dsrn) [15]     UART_TX_FLOW_EN        — enable transmitter
 * flow control [16]     UART_IRDA_EN           — enable the IrDA protocol [17]
 * UART_RXFIFO_RST        — reset the receive-FIFO (UART1's bit also disturbs
 * UART2: only use when UART2's FIFOs are empty) [18]     UART_TXFIFO_RST —
 * reset the transmit-FIFO (same UART2 caveat as [17]) [19]     UART_RXD_INV —
 * invert the RxD signal level [20]     UART_CTS_INV           — invert the CTS
 * signal level [21]     UART_DSR_INV           — invert the DSR signal level
 *   [22]     UART_TXD_INV           — invert the TxD signal level
 *   [23]     UART_RTS_INV           — invert the RTS signal level
 *   [24]     UART_DTR_INV           — invert the DTR signal level
 *   [26:25]  reserved
 *   [27]     UART_TICK_REF_ALWAYS_ON — clock source: 1 = APB clock, 0 =
 * REF_TICK [31:28]  reserved
 */
#define UART_CONF0_REG 0x20

/**
 * UART_CONF1_REG (0x24) — TRM 19.10, configuration register 1 (R/W)
 *   [6:0]    UART_RXFIFO_FULL_THRHD — RX threshold that raises RXFIFO_FULL_INT
 * (reset 0x60) [7]      reserved [14:8]   UART_TXFIFO_EMPTY_THRHD — TX
 * threshold that raises TXFIFO_EMPTY_INT (reset 0x60) [15]     reserved [22:16]
 * UART_RX_FLOW_THRHD     — HW flow-ctrl threshold: when RX data > this and
 *                                     RX_FLOW_EN=1, rtsn_out is pulled low
 * (reset 0x00) [23]     UART_RX_FLOW_EN        — 1: auto rtsn_out from
 * RX_FLOW_THRHD; 0: rtsn_out follows UART_SW_RTS instead [30:24]
 * UART_RX_TOUT_THRHD     — RX timeout value; counts baud cycles x8 (APB clock)
 *                                     or baud x8 x (REF_TICK/APB) with REF_TICK
 * (reset 0x00) [31]     UART_RX_TOUT_EN        — enable the receive-timeout
 * function
 */
#define UART_CONF1_REG 0x24

/**
 * UART_LOWPULSE_REG (0x28) — TRM 19.11, autobaud minimum low pulse duration
 * (RO) [19:0]   UART_LOWPULSE_MIN_CNT — minimum low-level pulse width seen
 * (reset 0xFFFFF) [31:20]  reserved
 */
#define UART_LOWPULSE_REG 0x28

/**
 * UART_HIGHPULSE_REG (0x2C) — TRM 19.12, autobaud minimum high pulse duration
 * (RO) [19:0]   UART_HIGHPULSE_MIN_CNT — minimum high-level pulse width seen
 * (reset 0xFFFFF) [31:20]  reserved Software computes the baud rate from this +
 * UART_LOWPULSE_MIN_CNT
 */
#define UART_HIGHPULSE_REG 0x2C

/**
 * UART_RXD_CNT_REG (0x30) — TRM 19.13, autobaud edge change count (RO)
 *   [9:0]    UART_RXD_EDGE_CNT — count of RxD edge changes (reset 0x000)
 *   [31:10]  reserved
 */
#define UART_RXD_CNT_REG 0x30

/**
 * UART_FLOW_CONF_REG (0x34) — TRM 19.14, software flow-control configuration
 * (R/W) [0]      UART_SW_FLOW_CON_EN — enable software flow control (with
 * sw_xon/sw_xoff) [1]      UART_XONOFF_DEL     — strip flow-control chars out
 * of received data [2]      UART_FORCE_XON      — clear internal CTSn, let the
 * transmitter continue [3]      UART_FORCE_XOFF     — set internal CTSn, stop
 * the transmitter [4]      UART_SEND_XON       — send an XON char after current
 * TX (HW auto-clears) [5]      UART_SEND_XOFF      — send an XOFF char after
 * current TX (HW auto-clears) [31:6]   reserved
 */
#define UART_FLOW_CONF_REG 0x34

/**
 * UART_SLEEP_CONF_REG (0x38) — TRM 19.15, sleep-mode configuration (R/W)
 *   [9:0]    UART_ACTIVE_THRESHOLD — system leaves Light-sleep when RxD
 * positive edges reach (ACTIVE_THRESHOLD + 2) (reset 0x0F0) [31:10]  reserved
 */
#define UART_SLEEP_CONF_REG 0x38

/**
 * UART_SWFC_CONF_REG (0x3C) — TRM 19.16, software flow-control character config
 * (R/W) [7:0]    UART_XON_THRESHOLD   — RX FIFO below this => send XON (reset
 * 0x00) [15:8]   UART_XOFF_THRESHOLD  — RX FIFO above this => send XOFF (reset
 * 0xE0) [23:16]  UART_XON_CHAR        — XON flow-control char (reset 0x11, DC1)
 *   [31:24]  UART_XOFF_CHAR       — XOFF flow-control char (reset 0x13, DC3)
 */
#define UART_SWFC_CONF_REG 0x3C

/**
 * UART_IDLE_CONF_REG (0x40) — TRM 19.17, frame-end idle configuration (R/W)
 *   [9:0]    UART_RX_IDLE_THRHD — RX taking longer than this to receive a byte
 * raises a frame-end signal (reset 0x100) [19:10]  UART_TX_IDLE_NUM   — minimum
 * idle duration between transfers; >= this after last data raises
 * TX_BRK_IDLE_DONE_INT (reset 0x100) [27:20]  UART_TX_BRK_NUM    — number of
 * NULL chars sent after TX data when TXD_BRK=1 (reset 0x0A) [31:28]  reserved
 */
#define UART_IDLE_CONF_REG 0x40

/**
 * UART_RS485_CONF_REG (0x44) — TRM 19.18, RS485 mode configuration (R/W)
 *   [0]      UART_RS485_EN        — select RS-485 mode
 *   [1]      UART_DL0_EN          — delay the STOP bit by 1 bit after DL1
 *   [2]      UART_DL1_EN          — delay the STOP bit by 1 bit
 *   [3]      UART_RS485TX_RX_EN   — loop the transmitter output back to the
 * receiver input [4]      UART_RS485RXBY_TX_EN — 1: TX may send while its RX
 * line is busy; 0: it may not [5]      UART_RS485_RX_DLY_NUM — delay of the
 * receiver's internal data signal [9:6]    UART_RS485_TX_DLY_NUM — delay of the
 * transmitter's internal data signal [31:10]  reserved
 */
#define UART_RS485_CONF_REG 0x44

/**
 * UART_AT_CMD_PRECNT_REG (0x48) — TRM 19.19, at_cmd pre-sequence timing (R/W)
 *   [23:0]   UART_PRE_IDLE_NUM — idle time required before the first at_cmd
 * char; a shorter gap means the next data is not taken as at_cmd (reset
 * 0x186A00) [31:24]  reserved
 */
#define UART_AT_CMD_PRECNT_REG 0x48

/**
 * UART_AT_CMD_POSTCNT_REG (0x4C) — TRM 19.20, at_cmd post-sequence timing (R/W)
 *   [23:0]   UART_POST_IDLE_NUM — duration required between the last at_cmd
 * char and the next data; a shorter gap means the previous data is not taken as
 * at_cmd (reset 0x186A00) [31:24]  reserved
 */
#define UART_AT_CMD_POSTCNT_REG 0x4C

/**
 * UART_AT_CMD_GAPTOUT_REG (0x50) — TRM 19.21, at_cmd gap timeout (R/W)
 *   [23:0]   UART_RX_GAP_TOUT — max interval between consecutive at_cmd chars;
 * a larger gap breaks the sequence. Should be set above half the baud rate
 * (reset 0x1E00) [31:24]  reserved
 */
#define UART_AT_CMD_GAPTOUT_REG 0x50

/**
 * UART_AT_CMD_CHAR_REG (0x54) — TRM 19.22, at_cmd char detection config (R/W)
 *   [7:0]    UART_AT_CMD_CHAR — the at_cmd character value (reset 0x2B, '+')
 *   [15:8]   UART_CHAR_NUM    — number of continuous at_cmd chars required to
 * raise AT_CMD_CHAR_DET_INT (reset 0x03) [31:16]  reserved
 */
#define UART_AT_CMD_CHAR_REG 0x54

/**
 * UART_MEM_CONF_REG (0x58) — TRM 19.23, FIFO threshold and allocation config
 * (R/W) [0]      UART_MEM_PD              — power down the shared FIFO RAM (all
 * three UARTs' MEM_PD must be set for low-power mode) [2:1]    reserved [6:3]
 * UART_RX_SIZE             — memory allocated to the RX FIFO (default 0x1 = 128
 * bytes; may steal space from other UARTs) [10:7]   UART_TX_SIZE             —
 * memory allocated to the TX FIFO (default 0x1 = 128 bytes) [14:11]  reserved
 *   [17:15]  UART_RX_FLOW_THRHD_H3    — high bits of UART_RX_FLOW_THRHD
 *   [20:18]  UART_RX_TOUT_THRHD_H3    — high bits of UART_RX_TOUT_THRHD
 *   [22:21]  UART_XON_THRESHOLD_H2    — high bits of UART_XON_THRESHOLD
 *   [24:23]  UART_XOFF_THRESHOLD_H2   — high bits of UART_XOFF_THRESHOLD
 *   [27:25]  UART_RX_MEM_FULL_THRHD   — high bits of UART_RXFIFO_FULL_THRHD
 *   [30:28]  UART_TX_MEM_EMPTY_THRHD  — high bits of UART_TXFIFO_EMPTY_THRHD
 *   [31]     reserved
 */
#define UART_MEM_CONF_REG 0x58

/**
 * UART_MEM_TX_STATUS_REG (0x5C) — TRM 19.24, TX FIFO write/read offsets (RO)
 *   [12:2]   UART_MEM_TX_RD_ADDR — offset address to read the TX FIFO
 *   [23:13]  UART_MEM_TX_WR_ADDR — offset address to write the TX FIFO
 *   [1:0], [31:24] reserved
 */
#define UART_MEM_TX_STATUS_REG 0x5C

/**
 * UART_MEM_RX_STATUS_REG (0x60) — TRM 19.25, RX FIFO write/read offsets (RO)
 *   [12:2]   UART_MEM_RX_RD_ADDR — offset address to read the RX FIFO
 *   [23:13]  UART_MEM_RX_WR_ADDR — offset address to write the RX FIFO
 *   [1:0], [31:24] reserved
 */
#define UART_MEM_RX_STATUS_REG 0x60

/**
 * UART_MEM_CNT_STATUS_REG (0x64) — TRM 19.26, FIFO memory counts (RO)
 *   [2:0]    UART_RX_MEM_CNT — high 3 bits of UART_RXFIFO_CNT (STATUS_REG[7:0])
 *   [5:3]    UART_TX_MEM_CNT — high 3 bits of UART_TXFIFO_CNT
 * (STATUS_REG[23:16]) [31:6]   reserved
 */
#define UART_MEM_CNT_STATUS_REG 0x64

/**
 * UART_POSPULSE_REG (0x68) — TRM 19.27, autobaud positive edge count (RO)
 *   [19:0]   UART_POSEDGE_MIN_CNT — count of RxD positive edges (reset 0xFFFFF)
 *   [31:20]  reserved
 */
#define UART_POSPULSE_REG 0x68

/**
 * UART_NEGPULSE_REG (0x6C) — TRM 19.28, autobaud negative edge count (RO)
 *   [19:0]   UART_NEGEDGE_MIN_CNT — count of RxD negative edges (reset 0xFFFFF)
 *   [31:20]  reserved
 */
#define UART_NEGPULSE_REG 0x6C

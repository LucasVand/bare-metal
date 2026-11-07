

#include "uart.h"
#include "uart/uart_registers.h"
#include "wdt.h"
#include "wdt/wdt_registers.h"
#include <stddef.h>
#include <stdint.h>

#define UHCI0_DMA_OUT_LINK_REG ((volatile unsigned int *)0x3FF54024)

#define DPORT_BASE 0x3FF00000
#define DPORT_PERIP_CLK_EN_REG ((volatile unsigned int *)(DPORT_BASE + 0x00C0))
#define DPORT_PERIP_RST_EN_REG ((volatile unsigned int *)(DPORT_BASE + 0x00C4))
#define UHCI0_CONF0_REG ((volatile unsigned int *)(0x3FF54000))

typedef struct dma_descriptor {
  unsigned int size : 12;
  unsigned int length : 12;
  unsigned int reserved : 6;
  unsigned int eof : 1;
  unsigned int owner : 1;
  const void *buf;
  struct dma_descriptor *next;
} dma_descriptor_t;

int uart0_count() {
  return *UART_REG(UART0_BASE, UART_STATUS_REG) >> 16 & 0x000000EF;
}

void set_dma_link(dma_descriptor_t *desc, volatile unsigned int *link) {
  *link |= 0x10000000; // stop the dma

  unsigned int reg = 0;
  int small_ptr = (int)desc & 0x000FFFFF; // create the 20 bit ptr
  reg |= small_ptr;                       // or in the small_ptr
  reg |= 0x20000000;                      // start the dma
  *link = reg;                            // set the reg
}
int str_len(const char *msg) {
  int count = 0;
  while (*msg != '\0') {
    msg++;
    count++;
  }
  return count;
}

static dma_descriptor_t desc = {0};

void print(const char *msg) {
  desc.buf = msg;
  int len = str_len(msg);
  desc.size = (len + 3) & ~3;
  desc.length = len;
  desc.eof = 1;
  desc.next = 0;
  desc.owner = 1;

  set_dma_link(&desc, UHCI0_DMA_OUT_LINK_REG);
}

void call_start_cpu0(void) {
  // turn off rtc wdt
  rwdt_unlock();
  volatile uint32_t *rwdt_config = wdt_reg(RTC_CNTL_BASE, RWDT_CONFIG0_REG);
  *rwdt_config &= ~(RWDT_EN | RWDT_FLASHBOOT_MOD_EN);
  rwdt_lock();

  mwdt_unlock(TIMG0_BASE);
  volatile uint32_t *mwdt_config = wdt_reg(TIMG0_BASE, MWDT_CONFIG0_REG);
  *mwdt_config &= ~(MWDT_EN | MWDT_FLASHBOOT_MOD_EN);
  mwdt_lock(TIMG0_BASE);

  // 5. Explicitly turn on the peripheral clock gates for UHCI0 inside the DPORT
  // options
  /* *DPORT_PERIP_CLK_EN_REG |= (1 << 8); */
  /* *DPORT_PERIP_RST_EN_REG &= ~(1 << 8); */
  /**/
  /* // Bind UHCI0 streaming lines to connect directly to the UART0 device
   * channel */
  /* // mapping */
  /* *UHCI0_CONF0_REG = (1 << 9); */

  // FIX: CONF1 resets with
  // TX_ACK_NUM_RE/TX_CHECK_SUM_RE/CHECK_SEQ_EN/CHECK_SUM_EN = 1,
  // which makes hardware REPLACE bytes in your TX stream. TRM says init them to
  // 0.

  const char *msg = "Hello from static memory layout please work!\n\r";

  uart_tx_cstr(msg, UART0_BASE);

  uart_tx_cstr("Please enter your string: \r\n", UART0_BASE);
  char buf[100];
  size_t c = 0;
  while (c + 1 < 100) {
    char ch = uart_rx_byte(UART0_BASE);
    if ((ch == '\n') | (ch == '\r')) {
      break;
    }
    uart_tx_byte(ch, UART0_BASE);
    buf[c] = ch;
    c++;
  }
  buf[c] = '\0';
  uart_tx_cstr("\r\n", UART0_BASE);

  uart_tx_cstr("Your string: ", UART0_BASE);
  uart_tx_cstr(buf, UART0_BASE);
  uart_tx_cstr("\r\n", UART0_BASE);

  while (1) {
  }
}

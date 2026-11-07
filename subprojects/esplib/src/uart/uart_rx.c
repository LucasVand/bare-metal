#include "uart.h"

#include <stddef.h>
#include <stdint.h>

uint8_t uart_rx_fifo_count(uint32_t uart) {
  volatile uint32_t *status = uart_reg(uart, UART_STATUS_REG);
  return (uint8_t)(*status & 0xFF);
}

uint8_t uart_rx_byte(uint32_t uart) {
  while (uart_rx_fifo_count(uart) == 0) {
  }

  return *uart_reg(uart, UART_FIFO_REG);
}

size_t uart_rx(void *data, size_t size, uint32_t uart) {
  uint8_t *data_b = data;
  for (int i = 0; i < size; i++) {
    data_b[i] = uart_rx_byte(uart);
  }

  return size;
}

size_t uart_rx_cstr(char *str, size_t size, uint32_t uart) {
  size_t count = 0;
  char c;
  while (count + 1 < size) {
    c = uart_rx_byte(uart);
    if (c == '\n') {
      break;
    }

    str[count] = c;
    count++;
  }
  str[count] = '\0';
  return count;
}

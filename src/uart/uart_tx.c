
#include "uart.h"
#include <stddef.h>

#define UART_FIFO_MAX_TOLERANCE 126

uint8_t uart_tx_fifo_count(uint32_t uart) {
  volatile uint32_t *status = uart_reg(uart, UART_STATUS_REG);
  return (uint8_t)(*status >> 16 & 0xFF);
}

void uart_tx_byte(const uint8_t data, uint32_t uart) {
  volatile uint32_t *reg = uart_reg(uart, UART_FIFO_REG);

  while (uart_tx_fifo_count(uart) >= UART_FIFO_MAX_TOLERANCE) {
  }

  *reg = data;
}

size_t uart_tx_byte_nb(const uint8_t data, uint32_t uart) {
  volatile uint32_t *reg = uart_reg(uart, UART_FIFO_REG);

  if (uart_tx_fifo_count(uart) >= UART_FIFO_MAX_TOLERANCE) {
    return 1;
  }

  *reg = data;
  return 0;
}
size_t uart_tx(const void *data, size_t size, uint32_t uart) {
  const uint8_t *data_b = data;
  for (int i = 0; i < size; i++) {
    uart_tx_byte(data_b[i], uart);
  }

  return size;
}

size_t uart_tx_cstr(const char *str, uint32_t uart) {
  size_t count = 0;
  while (*str != '\0') {
    uart_tx_byte(*str, uart);
    str++;
    count++;
  }
  return count;
}

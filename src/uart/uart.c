

#include <stdint.h>

volatile uint32_t *uart_reg(uint32_t base, uint32_t offset) {
  return (volatile uint32_t *)(base + offset);
}

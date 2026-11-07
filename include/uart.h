
#pragma once

#include "uart/uart_registers.h"
#include <stddef.h>
#include <stdint.h>

size_t uart_tx(const void *data, size_t size, uint32_t uart);
void uart_tx_byte(uint8_t byte, uint32_t uart);

size_t uart_tx_cstr(const char *str, uint32_t uart);

uint8_t uart_tx_fifo_count(uint32_t uart);

uint8_t uart_rx_fifo_count(uint32_t uart);
uint8_t uart_rx_byte(uint32_t uart);
size_t uart_rx(void *data, size_t size, uint32_t uart);
size_t uart_rx_cstr(char *str, size_t size, uint32_t uart);

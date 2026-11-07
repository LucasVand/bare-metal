
#include "uart.h"
#include "uart/uart_registers.h"
#include <stdarg.h>
#include <stddef.h>

void write(char c) { uart_tx_byte(c, UART0_BASE); }

size_t formatter(const char **fmt, va_list *args) {
  (*fmt)++;
  char next = **fmt;
  if (next == '%') {
    write('%');
    return 1;
  } else if (next == 'c') {
    char c = (char)va_arg(*args, int);
    write(c);
  } else if (next == 's') {
    const char *c = va_arg(*args, char *);
    while (*c != '\0') {
      write(*c);
      c++;
    }
  } else if (next == 'd') {
  }
  (*fmt)++;
  return 1;
}

size_t printf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  size_t count = 0;
  while (*fmt != '\0') {
    char c = *fmt;
    if (c == '%') {
      count += formatter(&fmt, &args);
    } else {
      write(c);
      fmt++;
    }
  }

  return count;
}

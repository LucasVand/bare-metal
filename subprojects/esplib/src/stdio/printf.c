#include "stdio.h"
#include "uart.h"
#include "uart/uart_registers.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#define SINK_BUF_LEN 32

typedef struct {
  size_t (*write_fn)(const char *data, size_t len);
  char buf[SINK_BUF_LEN];
  size_t fill;
  size_t total;
} sink_t;

static void sink_flush(sink_t *s) {
  if (s->fill > 0) {
    s->write_fn(s->buf, s->fill);
    s->fill = 0;
  }
}

static void sink_put(sink_t *s, char c) {
  s->buf[s->fill++] = c;
  s->total++;
  if (s->fill == SINK_BUF_LEN) {
    sink_flush(s);
  }
}

static void sink_pad(sink_t *s, char c, int n) {
  while (n-- > 0) {
    sink_put(s, c);
  }
}

static void emit_num(sink_t *s, uint64_t value, uint32_t base, int upper,
                     int negative, int precision, int width, int left,
                     int zero) {
  static const char lower[] = "0123456789abcdef";
  static const char upper_digits[] = "0123456789ABCDEF";
  const char *digits = upper ? upper_digits : lower;
  char tmp[24];
  int digits_len = 0;

  do {
    tmp[digits_len++] = digits[value % base];
    value /= base;
  } while (value != 0);

  int sign_len = negative ? 1 : 0;
  int zeros = precision > digits_len ? precision - digits_len : 0;
  int total = sign_len + digits_len + zeros;
  int spaces = width > total ? width - total : 0;
  int zero_eff = zero && precision < 0;

  if (left) {
    if (negative) {
      sink_put(s, '-');
    }
    sink_pad(s, '0', zeros);
    while (digits_len > 0) {
      sink_put(s, tmp[--digits_len]);
    }
    sink_pad(s, ' ', spaces);
  } else if (zero_eff) {
    if (negative) {
      sink_put(s, '-');
    }
    sink_pad(s, '0', zeros + spaces);
    while (digits_len > 0) {
      sink_put(s, tmp[--digits_len]);
    }
  } else {
    sink_pad(s, ' ', spaces);
    if (negative) {
      sink_put(s, '-');
    }
    sink_pad(s, '0', zeros);
    while (digits_len > 0) {
      sink_put(s, tmp[--digits_len]);
    }
  }
}

static size_t format_impl(size_t (*write_fn)(const char *data, size_t len),
                          const char *fmt, va_list args) {
  if (write_fn == NULL || fmt == NULL) {
    return 0;
  }

  sink_t s = {0};
  s.write_fn = write_fn;

  for (const char *p = fmt; *p != '\0'; p++) {
    if (*p != '%') {
      sink_put(&s, *p);
      continue;
    }

    p++;
    if (*p == '\0') {
      break;
    }
    if (*p == '%') {
      sink_put(&s, '%');
      continue;
    }

    int left = 0;
    int zero = 0;
    while (*p == '-' || *p == '0' || *p == '+' || *p == ' ' || *p == '#') {
      if (*p == '-') {
        left = 1;
      } else if (*p == '0') {
        zero = 1;
      }
      p++;
    }

    int width = 0;
    while (*p >= '0' && *p <= '9') {
      width = width * 10 + (*p - '0');
      p++;
    }

    int precision = -1;
    if (*p == '.') {
      p++;
      precision = 0;
      while (*p >= '0' && *p <= '9') {
        precision = precision * 10 + (*p - '0');
        p++;
      }
    }

    int long_mod = 0;
    int size_mod = 0;
    if (*p == 'l') {
      p++;
      long_mod = 1;
      if (*p == 'l') {
        p++;
        long_mod = 2;
      }
    } else if (*p == 'h') {
      p++;
      if (*p == 'h') {
        p++;
      }
    } else if (*p == 'z') {
      p++;
      size_mod = 1;
    }

    switch (*p) {
    case '\0':
      p--;
      break;

    case 'c': {
      char c = (char)va_arg(args, int);
      int pad_n = width > 1 ? width - 1 : 0;
      if (!left) {
        sink_pad(&s, ' ', pad_n);
      }
      sink_put(&s, c);
      if (left) {
        sink_pad(&s, ' ', pad_n);
      }
      break;
    }

    case 's': {
      const char *str = va_arg(args, const char *);
      if (str == NULL) {
        str = "(null)";
      }
      int len = 0;
      while (str[len] != '\0' && (precision < 0 || len < precision)) {
        len++;
      }
      int pad_n = width > len ? width - len : 0;
      if (!left) {
        sink_pad(&s, ' ', pad_n);
      }
      for (int i = 0; i < len; i++) {
        sink_put(&s, str[i]);
      }
      if (left) {
        sink_pad(&s, ' ', pad_n);
      }
      break;
    }

    case 'd':
    case 'i': {
      long long v;
      if (long_mod == 2) {
        v = va_arg(args, long long);
      } else if (long_mod == 1) {
        v = va_arg(args, long);
      } else if (size_mod) {
        v = (long long)va_arg(args, ptrdiff_t);
      } else {
        v = va_arg(args, int);
      }
      int negative = v < 0;
      uint64_t u = (uint64_t)v;
      if (negative) {
        u = ~u + 1;
      }
      emit_num(&s, u, 10, 0, negative, precision, width, left, zero);
      break;
    }

    case 'u':
    case 'x':
    case 'X': {
      uint64_t v;
      if (long_mod == 2) {
        v = va_arg(args, unsigned long long);
      } else if (long_mod == 1) {
        v = va_arg(args, unsigned long);
      } else if (size_mod) {
        v = (uint64_t)va_arg(args, size_t);
      } else {
        v = va_arg(args, unsigned int);
      }
      uint32_t base = (*p == 'u') ? 10 : 16;
      emit_num(&s, v, base, *p == 'X', 0, precision, width, left, zero);
      break;
    }

    case 'p': {
      uint64_t v = (uint64_t)(uintptr_t)va_arg(args, void *);
      sink_put(&s, '0');
      sink_put(&s, 'x');
      int digits_w = width > 2 ? width - 2 : 8;
      emit_num(&s, v, 16, 0, 0, precision, digits_w, left, 1);
      break;
    }

    default:
      break;
    }
  }

  sink_flush(&s);
  return s.total;
}

size_t printfs(size_t (*write_fn)(const char *data, size_t len),
               const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  size_t count = format_impl(write_fn, fmt, args);
  va_end(args);
  return count;
}

static size_t uart0_write(const char *data, size_t len) {
  return uart_tx(data, len, UART0_BASE);
}

int printf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  size_t count = format_impl(uart0_write, fmt, args);
  va_end(args);
  return (int)count;
}

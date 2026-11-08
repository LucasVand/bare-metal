
#pragma once

#include <stddef.h>

int printf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

size_t printfs(size_t (*write_fn)(const char *data, size_t len),
               const char *fmt, ...) __attribute__((format(printf, 2, 3)));
;

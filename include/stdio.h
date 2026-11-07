
#pragma once

#include <stddef.h>

int printf(const char *fmt, ...);

size_t printfs(size_t (*write_fn)(const char *data, size_t len),
               const char *fmt, ...);

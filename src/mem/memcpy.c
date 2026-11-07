
#include "mem.h"

void *memcpy(void *dest, const void *src, size_t count) {
  unsigned char *d = dest;
  const unsigned char *s = src;
  for (size_t i = 0; i < count; i++) {
    d[i] = s[i];
  }
  return dest;
}


#include "mem.h"

void *memmove(void *dest, const void *src, size_t count) {
  unsigned char *d = dest;
  const unsigned char *s = src;
  if (d == s || count == 0) {
    return dest;
  }
  if (d < s) {
    for (size_t i = 0; i < count; i++) {
      d[i] = s[i];
    }
  } else {
    for (size_t i = count; i > 0; i--) {
      d[i - 1] = s[i - 1];
    }
  }
  return dest;
}

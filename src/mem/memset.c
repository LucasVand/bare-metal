
#include "mem.h"

void *memset(void *dest, int value, size_t count) {
  unsigned char *d = dest;
  unsigned char v = (unsigned char)value;
  for (size_t i = 0; i < count; i++) {
    d[i] = v;
  }
  return dest;
}


#include "mem.h"

int memcmp(const void *lhs, const void *rhs, size_t count) {
  const unsigned char *a = lhs;
  const unsigned char *b = rhs;
  for (size_t i = 0; i < count; i++) {
    if (a[i] != b[i]) {
      return (int)a[i] - (int)b[i];
    }
  }
  return 0;
}

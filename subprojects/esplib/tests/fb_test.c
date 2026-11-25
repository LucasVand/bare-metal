

#include "alloc.h"
#include "unity.h"
#include <stdint.h>
#include <stdio.h>
void test_fixed_block_allocator() {
  uint8_t backing[100];

  FixedBlockAllocator fb;
  size_t cap = fb_new(&fb, backing, sizeof(backing), 10);
  TEST_ASSERT_NOT_EQUAL(0, cap);

  TEST_ASSERT_EQUAL(10, fb_capacity(&fb));
  TEST_ASSERT_EQUAL(0, fb_size(&fb));

  uint32_t *al = fb_alloc(&fb);
  TEST_ASSERT_NOT_NULL(al);

  for (int i = 0; i < 9; i++) {
    al = fb_alloc(&fb);
    TEST_ASSERT_NOT_NULL(al);
  }

  uint32_t *n = fb_alloc(&fb);
  TEST_ASSERT_NULL(n);

  fb_free(&fb, al);

  al = fb_alloc(&fb);
  TEST_ASSERT_NOT_NULL(al);
}

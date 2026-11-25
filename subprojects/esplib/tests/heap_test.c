
#include "collections.h"
#include "unity.h"
#include <stdint.h>

int cmp(const void *a, const void *b) {
  return *(uint32_t *)a > *(uint32_t *)b ? -1 : 0;
}

void test_heap(void) {
  uint32_t backing[10];
  Heap heap;
  heap_new(&heap, backing, sizeof(backing), sizeof(uint32_t), cmp);

  TEST_ASSERT_EQUAL(0, heap_size(&heap));
  TEST_ASSERT_EQUAL(9, heap_capacity(&heap));

  TEST_ASSERT_TRUE(heap_push(&heap, &(int){68}));
  TEST_ASSERT_TRUE(heap_push(&heap, &(int){20}));
  TEST_ASSERT_TRUE(heap_push(&heap, &(int){42}));
  TEST_ASSERT_TRUE(heap_push(&heap, &(int){100}));

  TEST_ASSERT_EQUAL(4, heap_size(&heap));

  int out;
  heap_peek(&heap, &out);
  TEST_ASSERT_EQUAL(100, out);

  heap_pop(&heap, &out);
  TEST_ASSERT_EQUAL(100, out);

  heap_pop(&heap, &out);
  TEST_ASSERT_EQUAL(68, out);

  heap_pop(&heap, &out);
  TEST_ASSERT_EQUAL(42, out);

  heap_pop(&heap, &out);
  TEST_ASSERT_EQUAL(20, out);

  TEST_ASSERT_FALSE(heap_pop(&heap, &out));
}

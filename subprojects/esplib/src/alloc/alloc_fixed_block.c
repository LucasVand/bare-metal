#include "alloc.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

size_t fb_size(FixedBlockAllocator *allocator) { return allocator->size; }
size_t fb_capacity(FixedBlockAllocator *allocator) {
  return allocator->buf_size / allocator->block_size;
}

size_t fb_new(FixedBlockAllocator *allocator, void *buf, size_t buf_size,
              size_t block_size) {
  if (!allocator || !buf || block_size < sizeof(alloc_fb_allocation)) {
    return 0;
  }
  allocator->buf_size = buf_size;
  allocator->block_size = block_size;
  allocator->buf = buf;
  allocator->free_list = buf;
  allocator->size = 0;

  uint8_t *byte_buf = buf;
  size_t count = 0;
  alloc_fb_allocation *a;
  for (size_t i = 0; i < buf_size; i += block_size) {
    a = (alloc_fb_allocation *)&byte_buf[i];

    a->next = (void *)a + block_size;
    count++;
  }
  a->next = NULL;

  return count;
}

void *fb_alloc(FixedBlockAllocator *allocator) {
  if (!allocator || !allocator->free_list) {
    return NULL;
  }
  alloc_fb_allocation *a = allocator->free_list;
  allocator->free_list = allocator->free_list->next;
  allocator->size += 1;
  return a;
}

void fb_free(FixedBlockAllocator *allocator, void *allocation) {
  if (!allocator || !allocation) {
    return;
  }

  alloc_fb_allocation *a = allocation;
  a->next = allocator->free_list;

  allocator->free_list = a;
  allocator->size -= 1;
}

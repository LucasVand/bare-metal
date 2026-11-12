#include "alloc.h"
#include <stdbool.h>
#include <stddef.h>

size_t alloc_fb_new(alloc_fb *allocator, void *buf, size_t buf_size,
                    size_t block_size) {
  if (!allocator || !buf || block_size < sizeof(alloc_fb_allocation)) {
    return 0;
  }
  allocator->buf_size = buf_size;
  allocator->block_size = block_size;
  allocator->buf = buf;
  allocator->free_list = buf;

  size_t count = 0;
  alloc_fb_allocation *a;
  for (size_t i = 0; i < buf_size; i += sizeof(alloc_fb_allocation)) {
    a = (alloc_fb_allocation *)&buf[i];

    a->next = (void *)a + block_size;
    count++;
  }
  a->next = NULL;

  return count;
}

void *alloc_fb_alloc(alloc_fb *allocator) {
  if (!allocator || !allocator->free_list) {
    return NULL;
  }
  alloc_fb_allocation *a = allocator->free_list;
  allocator->free_list = allocator->free_list->next;
  return a;
}

void alloc_fb_free(alloc_fb *allocator, void *allocation) {
  if (!allocator || !allocation) {
    return;
  }

  alloc_fb_allocation *a = allocation;
  a->next = allocator->free_list;

  allocator->free_list = a;
}

#pragma once

// Fixed size block allocator

#include <stdbool.h>
#include <stddef.h>

typedef struct {
  union {
    void *next;
    void *buf;
  };
} alloc_fb_allocation;

typedef struct {
  alloc_fb_allocation *free_list;
  size_t block_size;
  void *buf;
  size_t buf_size;
  size_t size;
} FixedBlockAllocator;

size_t fb_new(FixedBlockAllocator *allocator, void *buf, size_t buf_size,
              size_t block_size);

void *fb_alloc(FixedBlockAllocator *allocator);
void fb_free(FixedBlockAllocator *allocator, void *allocation);
size_t fb_size(FixedBlockAllocator *allocator);
size_t fb_capacity(FixedBlockAllocator *allocator);

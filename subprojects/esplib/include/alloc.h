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
} alloc_fb;

size_t alloc_fb_new(alloc_fb *allocator, void *buf, size_t buf_size,
                    size_t block_size);

void *alloc_fb_alloc(alloc_fb *allocator);
void alloc_fb_free(alloc_fb *allocator, void *allocation);

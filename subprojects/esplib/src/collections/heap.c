
#include "collections.h"
#include "mem.h"
#include <stdint.h>

void heap_new(Heap *heap, void *backing, size_t backing_size,
              size_t element_size, int (*cmp)(const void *, const void *)) {
  heap->backing = backing;
  heap->backing_size = backing_size;
  heap->element_size = element_size;
  heap->cmp = cmp;
  heap->capacity = (backing_size / element_size) - 1;
  heap->size = 0;
}

size_t heap_size(const Heap *heap) { return heap->size; }
size_t heap_element_size(const Heap *heap) { return heap->element_size; }
size_t heap_capacity(const Heap *heap) { return heap->capacity; }

static inline void *heap_get(const Heap *heap, size_t idx) {
  uintptr_t ptr = (uintptr_t)heap->backing;
  ptr += heap->element_size * idx;
  return (void *)ptr;
}
static inline void heap_set(Heap *heap, size_t idx, const void *element) {
  void *ptr = heap_get(heap, idx);
  memcpy(ptr, element, heap->element_size);
}

static inline void swp(Heap *heap, void *a, void *b) {
  memcpy(heap->backing, a, heap->element_size);
  memcpy(a, b, heap->element_size);
  memcpy(b, heap->backing, heap->element_size);
}
static void sift_up(Heap *heap, size_t idx) {
  while (idx > 1) {
    size_t parent = idx >> 1;
    void *idx_ptr = heap_get(heap, idx);
    void *parent_ptr = heap_get(heap, parent);

    if (heap->cmp(idx_ptr, parent_ptr) < 0) {
      swp(heap, idx_ptr, parent_ptr);
      idx = parent;
    } else {
      break;
    }
  }
}
static void sift_down(Heap *heap, size_t idx) {
  while ((idx << 1) <= heap->size) {
    size_t smallest = idx;
    size_t left = idx << 1;
    size_t right = left | 1;

    if (heap->cmp(heap_get(heap, left), heap_get(heap, smallest)) < 0) {
      smallest = left;
    }
    if (right <= heap->size &&
        heap->cmp(heap_get(heap, right), heap_get(heap, smallest)) < 0) {
      smallest = right;
    }

    if (smallest != idx) {
      swp(heap, heap_get(heap, idx), heap_get(heap, smallest));
      idx = smallest;
    } else {
      break;
    }
  }
}

bool heap_push(Heap *heap, const void *element) {
  if (heap->size >= heap->capacity) {
    return false;
  }
  heap->size += 1;
  heap_set(heap, heap->size, element);
  sift_up(heap, heap->size);
  return true;
}

bool heap_pop(Heap *heap, void *out) {
  if (heap->size == 0) {
    return false;
  }

  memcpy(out, heap_get(heap, 1), heap->element_size);
  if (heap->size > 1) {
    heap_set(heap, 1, heap_get(heap, heap->size));
  }
  heap->size -= 1;
  if (heap->size > 1) {
    sift_down(heap, 1);
  }
  return true;
}

bool heap_peek(const Heap *heap, void *out) {
  if (heap->size == 0) {
    return false;
  }

  memcpy(out, heap_get(heap, 1), heap->element_size);
  return true;
}

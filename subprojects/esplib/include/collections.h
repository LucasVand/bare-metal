
#include <stdbool.h>
#include <stddef.h>

typedef struct {
  void *backing;
  size_t backing_size;

  size_t element_size;
  size_t capacity;
  size_t size;

  int (*cmp)(const void *, const void *);
} Heap;

void heap_new(Heap *heap, void *backing, size_t backing_size,
              size_t element_size, int (*cmp)(const void *, const void *));
size_t heap_size(const Heap *heap);
size_t heap_element_size(const Heap *heap);
size_t heap_capacity(const Heap *heap);

bool heap_push(Heap *heap, const void *element);

bool heap_pop(Heap *heap, void *out);

bool heap_peek(const Heap *heap, void *out);

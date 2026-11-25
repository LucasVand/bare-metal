
#include "task.h"
#include "alloc.h"
#include "collections.h"
#include "mem.h"
#include "stdio.h"
#include "task_int.h"
#include <stddef.h>
#include <stdint.h>

extern void __context_switch(uint32_t **old_sp, uint32_t **new_sp);
extern void __bootstrap_task();
void task_yield(void);
void end_scheduler(void);

#define TASK_MAX 5
#define TASK_STACK_SIZE 2048

Task *task_queue_backing[TASK_MAX];
Heap task_queue;

Task task_allocator_backing[TASK_MAX];
FixedBlockAllocator task_allocator;

void *task_stack_allocator_backing[TASK_MAX][TASK_STACK_SIZE];
FixedBlockAllocator task_stack_allocator;

Task *task_current = NULL;

uint32_t *scheduler_entry_sp;

int task_cmp(const void *a, const void *b) {
  Task *at = *(Task **)a;
  Task *bt = *(Task **)b;

  if (at->priority < bt->priority) {
    return 1;
  } else if (at->priority == bt->priority) {
    return 0;
  }
  return -1;
}
void scheduler_allocators_start() {
  printf("Starting scheduler allocators\n");
  heap_new(&task_queue, task_queue_backing, sizeof(task_queue_backing),
           sizeof(Task *), task_cmp);

  fb_new(&task_allocator, task_allocator_backing,
         sizeof(task_allocator_backing), sizeof(Task));

  fb_new(&task_stack_allocator, task_stack_allocator_backing,
         sizeof(task_stack_allocator_backing), TASK_STACK_SIZE);
}

void task_current_status_set(task_status_t status) {
  task_current->status = status;
}
task_status_t task_current_status_get() { return task_current->status; }

void task_stack_setup(Task *task) {
  int ctx_size = 116 / 4;
  int total_size = ctx_size + 2;
  int32_t *top = (int32_t *)task->sp;
  memset((void *)(top - total_size), 0, total_size);

  // extra 8 for the actual saved entry point and args not the bootstrap task
  top -= 1;
  *top = (uint32_t)task->arg;

  top -= 1;
  *top = (uint32_t)task->entry;

  top -= ctx_size;
  *top = (uint32_t)__bootstrap_task;

  task->sp = (void *)top;
}

static void task_queue_push(Task **task) { heap_push(&task_queue, task); }

Task *task_queue_next_ready() {
  Task *out = NULL;
  heap_pop(&task_queue, &out);

  while (out != NULL && out->status != READY) {
    if (!heap_pop(&task_queue, &out)) {
      return NULL;
    }
  }
  return out;
}

TaskHandle task_create(void (*entry)(void *), void *args, size_t priority) {
  if (fb_size(&task_allocator) >= TASK_MAX) {
    return NULL;
  }

  Task *t = fb_alloc(&task_allocator);
  void *stack = fb_alloc(&task_stack_allocator);

  t->entry = entry;
  t->arg = args;
  t->status = READY;
  t->sp = (void *)&stack[TASK_STACK_SIZE - 1];
  t->original_sp = t->sp;
  t->priority = priority;

  task_stack_setup(t);
  task_queue_push(&t);

  return (TaskHandle)t;
}

void task_cancel(TaskHandle handle) {
  Task *task = (Task *)handle;
  task->status = CANCELLED;
}

void task_switch(Task *new_task) {
  /* printf("Starting new task (Entry: %p) (SP: %p)\r\n", new_task->entry, */
  /*        new_task->sp); */
  void *old_sp;
  if (task_current == NULL) {
    old_sp = &scheduler_entry_sp;
  } else {
    old_sp = &task_current->sp;
  }

  task_current = new_task;
  __context_switch(old_sp, &new_task->sp);
}

void scheduler_start() {
  printf("Starting scheduler \r\n");

  Task *new_task = task_queue_next_ready();
  if (new_task == NULL) {
    return;
  }

  asm volatile("mv %0, sp" : "=r"(scheduler_entry_sp));

  task_switch(new_task);
}
void task_free(Task *task) {
  fb_free(&task_stack_allocator, task->original_sp);
  fb_free(&task_allocator, task);
}

void task_yield() {
  if (task_current->status == READY) {
    task_queue_push(&task_current);
  }
  if (task_current->status == CANCELLED) {
    task_free(task_current);
  }

  Task *next_task = task_queue_next_ready();
  if (next_task == NULL) {
    end_scheduler();
  }
  if (next_task == task_current) {
    return;
  }
  task_switch(next_task);
}

void end_scheduler(void) {
  __context_switch(&task_current->sp, &scheduler_entry_sp);
}

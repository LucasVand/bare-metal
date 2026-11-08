
#include "scheduler_int.h"
#include <stddef.h>
#include <stdio.h>

#define TASK_QUEUE_SIZE 64

task_t queue[TASK_QUEUE_SIZE];

size_t head = 0;
size_t tail = 0;

task_t *task_queue_pop() {
  if (head == tail) {
    return NULL;
  }
  tail = (tail + 1) % TASK_QUEUE_SIZE;
  task_t *t = &queue[tail];
  return t;
}

task_handle_t task_create(void (*entry)(void *), void *args) {
  if ((head + 1) % TASK_QUEUE_SIZE == tail) {
    return NULL;
  }

  head = (head + 1) % TASK_QUEUE_SIZE;
  task_t *t = &queue[head];
  t->entry = entry;
  t->arg = args;
  t->status = READY;

  return (task_handle_t)t;
}

size_t task_cancel(task_handle_t handle) {
  task_t *task = (task_t *)handle;
  task->status = CANCELLED;
  return 1;
}

void scheduler_loop() {

  task_t *task = task_queue_pop();
  while (task != NULL) {
    task->entry(task->arg);

    task = task_queue_pop();
  }
}

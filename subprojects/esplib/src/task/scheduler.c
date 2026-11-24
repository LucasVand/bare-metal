
#include "mem.h"
#include "stdio.h"
#include "task_int.h"
#include <stddef.h>
#include <stdint.h>

extern void __context_switch(uint32_t **old_sp, uint32_t **new_sp);
extern void __bootstrap_task();
void task_yield(void);
void end_scheduler(void);

#define TASK_QUEUE_SIZE 5

task_t *queue[TASK_QUEUE_SIZE];
task_t tasks[TASK_QUEUE_SIZE];
uint32_t stacks[TASK_QUEUE_SIZE][2048];

task_t *task_current = NULL;

uint32_t *scheduler_entry_sp;

size_t task_count = 0;
size_t head = 0;
size_t tail = 0;

void task_current_status_set(task_status_t status) {
  task_current->status = status;
}
task_status_t task_current_status_get() { return task_current->status; }

void task_stack_setup(task_t *task) {
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

static void task_queue_push(task_t *task) {
  size_t next = (head + 1) % TASK_QUEUE_SIZE;
  if (next == tail) {
    return;
  }
  queue[head] = task;
  head = next;
}

task_t *task_queue_pop() {
  if (head == tail) {
    return NULL;
  }
  task_t *t = queue[tail];
  tail = (tail + 1) % TASK_QUEUE_SIZE;
  return t;
}

task_handle_t task_create(void (*entry)(void *), void *args) {
  if ((head + 1) % TASK_QUEUE_SIZE == tail) {
    return NULL;
  }
  if (task_count >= TASK_QUEUE_SIZE) {
    return NULL;
  }

  task_t *t = &tasks[task_count];
  t->entry = entry;
  t->arg = args;
  t->status = READY;
  t->sp = (void *)&stacks[task_count][2047];
  task_count++;

  task_stack_setup(t);
  task_queue_push(t);

  return (task_handle_t)t;
}

size_t task_cancel(task_handle_t handle) {
  task_t *task = (task_t *)handle;
  task->status = CANCELLED;
  return 1;
}

void task_switch(task_t *new_task) {
  printf("Starting new task (Entry: %p) (SP: %p)\r\n", new_task->entry,
         new_task->sp);
  void *old_sp;
  if (task_current == NULL) {
    old_sp = &scheduler_entry_sp;
  } else {
    old_sp = &task_current->sp;
  }

  task_current = new_task;
  __context_switch(old_sp, &new_task->sp);
}

void scheduler_loop() {
  printf("Starting scheduler \r\n");

  task_t *new_task = task_queue_pop();
  if (new_task == NULL) {
    return;
  }

  asm volatile("mv %0, sp" : "=r"(scheduler_entry_sp));

  task_switch(new_task);
}

void task_yield() {
  if (task_current->status == READY) {
    task_queue_push(task_current);
  }

  task_t *next_task = task_queue_pop();
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

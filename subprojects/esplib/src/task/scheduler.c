
#include "mem.h"
#include "stdio.h"
#include "task_int.h"
#include <stddef.h>
#include <stdint.h>

extern void __context_switch(uint32_t **old_sp, uint32_t **new_sp);
extern void __task_bootstrap(void);
void task_yield(void);

#define TASK_QUEUE_SIZE 5

task_t *queue[TASK_QUEUE_SIZE];
task_t tasks[TASK_QUEUE_SIZE];
uint32_t stacks[TASK_QUEUE_SIZE][2048];

task_t *task_current;
task_t task_inital;

uint32_t task_inital_stack[2048];

size_t task_count = 0;
size_t head = 0;
size_t tail = 0;

void end_of_scheduler(void *arg) { printf("End of scheduler\r\n"); }

void task_stack_setup(task_t *task) {
  uintptr_t top = (uintptr_t)task->sp & ~(uintptr_t)0xF;
  uintptr_t bootstrap_sp = top - 48;
  uint32_t *ctx_frame = (uint32_t *)(top - 160);
  uint32_t *caller_frame = (uint32_t *)(top - 176);

  memset((void *)(top - 176), 0, 176);

  *(uint32_t *)(bootstrap_sp - 12) = (uint32_t)top;

  ctx_frame[0] = ((uint32_t)__task_bootstrap & 0x3FFFFFFFu) | (2u << 30);
  ctx_frame[1] = 0x00060000u;
  ctx_frame[2] = 0;
  ctx_frame[3] = 0;
  ctx_frame[4] = 0;

  caller_frame[0] = 0;
  caller_frame[1] = (uint32_t)bootstrap_sp;
  caller_frame[2] = (uint32_t)task->arg;
  caller_frame[3] = (uint32_t)task->entry;

  task->sp = ctx_frame;
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
  task_t *old_task = task_current;
  task_current = new_task;
  __context_switch(&old_task->sp, &new_task->sp);
  printf("Return from task (Entry: %p) (SP: %p)\r\n", new_task->entry,
         new_task->sp);
}

void scheduler_loop() {
  printf("Starting scheduler \r\n");
  task_inital.sp = &task_inital_stack[2047];
  task_inital.entry = end_of_scheduler;

  printf("Starting inital task (Entry: %p) (SP: %p)\r\n", task_inital.sp,
         task_inital.entry);

  task_current = &task_inital;

  printf("Start task setup (Entry: %p) (SP: %p)\r\n", task_inital.entry,
         task_inital.sp);
  task_t *new_task = task_queue_pop();
  if (new_task == NULL) {
    return;
  }

  task_switch(new_task);
}

void task_exit(void) {
  task_current->status = CANCELLED;
  task_yield();
  while (1) {
  }
}

void task_yield() {
  if (task_current != &task_inital && task_current->status == READY) {
    task_queue_push(task_current);
  }

  task_t *next_task = task_queue_pop();
  if (next_task == NULL) {
    next_task = &task_inital;
  }
  if (next_task == task_current) {
    return;
  }
  task_switch(next_task);
}


#include "mem.h"
#include "task_int.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

extern void __context_switch(void *old_sp, void *new_sp);
extern void __task_stack_setup(void *sp, void *entry);

#define TASK_QUEUE_SIZE 5

task_t queue[TASK_QUEUE_SIZE];
uint32_t stacks[TASK_QUEUE_SIZE][2048];

task_t *task_current;
task_t task_inital;

uint32_t task_inital_stack[2048];

size_t head = 0;
size_t tail = 0;

void end_of_scheduler(void *arg) { printf("End of scheduler\r\n"); }

void task_stack_setup(task_t *task) {
  uintptr_t top = (uintptr_t)task->sp;
  uint32_t *ctx_frame = (uint32_t *)(top - 128);

  memset(ctx_frame, 0, 80 + 48);

  // create a fake underflow frame
  uint32_t *base_sp = (uint32_t *)top - 48;
  base_sp[-4] = ((uint32_t)end_of_scheduler & 0x3FFFFFFFu) | (2u << 30);
  base_sp[-3] = (uint32_t)top;

  uint32_t raw_entry = (uint32_t)task->entry;

  // set the entry make sure it looks like it came from a call8
  uint32_t call8_mask = (2u << 30);
  uint32_t encoded_ra = (raw_entry & 0x3FFFFFFFu) | call8_mask;

  // set it to the first spot on the stack
  ctx_frame[0] = encoded_ra;

  // set the PS state
  ctx_frame[64 / 4] = (uint32_t)(1u << 18);

  // set the new stack
  task->sp = ctx_frame;
}

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
  t->sp = (void *)&stacks[head][2047];

  task_stack_setup(t);

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
  // get the current stack ptr
  /* __asm__ volatile("mov %0, sp" : "=r"(task_inital.sp)); */
  task_inital.sp = &task_inital_stack[2047];
  task_inital.entry = end_of_scheduler;

  printf("Starting inital task (Entry: %p) (SP: %p)\r\n", task_inital.sp,
         task_inital.entry);
  task_stack_setup(&task_inital);

  task_current = &task_inital;

  printf("Start task setup (Entry: %p) (SP: %p)\r\n", task_inital.entry,
         task_inital.sp);
  task_t *new_task = task_queue_pop();
  if (new_task == NULL) {
    return;
  }

  task_switch(new_task);
}

void task_yield() {
  task_t *next_task = task_queue_pop();
  if (next_task == NULL) {
    task_switch(&task_inital);
    return;
  }
  task_switch(next_task);
}

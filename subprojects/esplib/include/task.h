
#pragma once

#include <stddef.h>

typedef void *TaskHandle;
typedef enum {
  READY = 1,
  CANCELLED = 0,
} task_status_t;

TaskHandle task_create(void (*entry)(void *), void *args, size_t priority);
void task_cancel(TaskHandle handle);

void task_yield();

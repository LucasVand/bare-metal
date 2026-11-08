
#pragma once

#include "task.h"
#include <stddef.h>

typedef enum {
  READY,
  CANCELLED,
} task_status_t;

typedef struct {
  void (*entry)(void *);
  void *arg;
  task_status_t status;
} task_t;

void scheduler_loop();

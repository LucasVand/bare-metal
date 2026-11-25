
#pragma once

#include "task.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
  void (*entry)(void *);
  void *arg;
  task_status_t status;
  uint32_t *sp;
  uint32_t *original_sp;
  size_t priority;
} Task;

/// Starts the scheduler, will exit after all tasks are completed
void scheduler_start();

/// Initalizes the required allocators to enable task creation
void scheduler_allocators_start();

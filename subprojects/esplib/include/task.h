
#pragma once

#include <stddef.h>

typedef void *task_handle_t;

task_handle_t task_create(void (*entry)(void *), void *args);
size_t task_cancel(task_handle_t handle);

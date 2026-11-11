#include "stdio.h"
#include "task.h"

void say(void *arg) { printf("this is from a task1\r\n"); }

void say2(void *arg) { printf("this is from a task2\r\n"); }

void main(void *arg) {
  printf("hello from main before\n\r");
  task_create(say, NULL);
  task_create(say2, NULL);

  printf("hello from main new\n\r");
}

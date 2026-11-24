#include "stdio.h"
#include "task.h"

void say(void *arg) {
  printf("this is from a task1 (number: %d) \r\n", *(int *)arg);
}

void say2(void *arg) { printf("this is from a task2\r\n"); }

int number = 10;

void main(void *arg) {
  printf("hello from main before\n\r");
  task_create(say, &number);
  task_create(say2, NULL);

  task_yield();

  printf("hello from main new\n\r");
}

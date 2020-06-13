#include "../thread.h"
#include <stdio.h>
#include <assert.h>

static void *thread_1()
{
    for (int j = 0; j < 1500000; j++)
    {
      printf("coucou\n");
    }


  thread_exit(NULL);
}

static void *thread_2()
{
  for (int j = 0; j < 1500000; j++)
  {
    printf("salut\n");
  }
  thread_exit(NULL);
}

int main()
{
  int err;
  thread_t thread1, thread2;

  printf("le main lance 2 threads...\n");
  err = thread_create(&thread1, thread_1, "thread1");
  assert(!err);
  err = thread_create(&thread2, thread_2, "thread2");
  err = thread_join(thread2, NULL);
  assert(!err);
  err = thread_join(thread1, NULL);
  assert(!err);
  return 0;
}

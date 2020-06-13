#ifndef USE_PTHREAD
  #include "../thread.h"
#else
  #define _GNU_SOURCE
  #include <pthread.h>
#endif
#include <stdio.h>
#include <assert.h>

static void * threadfunc(void * arg)
{
  char *name = arg;
  #ifndef USE_PTHREAD
    printf("je suis le thread %p, lancé avec l'argument %s\n", (void*) thread_self(), name);
    thread_yield();
    printf("je suis encore le thread %p, lancé avec l'argument %s\n", (void*) thread_self(), name);
    thread_exit(arg);
  #else
    printf("je suis le thread %p, lancé avec l'argument %s\n", (void*) pthread_self(), name);
    pthread_yield();
    printf("je suis encore le thread %p, lancé avec l'argument %s\n", (void*) pthread_self(), name);
    pthread_exit(arg);
  #endif
}

int main()
{
  #ifndef USE_PTHREAD
    thread_t thread1, thread2;
  #else
    pthread_t thread1, thread2;
  #endif
  void *retval1, *retval2;
  int err;

  printf("le main lance 2 threads...\n");
  #ifndef USE_PTHREAD
    err = thread_create(&thread1, threadfunc, "thread1");
    assert(!err);
    err = thread_create(&thread2, threadfunc, "thread2");
  #else
    err = pthread_create(&thread1, NULL, threadfunc, "thread1");
    assert(!err);
    err = pthread_create(&thread2, NULL, threadfunc, "thread2");
  #endif
  assert(!err);
  printf("le main a lancé les threads %p et %p\n",(void*) thread1, (void*) thread2);

  printf("le main attend les threads\n");
  #ifndef USE_PTHREAD
    err = thread_join(thread2, &retval2);
    assert(!err);
    err = thread_join(thread1, &retval1);
  #else
    err = pthread_join(thread2, &retval2);
    assert(!err);
    err = pthread_join(thread1, &retval1);
  #endif
  assert(!err);
  printf("les threads ont terminé en renvoyant '%s' and '%s'\n", (char *) retval1, (char *) retval2);

  return 0;
}

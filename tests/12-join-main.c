#ifndef USE_PTHREAD
  #include "../thread.h"
#else
  #define _GNU_SOURCE
  #include <pthread.h>
#endif

#include <stdio.h>
#include <assert.h>


/* test du join du main par un fils.
 *
 * le programme doit terminer correctement (faire le printf et s'arreter entièrement).
 * valgrind doit être content.
 *
 * support nécessaire:
 * - thread_create()
 * - thread_self() dans le main
 * - thread_exit() dans le main
 * - thread_join() du main par un autre thread
 */

#ifndef USE_PTHREAD
  thread_t thmain = NULL; /* si votre thread_t est un pointeur */
#else
  pthread_t thmain = 0; /* si votre thread_t est un pointeur */
#endif

static void * thfunc(void *dummy __attribute__((unused)))
{
  void *res;
  int err;

  #ifndef USE_PTHREAD
    err = thread_join(thmain, &res);
  #else
    err = pthread_join(thmain, &res);
  #endif
  assert(!err);
  assert(res == (void*) 0xdeadbeef);
  printf("main terminé OK\n");
  return NULL;
}

int main()
{
  #ifndef USE_PTHREAD
    thread_t th;
  #else
    pthread_t th;
  #endif
  int err;

  #ifndef USE_PTHREAD
    thmain = thread_self();
  #else
    thmain = pthread_self();
  #endif

  #ifndef USE_PTHREAD
    err = thread_create(&th, thfunc, NULL);
  #else
    err = pthread_create(&th, NULL, thfunc, NULL);
  #endif
  assert(!err);

  #ifndef USE_PTHREAD
    thread_exit((void*) 0xdeadbeef);
  #else
    pthread_exit((void*) 0xdeadbeef);
  #endif
  return 0; /* unreachable, shut up the compiler */
}

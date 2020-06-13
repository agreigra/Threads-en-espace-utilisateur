#ifndef USE_PTHREAD
  #include "../thread.h"
#else
  #define _GNU_SOURCE
  #include <pthread.h>
#endif
#include <stdio.h>
#include <assert.h>

/* test du join, avec ou sans thread_exit.
 *
 * le programme doit retourner correctement.
 * valgrind doit être content.
 *
 * support nécessaire:
 * - thread_create()
 * - thread_exit()
 * - retour sans thread_exit()
 * - thread_join() avec récupération valeur de retour, avec et sans thread_exit()
 */

static void * thfunc(void *dummy __attribute__((unused)))
{
  #ifndef USE_PTHREAD
    thread_exit((void*)0xdeadbeef);
  #else
    pthread_exit((void*)0xdeadbeef);
  #endif
  return NULL; /* unreachable, shut up the compiler */
}

static void * thfunc2(void *dummy __attribute__((unused)))
{
  return (void*) 0xbeefdead;
}

int main()
{
  #ifndef USE_PTHREAD
    thread_t th, th2;
  #else
    pthread_t th, th2;
  #endif
  int err;
  void *res = NULL;

  #ifndef USE_PTHREAD
    err = thread_create(&th, thfunc, NULL);
  #else
    err = pthread_create(&th, NULL, thfunc, NULL);
  #endif
  assert(!err);
  #ifndef USE_PTHREAD
    err = thread_create(&th2, thfunc2, NULL);
  #else
    err = pthread_create(&th2, NULL, thfunc2, NULL);
  #endif
  assert(!err);

  #ifndef USE_PTHREAD
    err = thread_join(th, &res);
  #else
    err = pthread_join(th, &res);
  #endif
  assert(!err);
  assert(res == (void*) 0xdeadbeef);

  #ifndef USE_PTHREAD
    err = thread_join(th2, &res);
  #else
    err = pthread_join(th2, &res);
  #endif
  assert(!err);
  assert(res == (void*) 0xbeefdead);

  printf("join OK\n");
  return 0;
}

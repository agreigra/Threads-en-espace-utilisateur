#ifndef USE_PTHREAD
  #include "../thread.h"
#else
  #define _GNU_SOURCE
  #include <pthread.h>
#endif
#include <stdio.h>
#include <assert.h>

/* test de switchs.
 *
 * les affichages doivent être dans le bon ordre (fifo)
 * le programme doit retourner correctement.
 * valgrind doit être content.
 *
 * support nécessaire:
 * - thread_create()
 * - thread_yield() depuis ou vers le main
 * - thread_exit()
 * - thread_join() avec récupération de la valeur de retour, ou sans
 */

static void * thfunc(void *id)
{
  int err, i;
  for(i=0; i<10; i++) {
    printf("%s yield vers un autre thread\n", (char*) id);
    #ifndef USE_PTHREAD
      err = thread_yield();
    #else
      err = pthread_yield();
    #endif
    assert(!err);
  }

  printf("%s terminé\n", (char*) id);
  #ifndef USE_PTHREAD
    thread_exit(NULL);
  #else
    pthread_exit(NULL);
  #endif
  return (void*) 0xdeadbeef; /* unreachable, shut up the compiler */
}

int main()
{
  #ifndef USE_PTHREAD
    thread_t th1,th2,th3;
  #else
    pthread_t th1,th2,th3;
  #endif
  void *res;
  int err, i;

  #ifndef USE_PTHREAD
    err = thread_create(&th1, thfunc, "fils1");
  #else
    err = pthread_create(&th1, NULL, thfunc, "fils1");
  #endif
  assert(!err);
  #ifndef USE_PTHREAD
    err = thread_create(&th2, thfunc, "fils2");
  #else
    err = pthread_create(&th2, NULL, thfunc, "fils2");
  #endif
  assert(!err);
  #ifndef USE_PTHREAD
    err = thread_create(&th3, thfunc, "fils3");
  #else
    err = pthread_create(&th3, NULL, thfunc, "fils3");
  #endif
  assert(!err);
  /* des switchs avec l'autre thread */
  for(i=0; i<20; i++) {
    printf("le main yield vers un fils\n");
    #ifndef USE_PTHREAD
      err = thread_yield();
    #else
      err = pthread_yield();
    #endif
    assert(!err);
  }

  #ifndef USE_PTHREAD
    err = thread_join(th3, &res);
  #else
    err = pthread_join(th3, &res);
  #endif
  assert(!err);
  assert(res == NULL);
  #ifndef USE_PTHREAD
    err = thread_join(th2, NULL); /* on ignore ce code de retour */
  #else
    err = pthread_join(th2, NULL); /* on ignore ce code de retour */
  #endif
  assert(!err);
  #ifndef USE_PTHREAD
    err = thread_join(th1, &res);
  #else
    err = pthread_join(th1, &res);
  #endif
  assert(!err);
  assert(res == NULL);

  printf("main terminé\n");
  return 0;
}

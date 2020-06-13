#ifndef USE_PTHREAD
  #include "../thread.h"
#else
  #define _GNU_SOURCE
  #include <pthread.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>

/* fibonacci.
 *
 * la durée doit être proportionnel à la valeur du résultat.
 * valgrind doit être content.
 * jusqu'à quelle valeur cela fonctionne-t-il ?
 *
 * support nécessaire:
 * - thread_create()
 * - thread_join() avec récupération de la valeur de retour
 * - retour sans thread_exit()
 */

static void * fibo(void *_value)
{
  #ifndef USE_PTHREAD
    thread_t th, th2;
  #else
    pthread_t th, th2;
  #endif
  int err;
  void *res = NULL, *res2 = NULL;
  unsigned long value = (unsigned long) _value;

  /* on passe un peu la main aux autres pour eviter de faire uniquement la partie gauche de l'arbre */
  #ifndef USE_PTHREAD
    thread_yield();
  #else
    pthread_yield();
  #endif

  if (value < 3)
    return (void*) 1;

  #ifndef USE_PTHREAD
    err = thread_create(&th, fibo, (void*)(value-1));
  #else
    err = pthread_create(&th, NULL, fibo, (void*)(value-1));
  #endif
  assert(!err);
  #ifndef USE_PTHREAD
    err = thread_create(&th2, fibo, (void*)(value-2));
  #else
    err = pthread_create(&th2, NULL, fibo, (void*)(value-2));
  #endif
  assert(!err);

  #ifndef USE_PTHREAD
    err = thread_join(th, &res);
  #else
    err = pthread_join(th, &res);
  #endif
  assert(!err);
  #ifndef USE_PTHREAD
    err = thread_join(th2, &res2);
  #else
    err = pthread_join(th2, &res2);
  #endif
  assert(!err);

  return (void*)((unsigned long) res + (unsigned long) res2);
}

int main(int argc, char *argv[])
{
  unsigned long value, res;
  struct timeval tv1, tv2;
  double s;

  if (argc < 2) {
    printf("argument manquant: entier x pour lequel calculer fibonacci(x)\n");
    return -1;
  }

  value = atoi(argv[1]);
  gettimeofday(&tv1, NULL);
  res = (unsigned long) fibo((void *)value);
  gettimeofday(&tv2, NULL);
  s = (tv2.tv_sec-tv1.tv_sec) + (tv2.tv_usec-tv1.tv_usec) * 1e-6;

  printf("fibo de %ld = %ld en %e s\n", value, res, s );

  return 0;
}

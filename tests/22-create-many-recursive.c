#ifndef USE_PTHREAD
  #include "../thread.h"
#else
  #define _GNU_SOURCE
  #include <pthread.h>
#endif
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>

/* test de plein de create-destroy récursif.
 *
 * valgrind doit etre content.
 * la durée du programme doit etre proportionnelle au nombre de threads donnés en argument.
 * jusqu'à combien de threads cela fonctionne-t-il ?
 *
 * support nécessaire:
 * - thread_create()
 * - retour sans thread_exit()
 * - thread_join() avec récupération de la valeur de retour
 */

static void * thfunc(void *_nb)
{
  unsigned long nb = (unsigned long) _nb;
  if ((unsigned long) nb > 0) {
    #ifndef USE_PTHREAD
      thread_t th;
    #else
      pthread_t th;
    #endif
    int err;
    void *res;
    #ifndef USE_PTHREAD
      err = thread_create(&th, thfunc, _nb-1);
    #else
      err = pthread_create(&th, NULL, thfunc, _nb-1);
    #endif
    assert(!err);
    #ifndef USE_PTHREAD
      err = thread_join(th, &res);
    #else
      err = pthread_join(th, &res);
    #endif
    assert(!err);
    assert(res == _nb-1);
  }
  return _nb;
}

int main(int argc, char *argv[])
{
  unsigned long nb;
  struct timeval tv1, tv2;
  unsigned long us;

  if (argc < 2) {
    printf("argument manquant: nombre de threads\n");
    return -1;
  }

  nb = atoi(argv[1]);

  gettimeofday(&tv1, NULL);
  thfunc((void*) nb);
  gettimeofday(&tv2, NULL);
  us = (tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec);
  printf("%ld threads créés et détruits récursivement en %lu us\n", nb, us);
  return 0;
}

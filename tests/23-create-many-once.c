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

/* test de plein de create, puis plein de join quand ils ont tous fini
 *
 * valgrind doit etre content.
 * la durée du programme doit etre proportionnelle au nombre de threads donnés en argument.
 * jusqu'à combien de threads cela fonctionne-t-il ?
 *
 * support nécessaire:
 * - thread_create()
 * - thread_exit()
 * - thread_join() sans récupération de la valeur de retour
 */

static void * thfunc(void *dummy __attribute__((unused)))
{
  #ifndef USE_PTHREAD
    thread_exit(NULL);
  #else
    pthread_exit(NULL);
  #endif
  return (void*) 0xdeadbeef; /* unreachable, shut up the compiler */
}

int main(int argc, char *argv[])
{
  #ifndef USE_PTHREAD
    thread_t *th;
  #else
    pthread_t *th;
  #endif

  int err, i, nb;
  struct timeval tv1, tv2;
  unsigned long us;

  if (argc < 2) {
    printf("argument manquant: nombre de threads\n");
    return -1;
  }

  nb = atoi(argv[1]);

  th = malloc(nb*sizeof(*th));
  if (!th) {
    perror("malloc");
    return -1;
  }

  gettimeofday(&tv1, NULL);

  /* on cree tous les threads */
  for(i=0; i<nb; i++) {
    #ifndef USE_PTHREAD
      err = thread_create(&th[i], thfunc, NULL);
    #else
      err = pthread_create(&th[i], NULL, thfunc, NULL);
    #endif
    assert(!err);
  }

  /* on leur passe la main, ils vont tous terminer */
  for(i=0; i<nb; i++) {
    #ifndef USE_PTHREAD
      thread_yield();
    #else
      pthread_yield();
    #endif
  }

  /* on les joine tous, maintenant qu'ils sont tous morts */
  for(i=0; i<nb; i++) {
    #ifndef USE_PTHREAD
      err = thread_join(th[i], NULL);
    #else
      err = pthread_join(th[i], NULL);
    #endif
    assert(!err);
  }

  gettimeofday(&tv2, NULL);
  us = (tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec);

  free(th);

  printf("%d threads créés et détruits tous d'un coup en %lu us\n", nb, us);
  return 0;
}

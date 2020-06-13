#ifndef USE_PTHREAD
  #include "../thread.h"
#else
  #define _GNU_SOURCE
  #include <pthread.h>
#endif
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* test de faire une somme avec plein de thread sur un compteur partagé
 *
 * valgrind doit etre content.
 * Les résultats doivent etre égals au nombre de threads * 1000.
 * La durée du programme doit etre proportionnelle au nombre de threads donnés en argument.
 *
 * support nécessaire:
 * - thread_create()
 * - thread_exit()
 * - thread_join() sans récupération de la valeur de retour
 * - thread_mutex_init()
 * - thread_mutex_destroy()
 * - thread_mutex_lock()
 * - thread_mutex_unloc()
 */

#define NB_MUTEX 10

int counter[NB_MUTEX] = { 0 };
#ifndef USE_PTHREAD
  thread_mutex_t lock[NB_MUTEX];
#else
  pthread_mutex_t lock[NB_MUTEX];
#endif

static void * thfunc(void *_nb)
{
    unsigned long nb = (unsigned long) _nb;
    unsigned long i = 0;
    int tmp;

    int m = nb % NB_MUTEX;

    for(i=0; i<1000;i++) {
	/* Verrouille la section critique accédant a counter */
  #ifndef USE_PTHREAD
  	thread_mutex_lock(&lock[m]);
  #else
    pthread_mutex_lock(&lock[m]);
  #endif
	tmp = counter[m];
  #ifndef USE_PTHREAD
  	thread_yield();
  #else
    pthread_yield();
  #endif
	tmp++;
  #ifndef USE_PTHREAD
  	thread_yield();
  #else
    pthread_yield();
  #endif
	counter[m] = tmp;
  #ifndef USE_PTHREAD
  	thread_mutex_unlock(&lock[m]);
  #else
    pthread_mutex_unlock(&lock[m]);
  #endif
    }

    return NULL;
}

int main(int argc, char *argv[])
{
  #ifndef USE_PTHREAD
    thread_t *th;
  #else
    pthread_t *th;
  #endif
  unsigned long i, nbthrd;
  int err, nb;

  if (argc < 2) {
    printf("argument manquant: nombre de threads\n");
    return -1;
  }

  nb = atoi(argv[1]);
  nbthrd = nb * NB_MUTEX;

  for(i=0; i<NB_MUTEX; i++) {
    #ifndef USE_PTHREAD
      if (thread_mutex_init(&lock[i]) != 0) {
      	  fprintf(stderr, "thread_mutex_init(lock[%lu]) failed\n", i);
      	  return -1;
      }
    #else
      if (pthread_mutex_init(&lock[i],NULL) != 0) {
          fprintf(stderr, "pthread_mutex_init(lock[%lu]) failed\n", i);
          return -1;
      }
    #endif
  }

  th = malloc(nbthrd*sizeof(*th));
  if (!th) {
    perror("malloc");
    return -1;
  }

  /* on cree tous les threads */
  for(i=0; i<nbthrd; i++) {
    #ifndef USE_PTHREAD
      err = thread_create(&th[i], thfunc, (void*)i);
    #else
      err = pthread_create(&th[i], NULL, thfunc, (void*)i);
    #endif
    assert(!err);
  }

  /* on leur passe la main, ils vont tous terminer */
  for(i=0; (int)i<nb; i++) {
    #ifndef USE_PTHREAD
      thread_yield();
    #else
      pthread_yield();
    #endif
  }

  /* on les joine tous, maintenant qu'ils sont tous morts */
  for(i=0; i<nbthrd; i++) {
    #ifndef USE_PTHREAD
      err = thread_join(th[i], NULL);
    #else
      err = pthread_join(th[i], NULL);
    #endif
    assert(!err);
  }

  free(th);
  for(i=0; i<NB_MUTEX; i++) {
      #ifndef USE_PTHREAD
        thread_mutex_destroy(&lock[i]);
      #else
        pthread_mutex_destroy(&lock[i]);
      #endif
  }

  for(i=0; i<NB_MUTEX; i++) {
      if ( counter[i] == ( nb * 1000 ) ) {
    	  printf("La somme %lu a été correctement calculée: %d * 1000 = %d\n", i, nb, counter[i]);
      }
      else {
    	  printf("Le résultat %lu est INCORRECT: %d * 1000 != %d\n", i, nb, counter[i]);
      }
  }
  return 0;
}

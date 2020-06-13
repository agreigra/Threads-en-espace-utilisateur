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
 * Le résultat doit etre égal au nombre de threads * 1000.
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

int counter = 0;
#ifndef USE_PTHREAD
  thread_mutex_t lock;
#else
  pthread_mutex_t lock;
#endif

static void * thfunc(void *dummy __attribute__((unused)))
{
    unsigned long i = 0;
    int tmp;

    for(i=0; i<1000;i++) {
    	/* Verrouille la section critique accédant a counter */
      #ifndef USE_PTHREAD
      	thread_mutex_lock(&lock);
      #else
        pthread_mutex_lock(&lock);
      #endif
    	tmp = counter;
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
    	counter = tmp;
      #ifndef USE_PTHREAD
      	thread_mutex_unlock(&lock);
      #else
        pthread_mutex_unlock(&lock);
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
  int err, i, nb;

  if (argc < 2) {
    printf("argument manquant: nombre de threads\n");
    return -1;
  }

  nb = atoi(argv[1]);

  #ifndef USE_PTHREAD
    if (thread_mutex_init(&lock) != 0) {
        fprintf(stderr, "thread_mutex_init failed\n");
        return -1;
    }
  #else
    if (pthread_mutex_init(&lock,NULL) != 0) {
        fprintf(stderr, "pthread_mutex_init failed\n");
        return -1;
    }
  #endif

  th = malloc(nb*sizeof(*th));
  if (!th) {
    perror("malloc");
    return -1;
  }

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

  free(th);
  #ifndef USE_PTHREAD
    thread_mutex_destroy(&lock);
  #else
    pthread_mutex_destroy(&lock);
  #endif

  if ( counter == ( nb * 1000 ) ) {
      printf("La somme a été correctement calculée: %d * 1000 = %d\n", nb, counter);
      return EXIT_SUCCESS;
  }
  else {
      printf("Le résultat est INCORRECT: %d * 1000 != %d\n", nb, counter);
      return EXIT_FAILURE;
  }
}

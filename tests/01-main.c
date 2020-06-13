#ifndef USE_PTHREAD
  #include "../thread.h"
#else
  #define _GNU_SOURCE
  #include <pthread.h>
#endif
#include <stdio.h>
#include <assert.h>


/* test du thread_self et yield du main seul.
 *
 * le programme doit retourner correctement.
 * valgrind doit être content.
 *
 * support nécessaire:
 * - thread_yield() depuis et vers le main
 * - thread_self() depuis le main
 */

int main()
{
  int err, i;

  for(i=0; i<10; i++) {
    printf("le main yield tout seul\n");
    #ifndef USE_PTHREAD
      err = thread_yield();
    #else
      err = pthread_yield();
    #endif
    assert(!err);
  }

  #ifndef USE_PTHREAD
    printf("le main est %p\n", (void*) thread_self());
  #else
    printf("le main est %p\n", (void*) pthread_self());
  #endif
  return 0;
}

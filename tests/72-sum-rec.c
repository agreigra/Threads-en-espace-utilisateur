#ifndef USE_PTHREAD
  #include "../thread.h"
#else
  #define _GNU_SOURCE
  #include <pthread.h>
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>

int SIZE;

// structure for void* parameter in sum_rec
struct param{
  int * tab;
  int size;
};

// prints the array tab
void show(int * tab){
  printf("[");
  int i;
  for(i = 0; i < SIZE-1; i++){
    printf("%d, ",tab[i]);
  }
  printf("%d]\n",tab[SIZE-1]);
}

// randomize the array tab
void randomize(int * tab){
  int i;
  for(i = 0; i < SIZE; i++){
    tab[i] = rand()%SIZE;
  }
}

// checks if the result is correct, compared with an iterative function
int check(int *tab, int res){
  int sum = 0;
  int i;
  for (i = 0; i < SIZE; i++) {
    sum += tab[i];
  }
  if(sum != res){
    return 0;
  }
  else{
    return 1;
  }
}

// computes the sum of the array elements
void * sum_rec(void * param){
  struct param *Param = (struct param *) param;
  int size = Param->size;
  int *tab = Param->tab;

  #ifndef USE_PTHREAD
    thread_t th1, th2;
  #else
    pthread_t th1, th2;
  #endif

  int err;
  void *res1 = NULL, *res2 = NULL;

  /* on passe un peu la main aux autres pour eviter de faire uniquement la partie gauche de l'arbre */
  #ifndef USE_PTHREAD
    thread_yield();
  #else
    pthread_yield();
  #endif

  if(size <= 1){
    intptr_t ret = (intptr_t) tab[0];
    return (void*) ret;
  }
  else{
    if(size%2 == 0){
      int * firstHalf = malloc(size/2 * sizeof(int));
      int * secondHalf = malloc(size/2 * sizeof(int));
      memcpy(firstHalf, tab, size/2 * sizeof(int));
      memcpy(secondHalf, tab + size/2, size/2 * sizeof(int));
      struct param *Param1 = malloc(sizeof(struct param));
      Param1->tab = firstHalf;
      Param1->size = size/2;
      struct param *Param2 = malloc(sizeof(struct param));
      Param2->tab = secondHalf;
      Param2->size = size/2;

      #ifndef USE_PTHREAD
        err = thread_create(&th1, sum_rec, (void *)Param1);
        assert(!err);
        err = thread_create(&th2, sum_rec, (void *)Param2);
        assert(!err);
      #else
        err = pthread_create(&th1, NULL, sum_rec, (void *)Param1);
        assert(!err);
        err = pthread_create(&th2, NULL, sum_rec, (void *)Param2);
        assert(!err);
      #endif

      #ifndef USE_PTHREAD
        err = thread_join(th1, &res1);
        assert(!err);
        err = thread_join(th2, &res2);
        assert(!err);
      #else
        err = pthread_join(th1, &res1);
        assert(!err);
        err = pthread_join(th2, &res2);
        assert(!err);
      #endif

      void * res = (void *) ((intptr_t) res1 + (intptr_t) res2);
      free(Param1->tab);
      free(Param2->tab);
      free(Param1);
      free(Param2);
      return(res);
    }
    else{
      int * firstHalf = malloc(size/2 * sizeof(int));
      int * secondHalf = malloc((size/2 + 1) * sizeof(int));
      memcpy(firstHalf, tab, size/2 * sizeof(int));
      memcpy(secondHalf, tab + size/2, (size/2 + 1) * sizeof(int));
      struct param *Param1 = malloc(sizeof(struct param));
      Param1->tab = firstHalf;
      Param1->size = size/2;
      struct param *Param2 = malloc(sizeof(struct param));
      Param2->tab = secondHalf;
      Param2->size = size/2 + 1;

      #ifndef USE_PTHREAD
        err = thread_create(&th1, sum_rec, (void *)Param1);
        assert(!err);
        err = thread_create(&th2, sum_rec, (void *)Param2);
        assert(!err);
      #else
        err = pthread_create(&th1, NULL, sum_rec, (void *)Param1);
        assert(!err);
        err = pthread_create(&th2, NULL, sum_rec, (void *)Param2);
        assert(!err);
      #endif

      #ifndef USE_PTHREAD
        err = thread_join(th1, &res1);
        assert(!err);
        err = thread_join(th2, &res2);
        assert(!err);
      #else
        err = pthread_join(th1, &res1);
        assert(!err);
        err = pthread_join(th2, &res2);
        assert(!err);
      #endif

      void * res = (void *) ((intptr_t) res1 + (intptr_t) res2);

      free(Param1->tab);
      free(Param2->tab);
      free(Param1);
      free(Param2);
      return(res);
    }
  }
}

int main(int argc, char ** argv) {
  if (argc < 2){
    printf("La taille du tableau n'est pas précisée, elle est fixée par défaut à 100.\n");
    SIZE = 100;
  }
  else{
    SIZE = atoi(argv[1]);
  }

  struct timeval tv1, tv2;
  double s;

  int * tab = malloc(SIZE * sizeof(int));
  srand(time(NULL));
  randomize(tab);
  // show(tab);

  struct param *Param = malloc(sizeof(struct param));
  Param->tab = tab;
  Param->size = SIZE;
  gettimeofday(&tv1, NULL);
  void* res = sum_rec(Param);
  gettimeofday(&tv2, NULL);
  s = 1000000*(tv2.tv_sec-tv1.tv_sec) + (tv2.tv_usec-tv1.tv_usec);

  if(check(Param->tab, (intptr_t) res)){
    printf("La somme de %d calculée en %f us vaut %ld\n",SIZE ,s, (intptr_t) res);
  }
  else{
    printf("Erreur lors du calcul de la somme : %ld\n", (intptr_t) res);
  }

  free(Param->tab);
  free(Param);

  return 0;
}

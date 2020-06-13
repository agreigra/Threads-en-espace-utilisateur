#define _GNU_SOURCE

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#ifndef USE_PTHREAD
#include "../thread.h"
#else
#include <pthread.h>
#endif

int SIZE;

// structure for void * parameter in quicksort
struct param{
  int *tab;
  int size;
};

// checks if the array is correctly ordered
void check(int *tab){
  int prev = tab[0];
  int i;
  for (i = 1; i < SIZE; i++) {
    if(prev > tab[i]){
      printf("error : %i > %i at index %d\n", prev, tab[i], i);
      return;
    }
    prev = tab[i];
  }
  printf("tabular ordered correctly !\n");
}

// prints the array
void show(int *tab){
  printf("[");
  int i;
  for(i = 0; i < SIZE-1; i++){
    printf("%d, ",tab[i]);
  }
  printf("%d]\n",tab[SIZE-1]);
}

// fills the array with random values (from 0 to size)
void randomize(int *tab){
  int i;
  for(i = 0; i < SIZE; i++){
    tab[i] = rand()%SIZE;
  }
}

// swaps the 2 values at num1 and num2 in the array in struct param
void swap(struct param *Param, int num1, int num2) {
  int temp = Param->tab[num1];
  Param->tab[num1] = Param->tab[num2];
  Param->tab[num2] = temp;
}

// finds the values to swap, and swaps them
int partition(struct param * Param, int left, int right, int pivot) {
  int leftPointer = left-1;
  int rightPointer = right;

  while(1) {
    while(Param->tab[++leftPointer] < pivot) {
      //do nothing
    }

    while(rightPointer > 0 && Param->tab[--rightPointer] > pivot) {
      //do nothing
    }

    if(leftPointer >= rightPointer) {
      break;
    }
    else {
      swap(Param, leftPointer,rightPointer);
    }
  }
  swap(Param, leftPointer,right-1);
  return leftPointer;
}

// inspired from : https://hackr.io/blog/quick-sort-in-c
// quicksort algorithm
void * quickSort(void * param) {
  struct param *Param = (struct param *) param;
  int size = Param->size;

  #ifndef USE_PTHREAD
  thread_t th1, th2;
  #else
  pthread_t th1, th2;
  #endif
  int err;

  /* on passe un peu la main aux autres pour eviter de faire uniquement la partie gauche de l'arbre */
  #ifndef USE_PTHREAD
  thread_yield();
  #else
  pthread_yield();
  #endif

  if(size <= 1) {
    return NULL;
  }
  else {
    int pivot = Param->tab[size-1];
    int partitionPoint = partition(Param, 0, size, pivot);

    int *tab_left = malloc(partitionPoint*sizeof(int));
    memcpy(tab_left, Param->tab, partitionPoint*sizeof(int));
    struct param *Param_left = malloc(sizeof(struct param));
    Param_left->tab = tab_left;
    Param_left->size = partitionPoint;

    #ifndef USE_PTHREAD
    err = thread_create(&th1, quickSort, (void *) Param_left);
    #else
    err = pthread_create(&th1, NULL, quickSort, (void *) Param_left);
    #endif
    assert(!err);

    int *tab_right = malloc((size - partitionPoint)*sizeof(int));
    memcpy(tab_right, Param->tab+partitionPoint, (size - partitionPoint)*sizeof(int));
    struct param *Param_right = malloc(sizeof(struct param));
    Param_right->tab = tab_right;
    Param_right->size = (size - partitionPoint);

    #ifndef USE_PTHREAD
    err = thread_create(&th2, quickSort, (void *) Param_right);
    #else
    err = pthread_create(&th2, NULL, quickSort, (void *) Param_right);
    #endif
    assert(!err);

    #ifndef USE_PTHREAD
    err = thread_join(th1,NULL);
    #else
    err = pthread_join(th1,NULL);
    #endif
    assert(!err);
    #ifndef USE_PTHREAD
    err = thread_join(th2,NULL);
    #else
    err = pthread_join(th2,NULL);
    #endif
    assert(!err);

    memcpy(Param->tab, Param_left->tab, partitionPoint*sizeof(int));
    memcpy(Param->tab+partitionPoint, Param_right->tab, (size-partitionPoint)*sizeof(int));

    free(Param_left->tab); free(Param_left);
    free(Param_right->tab); free(Param_right);

    return NULL;
  }
}

int main(int argc, char ** argv){
  if (argc < 2){
    printf("La taille du tableau n'est pas précisée, elle est fixée par défaut à 100.\n");
    SIZE = 100;
  }
  else{
    SIZE = atoi(argv[1]);
  }

  srand(time(NULL));
  struct timeval tv1, tv2;
  double s;

  int *tab = (int*) malloc(SIZE * sizeof(int));
  struct param *Param = (struct param *)malloc(sizeof(struct param));
  Param->tab = tab;

  randomize(Param->tab);
  show(Param->tab);
  Param->size = SIZE;

  gettimeofday(&tv1, NULL);
  quickSort((void *) Param);
  gettimeofday(&tv2, NULL);

  s = 1000000*(tv2.tv_sec-tv1.tv_sec) + (tv2.tv_usec-tv1.tv_usec);

  show(Param->tab);
  check(Param->tab);
  printf("Tableau de taille %d trié en %f us.\n", SIZE, s);

  free(Param->tab);
  free(Param);

  return EXIT_SUCCESS;
}

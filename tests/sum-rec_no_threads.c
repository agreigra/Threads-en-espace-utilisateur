#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZE 100

// structure for void* parameter in sum_rec
struct param{
  int * tab;
  int size;
};

// prints the array tab
void show(int * tab){
  printf("[");
  for(int i = 0; i < SIZE-1; i++){
    printf("%d, ",tab[i]);
  }
  printf("%d]\n",tab[SIZE-1]);
}

// randomize the array tab
void randomize(int * tab){
  for(int i = 0; i < SIZE; i++){
    tab[i] = rand()%SIZE;
  }
}

// checks if the result is correct, compared with an iterative function
void check(int *tab, int res){
  int sum = 0;
  for (size_t i = 0; i < SIZE; i++) {
    sum += tab[i];
  }
  if(sum != res){
    printf("error : result is %d, should be %d\n",res,sum);
  }
  else{
    printf("result is correct !\n");
  }
}

// computes the sum of the array elements
void * sum_rec(void * param){
  struct param *Param = (struct param *) param;
  int size = Param->size;
  int *tab = Param->tab;
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
      intptr_t res = (intptr_t) sum_rec(Param1) + (intptr_t) sum_rec(Param2);
      free(Param1->tab);
      free(Param2->tab);
      free(Param1);
      free(Param2);
      return((void*) res);
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
      intptr_t res = (intptr_t) sum_rec(Param1) + (intptr_t) sum_rec(Param2);
      free(Param1->tab);
      free(Param2->tab);
      free(Param1);
      free(Param2);
      return((void*) res);
    }
  }
}

int main(int argc, char const *argv[]) {
  int * tab = malloc(SIZE * sizeof(int));
  srand(time(NULL));
  randomize(tab);
  show(tab);
  struct param *Param = malloc(sizeof(struct param));
  Param->tab = tab;
  Param->size = SIZE;
  void* res = sum_rec(Param);
  check(Param->tab, (intptr_t) res);
  free(Param->tab);
  free(Param);
  return 0;
}

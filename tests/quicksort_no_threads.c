#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 100

// structure for void * parameter in quicksort
struct param{
  int tab[SIZE];
  int left;
  int right;
};

// checks if the array is correctly ordered
void check(int *tab){
  int prev = tab[0];
  for (size_t i = 1; i < SIZE; i++) {
    if(prev > tab[i]){
      printf("error : %i > %i at index %lu\n", prev, tab[i], i);
      return;
    }
  }
  printf("tabular ordered correctly !\n");
}

// prints the array
void show(int *tab){
    printf("[");
    for(int i = 0; i < SIZE-1; i++){
        printf("%d, ",tab[i]);
    }
    printf("%d]\n",tab[SIZE-1]);
}

// fills the array with random values (from 0 to size)
void randomize(struct param *Param){
    for(int i = 0; i < SIZE; i++){
        Param->tab[i] = rand()%SIZE;
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
   int leftPointer = left -1;
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
   swap(Param, leftPointer,right);
   return leftPointer;
}

// inspired from : https://hackr.io/blog/quick-sort-in-c
// quicksort algorithm
static void * quickSort(void * param) {
  struct param *Param = (struct param *) param;
  int left = Param->left;
  int right = Param->right;
  int err;

  if(right-left <= 0) {
    return NULL;
  }
  else {
    int pivot = Param->tab[right];
    int partitionPoint = partition(Param, left, right, pivot);
    Param->left = left;
    Param->right = partitionPoint-1;
    quickSort(Param);

    Param->left = partitionPoint+1;
    Param->right = right;
    quickSort(Param);
    return NULL;
  }
}

int main(int argc, char *  argv[]){
    srand(time(NULL));
    struct param *Param = (struct param *)malloc(sizeof(struct param));
    randomize(Param);
    Param->left = 0;
    Param->right = SIZE-1;
    printf("randomized : \n");
    show(Param->tab);
    quickSort((void *) Param);
    printf("sorted : \n");
    show(Param->tab);
    check(Param->tab);
    free(Param);
    Param = NULL;
    return EXIT_SUCCESS;
}

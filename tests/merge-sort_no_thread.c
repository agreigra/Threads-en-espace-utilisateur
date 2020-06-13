#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define SIZE 100

// structure for void * parameter of merge_sort
struct param{
  int *tab;
  int *tmp;
  int size;
};

// prints the array tab
void show(int *tab,int size){
  printf("[");
  for(int i = 0; i < size-1; i++){
    printf("%d, ",tab[i]);
  }
  printf("%d]\n",tab[size-1]);
}

// checks if the array is correctly sorted
void check(int *tab){
  int prev = tab[0];
  for (size_t i = 1; i < SIZE; i++) {
    if(prev > tab[i]){
      printf("error : %i > %i at index %lu\n", prev, tab[i], i);
      return;
    }
    prev = tab[i];
  }
  printf("tabular ordered correctly !\n");
}

//fills the tabular with random values (from 0 to SIZE)
void randomize(int* tab){
  for(int i = 0; i < SIZE; i++){
    tab[i] = rand()%SIZE;
  }
}

// inspired from https://waytolearnx.com/2019/08/tri-par-fusion-en-c.html
// merge sort algorithm
void* merge_sort(void *param) {
  struct param *Param = (struct param *) param;
  int size = Param->size;

  if(size <= 1){
    return NULL;
  }

  int m = size/2;

  int *tab_left = malloc(m * sizeof(int));
  memcpy(tab_left, Param->tab, m * sizeof(int));
  int *tmp_left = malloc(m * sizeof(int));
  memcpy(tmp_left, Param->tmp, m * sizeof(int)); //To remove for optimisations ?
  struct param *Param_left = malloc(sizeof(struct param));
  Param_left->tab = tab_left; Param_left->tmp = tmp_left;
  Param_left->size = m;

  merge_sort(Param_left);

  memcpy(Param->tab, Param_left->tab, m*sizeof(int));
  memcpy(Param->tmp, Param_left->tmp, m*sizeof(int));

  struct param *Param_right = malloc(sizeof(struct param));
  if(size%2 == 0){
    int *tab_right = malloc(m * sizeof(int));
    memcpy(tab_right, Param->tab + m, m*sizeof(int));
    int *tmp_right = malloc(m * sizeof(int));
    memcpy(tmp_right, Param->tmp + m, m*sizeof(int)); //To remove for optimisations ?
    Param_right->tab = tab_right; Param_right->tmp = tmp_right;
    Param_right->size = m;
    merge_sort(Param_right);

    memcpy(Param->tab + m, Param_right->tab, m*sizeof(int));
    memcpy(Param->tmp + m, Param_right->tmp, m*sizeof(int));
  }
  else{
    int *tab_right = malloc((m+1) * sizeof(int));
    memcpy(tab_right, Param->tab + m, (m+1)*sizeof(int));
    int *tmp_right = malloc((m+1) * sizeof(int));
    memcpy(tmp_right, Param->tmp + m, (m+1)*sizeof(int)); //To remove for optimisations ?
    Param_right->tab = tab_right; Param_right->tmp = tmp_right;
    Param_right->size = m+1;
    merge_sort(Param_right);

    memcpy(Param->tab + m, Param_right->tab, (m+1)*sizeof(int));
    memcpy(Param->tmp + m, Param_right->tmp, (m+1)*sizeof(int));
  }

  free(Param_left->tab); free(Param_left->tmp);
  free(Param_left);
  free(Param_right->tab); free(Param_right->tmp);
  free(Param_right);

  int lpart = 0;
  int rpart = m;

  for(int i = 0; i < size; i++) {
    if(lpart == m) {
      Param->tmp[i] = Param->tab[rpart];
      rpart++;
    }else if (rpart == size) {
      Param->tmp[i] = Param->tab[lpart];
      lpart++;
    }else if (Param->tab[lpart] < Param->tab[rpart]) {
      Param->tmp[i] = Param->tab[lpart];
      lpart++;
    }else {
      Param->tmp[i] = Param->tab[rpart];
      rpart++;
    }
  }
  for(int i = 0; i < size; i++) {
    Param->tab[i] = Param->tmp[i];
  }
}

int main() {
  srand(time(NULL));
  int* tab = malloc(SIZE*sizeof(int));
  int* tmp = malloc(SIZE*sizeof(int));
  struct param *Param = (struct param *) malloc(sizeof(struct param));
  Param->tab = tab;
  Param->tmp = tmp;
  Param->size = SIZE;
  randomize(Param->tab);
  show(Param->tab,SIZE);
  merge_sort(Param);
  show(Param->tab,SIZE);
  check(Param->tab);
  free(Param->tab); free(Param->tmp);
  free(Param);
  return 0;
}

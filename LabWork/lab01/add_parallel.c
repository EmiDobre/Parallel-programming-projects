#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/*
    schelet pentru exercitiul 5
*/

int *arr;
int array_size;
int num_threads;

void *func(void *arg) {
  long id = *(long *)arg;
  //start si end
  int n = array_size;
  int start = id * (double)(n/num_threads);
  int min = (id + 1)* (double)(n/num_threads);
  if ( min > n ) 
    min = n;
  int end = min;
    for (int i = start; i < end; i++) {
    arr[i] += 100;
  }
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Specificati dimensiunea array-ului si numarul de thread-uri\n");
    exit(-1);
  }

  array_size = atoi(argv[1]);
  num_threads = atoi(argv[2]);

  arr = malloc(array_size * sizeof(int));
  for (int i = 0; i < array_size; i++) {
    arr[i] = i;
  }
  
  printf("vector: ");
  for (int i = 0; i < array_size; i++) {
    printf("%d", arr[i]);
    if (i != array_size - 1) {
      printf(" ");
    } else {
      printf("\n");
    }
  }

  // TODO: aceasta operatie va fi paralelizata cu num_threads fire de executie
  //impart operatia de adunare la un nr de threaduri
  //fiecare thread are un id de la 1 l a p -1 
  int nr_threads = 4; 
  pthread_t threads[nr_threads];
  int r;
  long id;
  void *status;
  long ids[nr_threads];

//pt threadul numarul id lucrez cu portiunea vector[start],
//vector[end]

  for ( id = 0; id < nr_threads; id++ ) {
    ids[id] = id; // vector id uri threaduri
    r = pthread_create(&threads[id], NULL, func, &ids[id]);
    if (r) {
      printf("Eroare la crearea thread-ului %ld\n", id);
      exit(-1);
    }
  }

  for (id = 0; id < nr_threads; id++) {
    r = pthread_join(threads[id], &status);

    if (r) {
      printf("Eroare la asteptarea thread-ului %ld\n", id);
      exit(-1);
    }
  }


  for (int i = 0; i < array_size; i++) {
    printf("%d", arr[i]);
    if (i != array_size - 1) {
      printf(" ");
    } else {
      printf("\n");
    }
  }

  return 0;
}

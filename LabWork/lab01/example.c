#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define NUM_THREADS 2



void *f(void *arg) {
  long id = *(long *)arg;
  printf("Hello World din thread-ul %ld!\n", id);
  pthread_exit(NULL);
}


//3) Modificare functiee f:
// void *f(void *arg) {
//   long id = *(long *)arg;
//   for ( int i = 0; i < 100; i++ )
//     printf("Hello World din thread-ul %ld! cu nr iteratie: %d \n", id, i + 1);
//   pthread_exit(NULL);
// }

//4) fiecare thread cu functia lui:
void *f2(void *arg) {
  long id = *(long *)arg;
  printf("Functia2 din thread-ul %ld! \n", id);
  pthread_exit(NULL);
}

void *f3(void *arg) {
  long id = *(long *)arg;
  printf("Functia3 din thread-ul %ld! \n", id);
  pthread_exit(NULL);
}


void *func(void *arg) {
  long id = *(long *)arg;
  if ( id == 0 )
    f2(arg);
  else
    f3(arg);
}



int main(int argc, char *argv[]) {

  //Pentru a putea obține numărul de core-uri de 
  //pe un calculator, se poate folosi funcția sysconf astfel:
  long cores = sysconf(_SC_NPROCESSORS_CONF);
  int nr_threads ;
  nr_threads =  NUM_THREADS;

  //1) thredurile = nr core uri
  //=> o sa s afizeze random cele 15 threaduiri
  //nr_threads = cores;

  pthread_t threads[nr_threads];
  int r;
  long id;
  void *status;
  long ids[nr_threads];

//creez 2 threaduri care ambele ruleaza functia f 
  for (id = 0; id < nr_threads; id++) {
    ids[id] = id;
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

  return 0;
}

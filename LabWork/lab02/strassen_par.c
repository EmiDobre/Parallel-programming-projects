#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "pthread_barrier_mac.h"

int N;
int **a;
int **b;
int **c;
int **M1;
int **AUXM11;
int **AUXM12;
int **M2;
int **AUXM21;
int **M3;
int **AUXM31;
int **M4;
int **AUXM41;
int **M5;
int **AUXM51;
int **M6;
int **AUXM61;
int **AUXM62;
int **M7;
int **AUXM71;
int **AUXM72;
pthread_barrier_t barrier;

void get_args(int argc, char **argv)
{
	if(argc < 2) {
		printf("Numar insuficient de parametri: ./program N\n");
		exit(1);
	}

	N = atoi(argv[1]);
}

int **alloc_matrix(int n, int m)
{
	int **matrix;
	matrix = malloc(sizeof(int *) * n);
	if(matrix == NULL) {
		printf("Eroare la malloc");
		exit(1);
	}

	int i;
	for (i = 0; i < N; i++) {
		matrix[i] = malloc(sizeof(int) * N);
		if (matrix[i] == NULL) {
			printf("Eroare la malloc");
			exit(1);
		}
	}

	return(matrix);
}

void init()
{
	int i, j;
	a = alloc_matrix(N, N);
	b = alloc_matrix(N, N);
	c = alloc_matrix(N, N);

	M1 = alloc_matrix(N, N);
	AUXM11 = alloc_matrix(N, N);
	AUXM12 = alloc_matrix(N, N);

	M2 = alloc_matrix(N, N);
	AUXM21 = alloc_matrix(N, N);

	M3 = alloc_matrix(N, N);
	AUXM31 = alloc_matrix(N, N);

	M4 = alloc_matrix(N, N);
	AUXM41 = alloc_matrix(N, N);

	M5 = alloc_matrix(N, N);
	AUXM51 = alloc_matrix(N, N);

	M6 = alloc_matrix(N, N);
	AUXM61 = alloc_matrix(N, N);
	AUXM62 = alloc_matrix(N, N);

	M7 = alloc_matrix(N, N);
	AUXM71 = alloc_matrix(N, N);
	AUXM72 = alloc_matrix(N, N);

	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			c[i][j] = 0;
			
			M1[i][j] = 0;
			AUXM11[i][j] = 0;
			AUXM12[i][j] = 0;
			
			M2[i][j] = 0;
			AUXM21[i][j] = 0;
			
			M3[i][j] = 0;
			AUXM31[i][j] = 0;
			
			M4[i][j] = 0;
			AUXM41[i][j] = 0;
			
			M5[i][j] = 0;
			AUXM51[i][j] = 0;
			
			M6[i][j] = 0;
			AUXM61[i][j] = 0;
			AUXM62[i][j] = 0;
			
			M7[i][j] = 0;
			AUXM71[i][j] = 0;
			AUXM72[i][j] = 0;
			
			if (i <= j) {
				a[i][j] = 1;
				b[i][j] = 1;
			} else {
				a[i][j] = 0;
				b[i][j] = 0;
			}
		}
	}
}

void print(int **mat)
{
	int i, j;
	for (i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			printf("%i\t", mat[i][j]);
		}
		printf("\n");
	}
}

void mul_matrix(int **C,  int startCi, int startCj, int **A, int startAi, int startAj, int **B, int startBi, int startBj) 
{
	// NU paralelizati aceasta functie
	int i, j, k;
	for (i = 0; i < N/2; i++)
		for (j = 0; j < N/2; j++)
			for (k = 0; k < N/2; k++)
				C[startCi + i][startCj + j] += A[startAi + i][startAj + k] * B[startBi + k][startBj + j];
}

void add_matrix(int **C, int startCi, int startCj, int **A, int startAi, int startAj, int **B, int startBi, int startBj) 
{
	// NU paralelizati aceasta functie
	int i, j;
	for( i = 0; i < N/2; i++)
		for (j = 0; j < N/2; j++)
			C[startCi + i][startCj + j] = A[startAi + i][startAj + j] + B[startBi + i][startBj + j];
}

void sub_matrix(int **C, int startCi, int startCj,  int **A, int startAi, int startAj, int **B, int startBi, int startBj) 
{
	// NU paralelizati aceasta functie
	int i, j;
	for (i = 0; i < N/2; i++)
		for (j = 0; j < N/2; j++)
			C[startCi + i][startCj + j] = A[startAi + i][startAj + j] - B[startBi + i][startBj + j];
}

void *f1(void *args) {
    add_matrix(AUXM11, 0, 0, a, 0, 0, a, N/2, N/2);
	add_matrix(AUXM12, 0, 0, b, 0, 0, b, N/2, N/2);
	mul_matrix(M1, 0, 0, AUXM11, 0, 0, AUXM12, 0, 0);

    return NULL;
}

void *f2(void *args) {
    add_matrix(AUXM21, 0,0, a, N/2, 0, a, N/2, N/2);
	mul_matrix(M2, 0, 0, AUXM21, 0, 0, b, 0, 0);    

    return NULL;
}

void *f3(void *args) {
    sub_matrix(AUXM31, 0, 0, b, 0, N/2, b, N/2, N/2);
	mul_matrix(M3, 0, 0, a, 0, 0, AUXM31, 0, 0);

    return NULL;
}

void *f4(void *args) {
    sub_matrix(AUXM41, 0, 0, b, N/2, 0, b, 0, 0);
	mul_matrix(M4, 0, 0, a, N/2, N/2, AUXM41, 0, 0);

    return NULL;
}

void *f5(void *args) {
    add_matrix(AUXM51, 0,0, a, 0, 0, a, 0, N/2);
	mul_matrix(M5, 0, 0, AUXM51, 0, 0, b, N/2, N/2);

    return NULL;
}

void *f6(void *args) {
    sub_matrix(AUXM61, 0, 0, a, N/2, 0, a, 0, 0);
	add_matrix(AUXM62, 0, 0, b, 0, 0, b, 0, N/2);
	mul_matrix(M6, 0, 0, AUXM61, 0, 0, AUXM62, 0, 0);

    return NULL;
}

void *f7(void *args) {
	sub_matrix(AUXM71, 0, 0, a, 0, N/2, a, N/2, N/2);
	add_matrix(AUXM72, 0, 0, b, N/2, 0, b, N/2, N/2);
	mul_matrix(M7, 0, 0, AUXM71, 0, 0, AUXM72, 0, 0);

    return NULL;
}

void *allThreadsInOneFunction(void *args) {
	int thread_id = *(int *)args;

	switch(thread_id) {
		case 0:
			add_matrix(AUXM11, 0, 0, a, 0, 0, a, N/2, N/2);
			add_matrix(AUXM12, 0, 0, b, 0, 0, b, N/2, N/2);
			mul_matrix(M1, 0, 0, AUXM11, 0, 0, AUXM12, 0, 0);
			break;
		case 1:
			add_matrix(AUXM21, 0,0, a, N/2, 0, a, N/2, N/2);
			mul_matrix(M2, 0, 0, AUXM21, 0, 0, b, 0, 0);    
			break;
		case 2:
			sub_matrix(AUXM31, 0, 0, b, 0, N/2, b, N/2, N/2);
			mul_matrix(M3, 0, 0, a, 0, 0, AUXM31, 0, 0);
			break;
		case 3:
			sub_matrix(AUXM41, 0, 0, b, N/2, 0, b, 0, 0);
			mul_matrix(M4, 0, 0, a, N/2, N/2, AUXM41, 0, 0);
			break;
		case 4:
		    add_matrix(AUXM51, 0,0, a, 0, 0, a, 0, N/2);
			mul_matrix(M5, 0, 0, AUXM51, 0, 0, b, N/2, N/2);
			break;
		case 5:
		    sub_matrix(AUXM61, 0, 0, a, N/2, 0, a, 0, 0);
			add_matrix(AUXM62, 0, 0, b, 0, 0, b, 0, N/2);
			mul_matrix(M6, 0, 0, AUXM61, 0, 0, AUXM62, 0, 0);
			break;
		case 6:
			sub_matrix(AUXM71, 0, 0, a, 0, N/2, a, N/2, N/2);
			add_matrix(AUXM72, 0, 0, b, N/2, 0, b, N/2, N/2);
			mul_matrix(M7, 0, 0, AUXM71, 0, 0, AUXM72, 0, 0);
			break;
		default:
			printf("No action for thread: %d!\n", thread_id);
			break;
	}

	//We can use a barrier here, to wait for all threads to finish their task from above.
	// After we finished waiting for all threads, the barrier will unlock, and all the threads
	// can continue their execution
	pthread_barrier_wait(&barrier);

	//Now we have all we need to calculate the C1,1 C1,2 and C2,1 C2,2 matrix, but we have to do it with one thread.
	// we can force the thread with id == 0 to execute this, and then print the result. All the other 
	// threads will ignore this if-block, because their id don't match. There is only one thread with id = 0
	// that will execute this!
	if (0 == thread_id) {
		// calculul submatricii C1,1
		add_matrix(c, 0, 0, M1, 0, 0, M4, 0, 0);
		sub_matrix(c, 0, 0, c, 0, 0, M5, 0, 0);
		add_matrix(c, 0, 0, c, 0, 0, M7, 0, 0);

		// calculul submatricii C1,2
		add_matrix(c, 0, N/2, M3, 0, 0, M5, 0, 0);

		// calculul submatricii C2,1
		add_matrix(c, N/2, 0, M2, 0, 0, M4, 0, 0);

		// calculul submatricii C2,2
		sub_matrix(c, N/2, N/2, M1, 0, 0, M2, 0, 0);
		add_matrix(c, N/2, N/2, c, N/2, N/2, M3, 0, 0);
		add_matrix(c, N/2, N/2, c, N/2, N/2, M6, 0, 0);

		print(c);
	}

	return NULL;
}

int main(int argc, char *argv[])
{
	get_args(argc, argv);
	init();

    pthread_t threads[7];
    int ids[7];
	
	//VERSION 1 
	// Each thread has a different function to call.. The result is further computed in main function
	// after the threads complete their task and pthread_join is executed for all threads!!!
	// {
	// 	for (int i = 0; i < 7; i++) {
    // 	    ids[0] = i;
    // 	}
	// 	// calculul matricii M1
	// 	pthread_create(&threads[0], NULL, f1, &ids[0]);

	// 	// calculul matricii M2
	// 	pthread_create(&threads[1], NULL, f2, &ids[1]);

	// 	// calculul matricii M3
	// 	pthread_create(&threads[2], NULL, f3, &ids[2]);

	// 	// calculul matricii M4
	// 	pthread_create(&threads[3], NULL, f4, &ids[3]);

	// 	// calculul matricii M5
	// 	pthread_create(&threads[4], NULL, f5, &ids[4]);

	// 	// calculul matricii M6
	// 	pthread_create(&threads[5], NULL, f6, &ids[5]);

	// 	// calculul matricii M7
	// 	pthread_create(&threads[6], NULL, f7, &ids[6]);

	// 	for (int i = 0; i < 7; i++) {
	// 		int r = pthread_join(threads[i], NULL);

	// 		if (r) {
	// 			printf("Eroare la asteptarea thread-ului %d.", i);
	// 			exit(1);
	// 		}
	// 	}

	// 	// calculul submatricii C1,1
	// 	add_matrix(c, 0, 0, M1, 0, 0, M4, 0, 0);
	// 	sub_matrix(c, 0, 0, c, 0, 0, M5, 0, 0);
	// 	add_matrix(c, 0, 0, c, 0, 0, M7, 0, 0);

	// 	// calculul submatricii C1,2
	// 	add_matrix(c, 0, N/2, M3, 0, 0, M5, 0, 0);

	// 	// calculul submatricii C2,1
	// 	add_matrix(c, N/2, 0, M2, 0, 0, M4, 0, 0);

	// 	// calculul submatricii C2,2
	// 	sub_matrix(c, N/2, N/2, M1, 0, 0, M2, 0, 0);
	// 	add_matrix(c, N/2, N/2, c, N/2, N/2, M3, 0, 0);
	// 	add_matrix(c, N/2, N/2, c, N/2, N/2, M6, 0, 0);

	// 	print(c);
	// }

	//VERSION 2
	// All threads are called with the same function, and all the logic is done in the function which
	// is executed by threads.
	{
		pthread_barrier_init(&barrier, NULL, 7); //we init the barrier, with the count set to 7(we have 7 threads);
		for (int i = 0; i < 7; i++) {
			ids[i] = i; //we set the id for each thread in the id vector

			pthread_create(&threads[i], NULL, allThreadsInOneFunction, &ids[i]);
		}

		for (int i = 0; i < 7; i++) {
			int r = pthread_join(threads[i], NULL); //wait for each thread to finish before executing something in main!! very important!!

			if (r) {
				printf("Error occured while waiting for thread %d.\n", i);
				exit(1);
			}
		}

		pthread_barrier_destroy(&barrier); //don't forget to destroy the barrier!!
	}

	return 0;
}

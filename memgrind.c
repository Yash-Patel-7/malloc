#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mymalloc.h"

// enumeration for memory size and maximum allocation size in bytes
// where MEMSIZE > MAXSIZE
enum {
	MEMSIZE = 4104,
	MAXSIZE = 511
};

// prototype for memgrind
void memgrind();

// Your program should run each task 50 times, recording the amount of time needed for each iteration,
// and then reporting the average time needed for each iteration. You may use gettimeofday() to obtain timing information.
int main() {
	// call memgrind function
	memgrind();
	
	// return successful exit status
	return EXIT_SUCCESS;
}

// 1. malloc() and immediately free() a 1-byte chunk, 120 times.
double task1() {
	struct timeval start, end;
	double time = 0;
	int i;
	for (i = 0; i < 50; i++) {
		gettimeofday(&start, NULL);
		int j;
		for (j = 0; j < 120; j++) {
			char *ptr = malloc(1);
			if (ptr != NULL) {
				free(ptr);
			}
		}
		gettimeofday(&end, NULL);
		time += (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
	}
	return time / 50;
}

// 2. Use malloc() to get 120 1-byte chunks, storing the pointers in an array, then use free() to deallocate the chunks
double task2() {
	struct timeval start, end;
	double time = 0;
	int i;
	for (i = 0; i < 50; i++) {
		gettimeofday(&start, NULL);
		char *p[120];
		int j;
		for (j = 0; j < 120; j++) {
			p[j] = malloc(1);
		}
		for (j = 0; j < 120; j++) {
			if (p[j] != NULL) {
				free(p[j]);
			}
		}
		gettimeofday(&end, NULL);
		time += (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
	}
	return time / 50;
}

// 3. Randomly choose between 
// a) Allocating a 1-byte chunk and storing the pointer in an array
// b) Deallocating one of the chunks in the array (if any)
// Repeat above until you have called malloc() 120 times, regardless of the number of iterations
double task3() {
	struct timeval start, end;
	double time = 0;
	int i;
	for (i = 0; i < 50; i++) {
		gettimeofday(&start, NULL);
		char *p[120];
		int malloc_count = 0;
		int index = 0;
		while (malloc_count < 120) {
			int r = rand() % 2;
			if (r == 0) {
				p[index] = malloc(1);
				malloc_count++;
				index++;
			} else {
				if (index > 0) {
					if (p[index - 1] != NULL) {
						free(p[index - 1]);
					}
					index--;
				}
			}
		}
		while (index > 0) {
			if (p[index - 1] != NULL) {
				free(p[index - 1]);
			}
			index--;
		}
		gettimeofday(&end, NULL);
		time += (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
	}
	return time / 50;
}

// 4. Repeat number 1 but with a variable number of bytes to allocate called size, including from 0 to MAXSIZE bytes
// if malloc returns NULL, then don't free it
double task4() {
	struct timeval start, end;
	double time = 0;
	int i;
	for (i = 0; i < 50; i++) {
		gettimeofday(&start, NULL);
		int j;
		for (j = 0; j < 120; j++) {
			char *p = malloc(rand() % (MAXSIZE + 1));
			if (p != NULL) {
				free(p);
			}
		}
		gettimeofday(&end, NULL);
		time += (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
	}
	return time / 50;
}

// 5. Repeat number 2 but with a variable number of bytes to allocate called size, including from 0 to MAXSIZE bytes
// if malloc returns NULL, then don't free it
double task5() {
	struct timeval start, end;
	double time = 0;
	int i;
	for (i = 0; i < 50; i++) {
		gettimeofday(&start, NULL);
		char *p[120];
		int j;
		for (j = 0; j < 120; j++) {
			p[j] = malloc(rand() % (MAXSIZE + 1));
		}
		for (j = 0; j < 120; j++) {
			if (p[j] != NULL) {
				free(p[j]);
			}
		}
		gettimeofday(&end, NULL);
		time += (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
	}
	return time / 50;
}

// 6. test memory fragmentation performance
double task6() {
	struct timeval start, end;
	double time = 0;
	int i;
	for (i = 0; i < 50; i++) {
		gettimeofday(&start, NULL);
		char *p[MEMSIZE];
		int index = 0;
		// malloc until malloc returns NULL
		// malloc random size between 0 and MAXSIZE bytes
		while (1) {
			p[index] = malloc(rand() % (MAXSIZE + 1));
			if (p[index] == NULL) {
				break;
			}
			index++;
		}
		// free every third pointer between 0 and index
		int j;
		for (j = 0; j < index; j++) {
			if (j % 3 == 0) {
				if (p[j] != NULL) {
					free(p[j]);
				}
			}
		}
		// malloc those freed positions again
		for (j = 0; j < index; j++) {
			if (j % 3 == 0) {
				p[j] = malloc(rand() % (MAXSIZE + 1));
			}
		}
		// free all the pointers that are not NULL between 0 and index
		for (j = 0; j < index; j++) {
			if (p[j] != NULL) {
				free(p[j]);
			}
		}
		gettimeofday(&end, NULL);
		time += (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
	}
	return time / 50;
}

// run all the tasks and print the results including whether there is a memory leak at the end
void memgrind() {
	printf("Task 1 Average Time: %f microseconds\n", task1());
	printf("Task 2 Average Time: %f microseconds\n", task2());
	printf("Task 3 Average Time: %f microseconds\n", task3());
	printf("Task 4 Average Time: %f microseconds\n", task4());
	printf("Task 5 Average Time: %f microseconds\n", task5());
	printf("Task 6 Average Time: %f microseconds\n", task6());
	if (isMemoryLeaking()) {
		printf("Memory leak detected!\n");
	} else {
		printf("No memory leak detected!\n");
	}
}

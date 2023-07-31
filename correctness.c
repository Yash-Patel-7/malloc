#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mymalloc.h"

// enumeration for memory size and maximum allocation size in bytes
// where MEMSIZE > MAXSIZE
enum {
	MEMSIZE = 4104,
	MAXSIZE = 511
};

// prototypes of programs
void program1();
void program2();
void program3();
void program4();
void program5();
void program6();

// driver function
int main() {
	// prints the program number and calls each program
	printf("\nProgram 1 - free() deallocates memory previously allocated by malloc()\n");
	program1();
	printf("\nProgram 2 - the errors are reported by the program\n");
	program2();
	printf("\nProgram 3 - detects memory leaks\n");
	program3();
	printf("\nProgram 4 - malloc() and free() arrange so that adjacent free blocks are coalesced\n");
	program4();
	printf("\nProgram 5 - malloc() always returns memory addresses that are divisible by 8\n");
	program5();
	printf("\nProgram 6 - malloc() reserves unallocated memory\n");
	program6();
	printf("\n");
	// return successful exit status
	return EXIT_SUCCESS;
}

// free() deallocates memory previously allocated by malloc()
void program1() {
	// allocate memory
	char *ptr = (char *) malloc(MAXSIZE);
	// free memory
	free(ptr);
	// free the same pointer again to see if it was already deallocated
	free(ptr);
	// detect memory leaks
	if (isMemoryLeaking()) {
		printf("Memory leak detected!\n");
	} else {
		printf("No memory leak detected!\n");
	}
}

// the errors are reported by the program
void program2() {
	// calling free() with an address not obtained from malloc()
	int x;
	free(&x);

	// calling free() with an address not at the start of a chunk
	char *ptr = (char *) malloc(MAXSIZE);
	free(ptr + 1);

	// calling free() a second time on the same pointer (double free)
	char *ptr2 = ptr;
	free(ptr);
	free(ptr2);

	// detect memory leaks
	if (isMemoryLeaking()) {
		printf("Memory leak detected!\n");
	} else {
		printf("No memory leak detected!\n");
	}
}

// detects memory leaks
void program3() {
	// allocate memory
	char *ptr = (char *) malloc(MAXSIZE);
	// use isMemoryLeaking() to detect memory leaks
	if (isMemoryLeaking()) {
		printf("Memory leak detected!\n");
	} else {
		printf("No memory leak detected!\n");
	}
	// free memory
	free(ptr);
	// use isMemoryLeaking() to detect memory leaks
	if (isMemoryLeaking()) {
		printf("Memory leak detected!\n");
	} else {
		printf("No memory leak detected!\n");
	}
}

// malloc() and free() arrange so that adjacent free blocks are coalesced
void program4() {
	// compute the data size of each allocation for 4 equal allocations to fill the memory completely
	size_t size = (MEMSIZE - 72) >> 2;
	// compute the next smaller multiple of 8 of size
	size = (size >> 3) << 3;

	// allocate memory
	char *ptr1 = (char *) malloc(size);
	char *ptr2 = (char *) malloc(size);
	char *ptr3 = (char *) malloc(size);
	char *ptr4 = (char *) malloc(size);

	// at this point, the memory is full, so free the middle two allocations
	free(ptr2);
	free(ptr3);

	// now, the two adjacent free blocks should be coalesced
	// and the data size of the free block should be 2 * size + 16 (ptr 3's metadata size in bytes)
	// allocate memory
	char *ptr5 = (char *) malloc((size << 1) + 16);

	// ptr5 should be equal to ptr2 because allocations pointed to by ptr2 and ptr3 were coalesced into 1 free block, which starts at ptr2
	// example diagram: [ptr1][ptr2][ptr3][ptr4] -> after free() -> [ptr1][free][ptr4] -> after malloc() -> [ptr1][ptr5][ptr4]
	// ptr5 should be equal to ptr2 because the free block starts at ptr2
	if (ptr5 == ptr2) {
		printf("malloc() and free() coalesced adjacent free blocks!\n");
	} else {
		printf("malloc() and free() did not coalesce adjacent free blocks!\n");
	}

	// free the rest of the allocations
	free(ptr1);
	free(ptr4);
	free(ptr5);

	// detect memory leaks
	if (isMemoryLeaking()) {
		printf("Memory leak detected!\n");
	} else {
		printf("No memory leak detected!\n");
	}
}

// malloc() always returns memory addresses that are divisible by 8
void program5() {
	char *ptr[121];
	bool isAligned = true;
	for (int i = 0; i < 121; i++) {
		// use random size that is between 0 and 9
		int size = rand() % 10;
		ptr[i] = (char *) malloc(size);
		if (((size_t) ptr[i] & 7) != 0) {
			isAligned = false;
			break;
		}
	}
	// free if not null
	for (int i = 0; i < 121; i++) {
		if (ptr[i] != NULL) {
			free(ptr[i]);
		}
	}
	if (isAligned) {
		printf("malloc() always aligns allocations to 8 bytes!\n");
	} else {
		printf("malloc() does not always align allocations to 8 bytes!\n");
	}
	// detect memory leaks
	if (isMemoryLeaking()) {
		printf("Memory leak detected!\n");
	} else {
		printf("No memory leak detected!\n");
	}
}

// malloc() reserves unallocated memory
void program6() {
	// compute the data size of each allocation for 4 equal allocations to fill the memory completely
	size_t size = (MEMSIZE - 72) >> 2;

	// compute the next smaller multiple of 8 of size
	size = (size >> 3) << 3;

	// allocate memory
	char *ptr1 = (char *) malloc(size);
	char *ptr2 = (char *) malloc(size);
	char *ptr3 = (char *) malloc(size);
	char *ptr4 = (char *) malloc(size);

	// at this point the memory is full, so
	// for each allocation, fill it completely with a distinct character (e.g. 'a', 'b', 'c', 'd')
	for (int i = 0; i < size; i++) {
		ptr1[i] = 'a';
		ptr2[i] = 'b';
		ptr3[i] = 'c';
		ptr4[i] = 'd';
	}

	// check if the allocations still contain the same characters
	bool isOverlapping1 = false;
	for (int i = 0; i < size; i++) {
		if (ptr1[i] != 'a' || ptr2[i] != 'b' || ptr3[i] != 'c' || ptr4[i] != 'd') {
			isOverlapping1 = true;
			break;
		}
	}

	// free the middle two allocations
	free(ptr2);
	free(ptr3);

	// now, the two adjacent free blocks should be coalesced
	// and the data size of the free block should be 2 * size + 16 (ptr 3's metadata size in bytes)
	size_t freeSize = (size << 1) + 16;

	// allocate memory
	char *ptr5 = (char *) malloc(freeSize);

	// fill ptr5 with a distinct character (e.g. 'e')
	for (int i = 0; i < freeSize; i++) {
		ptr5[i] = 'e';
	}

	// check if the allocations still contain the same characters
	bool isOverlapping2 = false;
	for (int i = 0; i < freeSize; i++) {
		if (i < size) {
			if (ptr1[i] != 'a' || ptr4[i] != 'd') {
				isOverlapping2 = true;
				break;
			}
		}
		if (ptr5[i] != 'e') {
			isOverlapping2 = true;
			break;
		}
	}

	// if the allocations are overlapping, then print "malloc() produces overlapping allocations!"
	// otherwise, print "malloc() does not produce overlapping allocations!"
	if (isOverlapping1 || isOverlapping2) {
		printf("malloc() produces overlapping allocations!\n");
	} else {
		printf("malloc() does not produce overlapping allocations!\n");
	}
	
	// free the rest of the allocations
	free(ptr1);
	free(ptr4);
	free(ptr5);

	// detect memory leaks
	if (isMemoryLeaking()) {
		printf("Memory leak detected!\n");
	} else {
		printf("No memory leak detected!\n");
	}
}

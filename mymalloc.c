#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mymalloc.h"

// enumeration for memory size variable
// MEMSIZE is number of bytes in memory array that MUST be divisible by 8 and at least able to hold an allocation of 8 bytes of data
enum { MEMSIZE = 4104 };
// define memory array
static double mem[MEMSIZE / 8];
// define chunk struct containing size_t dataSize and a pointer to next chunk
typedef struct chunk {
	size_t dataSize;
	struct chunk *next;
} chunk;
// define reserved struct containing a pointer to the first chunk
typedef struct reserved {
	chunk *firstChunk;
} reserved;
// format of memory array is:
// reserved struct at beginning to keep track of first allocated chunk
// after that, there is a series of chunks in the format of chunk struct followed by the data
// example diagram: reserved struct -> chunk struct -> data -> chunk struct -> data -> chunk struct -> data -> ...
// data is the memory that is allocated to the user by mymalloc and freed by myfree (the user can't see the chunk struct)
// data is variable length, so the chunk struct is used to keep track of the size of the data that is allocated to the user
// the chunk struct is also used to keep track of the next chunk in the array in order to traverse all the chunks
// the reserved struct is used to keep track of the first chunk in the array so that the user can traverse all the chunks
void *mymalloc(size_t size, char *file, int line) {
	// if MEMSIZE is less than the size of the reserved struct + chunk struct + 8 bytes of data, or
	// if MEMSIZE is not divisible by 8, then printf error message with file and line number saying that the memory size is invalid and return NULL
	if (MEMSIZE < sizeof(reserved) + sizeof(chunk) + 8 || ((size_t) MEMSIZE & 7) != 0) {
		printf("Error at file %s at line %d: memory size is invalid\n", file, line);
		return NULL;
	}
	// cast memory array to a char pointer so that the pointer arithmetic uses bytes instead of doubles
	char *memory = (char *) mem;
	// compute the smallest multiple of 8 at least as large as size
	size = (size + 7) & ~7;
	// if size is 0 or greater than the maximum data size, which is total memory size minus reserved and chunk struct, return NULL
	if (size == 0 || size > MEMSIZE - sizeof(reserved) - sizeof(chunk)) {
		return NULL;
	}
	// get the reserved struct from the beginning of the memory array
	reserved *res = (reserved *) memory;
	// if the first chunk is NULL or the end of memory array, then there are 0 allocated chunks, so allocate the first chunk based on the size
	// set the last chunk's next pointer to the end of the memory array
	if (res->firstChunk == NULL || (char *) res->firstChunk == (memory + MEMSIZE)) {
		res->firstChunk = (chunk *) (memory + sizeof(reserved));
		res->firstChunk->dataSize = size;
		res->firstChunk->next = (chunk *) (memory + MEMSIZE);
		return (void *) (memory + sizeof(reserved) + sizeof(chunk));
	}
	// if the first chunk is not NULL, then there are at least 1 allocated chunks
	// so traverse the chunks to find free space in between the chunks that is big enough to hold data size + chunk struct
	// free space is defined as the space between the end of the previous chunk and the beginning of the next chunk
	// if there is no free space, then return NULL
	// if there is free space, then allocate the chunk and return the pointer to the data
	// update the next pointer of the previous chunk to point to the new chunk
	// update the next pointer of the new chunk to point to the next chunk so the chunks are still linked

	// if there is enough free space between the end of the reserved struct and the beginning of the first chunk, then allocate the chunk
	// diagram: reserved struct -> free space -> first chunk -> ...
	// set the next pointer of the new chunk to the first chunk
	// set the first chunk pointer of the reserved struct to the new chunk
	if ((char *) res->firstChunk - (memory + sizeof(reserved)) >= size + sizeof(chunk)) {
		chunk *newChunk = (chunk *) (memory + sizeof(reserved));
		newChunk->dataSize = size;
		newChunk->next = res->firstChunk;
		res->firstChunk = newChunk;
		return (void *) (memory + sizeof(reserved) + sizeof(chunk));
	}

	chunk *prevChunk = res->firstChunk;
	chunk *currChunk = prevChunk;
	while (true) {
		// compute the free space between the end of the previous chunk and the beginning of the next chunk
		// if current chunk is the same as previous chunk, then set free space to 0
		size_t freeSpace = 0;
		if (currChunk != prevChunk) {
			freeSpace = (char *) currChunk - (char *) prevChunk - prevChunk->dataSize - sizeof(chunk);
		}
		if (freeSpace >= size + sizeof(chunk)) {
			chunk *newChunk = (chunk *) ((char *) prevChunk + prevChunk->dataSize + sizeof(chunk));
			newChunk->dataSize = size;
			newChunk->next = currChunk;
			prevChunk->next = newChunk;
			return (void *) ((char *) newChunk + sizeof(chunk));
		}
		// if current chunk is at the end of the memory array, then there is no free space, so break out of the loop
		if ((char *) currChunk == (memory + MEMSIZE)) {
			break;
		}
		prevChunk = currChunk;
		currChunk = currChunk->next;
	}
	return NULL;
}
// free the chunk that contains the pointer
void myfree(void *ptr, char *file, int line) {
	// if MEMSIZE is less than the size of the reserved struct + chunk struct + 8 bytes of data, or
	// if MEMSIZE is not divisible by 8, then printf error message with file and line number saying that the memory size is invalid and return
	if (MEMSIZE < sizeof(reserved) + sizeof(chunk) + 8 || ((size_t) MEMSIZE & 7) != 0) {
		printf("Error at file %s at line %d: memory size is invalid\n", file, line);
		return;
	}
	// cast memory array to a char pointer so that the pointer arithmetic uses bytes instead of doubles
	char *memory = (char *) mem;
	// get the reserved struct from the beginning of the memory array
	reserved *res = (reserved *) memory;
	// if the first chunk is NULL meaning the memory has never been allocated, or
	// if the ptr is not in the range between the end of the reserved + chunk struct and the last index of the memory array, then
	// printf error message with file and line number saying that the pointer is not obtained from malloc and return
	if (res->firstChunk == NULL || (char *) ptr < (memory + sizeof(reserved) + sizeof(chunk)) || (char *) ptr >= (memory + MEMSIZE)) {
		printf("Error at file %s at line %d: pointer %p is not obtained from malloc\n", file, line, ptr);
		return;
	}
	// if the first chunk is equal to the end of the memory array, then the memory has been allocated but has been freed,
	// so printf error message with file and line number saying that the pointer has already been freed and return
	if ((char *) res->firstChunk == (memory + MEMSIZE)) {
		printf("Error at file %s at line %d: pointer %p has already been freed\n", file, line, ptr);
		return;
	}
	// if the first chunk is not NULL and not the end of the memory array, then there are at least 1 allocated chunks
	// if the pointer is not NULL, then there is something to free
	// so traverse the chunks to find the chunk that contains the pointer
	// if the pointer is not found, then return
	// if the pointer is found, then free the chunk
	// if the pointer is found at the first chunk, then update reserved struct to point to the first chunk in the array
	// update the next pointer of the previous chunk to point to the next chunk so the chunks are still linked
	chunk *prevChunk = res->firstChunk;
	chunk *currChunk = prevChunk;
	while (true) {
		// if the ptr is at the start of the data, then free the chunk
		if ((char *) currChunk + sizeof(chunk) == (char *) ptr) {
			if (currChunk == res->firstChunk) {
				res->firstChunk = currChunk->next;
			} else {
				prevChunk->next = currChunk->next;
			}
			return;
		}
		// if the ptr is not at the start of the data and if it is in the range of the rest of the data, then print error and return
		// example diagram where each hexadecimal represents 1 byte:
		// prevChunk[0x5] data[0x6 0x7] currChunk[0x8] data[0x9 0xa]
		// if ptr is 0x7, then it is not at the beginning of the data but it is in the range of the rest of the data, 
		// so print error message using file and line number saying that the pointer is not at the start of the chunk
		if ((char *) ptr > (char *) currChunk + sizeof(chunk) && (char *) ptr < (char *) currChunk + sizeof(chunk) + currChunk->dataSize) {
			printf("Error at file %s at line %d: pointer %p is not at the start of the chunk\n", file, line, ptr);
			return;
		}

		// if the ptr belongs to free space between the end of the previous chunk and the beginning of the next chunk, then print error and return
		// example diagram where each hexadecimal represents 1 byte:
		// prevChunk[0x5] data[0x6] free[0x7] currChunk[0x8] data[0x9]
		// if ptr is 0x7, then it belongs to free space, so print error message using file and line number
		// saying that the pointer has already been freed
		if ((char *) ptr >= (char *) prevChunk + prevChunk->dataSize + sizeof(chunk) && (char *) ptr < (char *) currChunk) {
			printf("Error at file %s at line %d: pointer %p has already been freed\n", file, line, ptr);
			return;
		}
		// if current chunk is at the end of the memory array, then the pointer is not found, so break out of the loop
		if ((char *) currChunk == (memory + MEMSIZE)) {
			break;
		}
		prevChunk = currChunk;
		currChunk = currChunk->next;
	}
}
// check memory leaks
size_t isMemoryLeaking() {
	// if MEMSIZE is less than the size of the reserved struct + chunk struct + 8 bytes of data, or
	// if MEMSIZE is not divisible by 8, then return false
	if (MEMSIZE < sizeof(reserved) + sizeof(chunk) + 8 || ((size_t) MEMSIZE & 7) != 0) {
		printf("Error: memory size is invalid\n");
		return false;
	}
	// cast memory array to a char pointer so that the pointer arithmetic uses bytes instead of doubles
	char *memory = (char *) mem;
	// if the first chunk is NULL or at the end of the memory array, then there are no memory leaks
	// so return false otherwise return true
	reserved *res = (reserved *) memory;
	return !(res->firstChunk == NULL || (char *) res->firstChunk == (memory + MEMSIZE));
	
}

Yash Patel - NetID: ypp10
Parth Patel - NetID: pp847

Test Plan: correctness.c

(a) Properties or Requirements
	1. free() deallocates memory previously allocated by malloc().
	2. the errors are reported by the program.
	3. detects memory leaks.
	4. malloc() and free() arrange so that adjacent free blocks are coalesced.
	5. malloc() always returns memory addresses that are divisible by 8.
	6. malloc() reserves unallocated memory.

(b) Determination
	1. malloc() a block of memory, free() it, and then call free() on it again to determine if the block was already deallocated by the previous call to free().
	2. 	a. declare a local variable, pass its address to free(), to determine if the address was not obtained from malloc().
		b. malloc() a block of memory, pass its address + 1 to free(), to determine if the address is not at the start of the chunk.
		c. call free() a second time on the same pointer to determine if the pointer was already freed before.
	3. malloc() a block of memory, isMemoryLeaking() should be true, free() it, isMemoryLeaking() should be false to determine memory leaks.
	4. 	a. completely fill the memory with 4 equal allocations.
		b. free() the middle 2 allocations.
		c. malloc() the size of the free chunk.
		d. determine if malloc() returns the same pointer as the 2nd allocation since the free chunk starts after the 1st allocation.
	5. malloc() a block of memory of a random size between 0 and 9, 121 times, to determine if all 121 pointers returned are divisible by 8.
	6. 	a. completely fill the memory with 4 equal allocations.
		b. fill each of the 4 allocations with a distinct character.
		c. determine if the allocations still contain the same distinct character.
		d. free() the middle 2 allocations.
		e. malloc() the size of the free chunk.
		f. fill the last allocation with a distinct character.
		g. determine if the current allocations still contain the same distinct character.
		h. determine whether malloc() produced overlapping allocations to see if malloc() reserves unallocated memory.

(c) Methods or Programs (correctness.c)
	1. program1 for requirement 1.
	2. program2 for requirement 2.
	3. program3 for requirement 3.
	4. program4 for requirement 4.
	5. program5 for requirement 5.
	6. program6 for requirement 6.

Performance: memgrind.c
	1. Task 1: malloc() and immediately free() a 1-byte chunk, 120 times.
	2. Task 2: Use malloc() to get 120 1-byte chunks, storing the pointers in an array, then use free() to deallocate the chunks.
	3. Task 3:	Randomly choose between:
				a. Allocating a 1-byte chunk and storing the pointer in an array.
				b. Deallocating one of the chunks in the array (if any).
				Repeat above until you have called malloc() 120 times, regardless of the number of iterations.
	4. Task 4: Repeat number 1 but with a variable number of bytes to allocate called size, including from 0 to MAXSIZE bytes.
	5. Task 5: Repeat number 2 but with a variable number of bytes to allocate called size, including from 0 to MAXSIZE bytes.
	6. Task 6: Test memory fragmentation performance by allocating until malloc returns NULL, freeing every third allocation, and malloc again.

Design Notes:
	1. All the design properties or requirements were proved by the test programs.
	2. The allocations are aligned to 8 bytes, so the length of the memory array in bytes must be divisible by 8 and at least able to hold 8 bytes of data.
	3. malloc() and free() reserve a few bytes at the beginning of the memory array to store any critical information about the chunks.
	For this reason, the few bytes at the beginning of the memory array cannot be allocated to the user.

Other Notes:
	1. Memory size of 4104 bytes is special because it allows 4 equal allocations to completely fill the memory.
	This is important because the correctness programs use this memory size to prove some of the requirements.

Execution in terminal:
	1. Ensure that you are in the correct directory where the files reside
	2. Compile all the files using this command: make
	3. Run the correctness programs using this command: ./correctness
	4. Run the performance tests using this command: ./memgrind
	5. Clean the environment using this command: make clean

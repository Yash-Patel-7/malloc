all: build

build: clean correctness memgrind

correctness: correctness.c
	rm -rf correctness && gcc -g -Wall -Werror -fsanitize=address -std=c99 correctness.c mymalloc.c -o correctness

memgrind: memgrind.c
	rm -rf memgrind && gcc -g -Wall -Werror -fsanitize=address -std=c99 memgrind.c mymalloc.c -o memgrind

clean:
	rm -rf correctness && rm -rf memgrind

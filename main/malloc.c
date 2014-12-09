
#include <malloc.h>

extern unsigned int _end;

void *malloc(unsigned int size) {
	return (void *)((_end += size)-size);
}

void *malloc_aligned(unsigned int size, unsigned int alignment) {
	_end += (alignment - (_end % alignment));
	return malloc(size);
}


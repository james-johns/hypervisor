/**
 * \file
 * \author James Johns
 */
#include <malloc.h>

extern unsigned int _end;

/**
 * \fn malloc(unsigned int size)
 *
 * Allocate memory located at the address stored in _end.
 *
 * Note: does not support freeing memory
 */
void *malloc(unsigned int size) {
	return (void *)((_end += size)-size);
}

/**
 * \fn malloc_aligned(unsigned int size, unsigned int alignment)
 *
 * Allocate memory located at the address stored in _end, and align
 * the base address to a multiple of alignment
 *
 * Note: does not support freeing memory or reallocating memory holes
 */
void *malloc_aligned(unsigned int size, unsigned int alignment) {
	_end += (alignment - (_end % alignment));
	return malloc(size);
}


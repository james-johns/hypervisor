

#ifndef _MALLOC_H_
#define _MALLOC_H_

void *malloc(unsigned int size);
void *malloc_aligned(unsigned int size, unsigned int alignment);

void test_malloc();

#endif

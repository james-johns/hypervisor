


#ifndef _PAGETABLE_H_
#define _PAGETABLE_H_

#include <memory.h>

struct pageTable_s *createPageTable();

void printPageTable(struct pageTable_s *table, unsigned int baseAddr, unsigned int level);
void mapVirtToPhys(struct pageTable_s *table, unsigned int virt, unsigned int phys,
		unsigned int size, unsigned int attrs);

#endif

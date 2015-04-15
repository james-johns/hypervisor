/**
 * \file
 * \author James Johns
 */

#include <pageTable.h>
#include <memory.h>
#include <malloc.h>
#include <printh.h>

#define NULL ((void *)0)

void printPageTable(struct pageTable_s *table, unsigned int baseAddr, unsigned int level)
{
	int i = 0;
	unsigned int offsetAddr = baseAddr;
	struct pageDescriptor_s desc;
	for (i = 0; i < LPAE_ENTRIES; i++) {
		desc = table->entry[i];
		offsetAddr += ((level == 1) ? 0x40000000 : ((level == 2) ? 0x00100000 : 0x00001000));

		if (!(desc.type & 0x1))
			continue;
		printh("%s mapping %d to %d (level %d)\r\n", ((desc.type & 0x2 && level < 3) ? "table" : "   page"),
			baseAddr + (i * ((level == 1) ? 0x40000000 : ((level == 2) ? 0x00100000 : 0x00001000))),
			((desc.highAddr << 21) | desc.lowAddr << 12), level);
		if (desc.type & 0x2 && level < 3) {
			printPageTable((struct pageTable_s *)((desc.highAddr << 21) | (desc.lowAddr << 12)),
				offsetAddr, level+1);
		}
	}
}

/**
 * \fn createPageTable
 *
 * Allocate and initialise a page table structure.
 */
struct pageTable_s *createPageTable() 
{
	struct pageTable_s *toRet;
	unsigned int i;

	toRet = (struct pageTable_s *) malloc_aligned(sizeof(struct pageTable_s), 4096);
	for (i = 0; i < LPAE_ENTRIES; i++) {
		toRet->entry[i].type = 0x0;
	}

	return toRet;
}

/**
 * \fn createPageDescriptor(unsigned int phys, unsigned int type, unsigned int attrs)
 *
 * Create page descriptor bit pattern from arguments and return as structure.
 */
struct pageDescriptor_s createPageDescriptor(unsigned int phys, unsigned int type, unsigned int attrs)
{
	struct pageDescriptor_s toRet;

	toRet.type = type & 0x3;
	toRet.lowAddr    = ((phys & 0x001FF000) >> 12);
	toRet.highAddr   = ((phys & 0xFFE00000) >> 21);
	toRet.lowAttrs   = (attrs & 0x000003FF);
	toRet.highAttrs  = (attrs & 0x000FFC00) >> 10;
	toRet.zero = 0;

	return toRet;
}

/**
 * \fn setPageTableEntry(struct pageTable_s *table, struct pageDescriptor_s descriptor,
 *                     unsigned short offset)
 *
 * Set entry in page table at offset to page descirptor
 */
void setPageTableEntry(struct pageTable_s *table, 
		struct pageDescriptor_s descriptor, unsigned short offset)
{
	if (offset >= 0x200)
		return;

	table->entry[offset] = descriptor;
}

/**
 * \fn getPageTable(struct pageDescriptor_s descriptor)
 *
 * Return next level pageTable pointed to by descriptor.
 */
struct pageTable_s *getPageTable(struct pageDescriptor_s descriptor)
{
	struct pageTable_s *toRet;
	unsigned int addr;

	if (descriptor.type != 0x3)
		return NULL;
	addr = (descriptor.highAddr << 21) | (descriptor.lowAddr << 12);
	toRet = (struct pageTable_s *)addr;

	return toRet;
}

/**
 * \fn mapPageTable(unsigned int startLevel, struct pageTable_s *table, 
 *                struct pageTable_s *nextTable, unsigned int virt)
 *
 * Set page table entry to point to next level page table
 */
void mapPageTable(unsigned int startLevel, struct pageTable_s *table, 
		struct pageTable_s *nextTable, unsigned int virt)
{
	struct pageDescriptor_s desc = createPageDescriptor((unsigned int)nextTable, 0x3, 0x3df);
	unsigned int offset = (virt & ((startLevel == 1) ? 0xC0000000 : 0x3FE00000))
		>> ((startLevel == 1) ? 30 : 21);

	setPageTableEntry(table, desc, offset);
}

/**
 * \fn mapVirtToPhys(struct pageTable_s *table, unsigned int virt, unsigned int phys,
 *                unsigned int size, unsigned int attrs)
 *
 * Map virtual address to physical address in page table for specified length, 
 * with specified attrs. Procedure breaks size into appropriate blocks to manage
 * memory block limits per entry in each level of the page table stricture.
 *
 * Assumes all memory from virt address to virt address + size is not mapped in the
 * page table yet - if the memory is already mapped, this will overwrite the mappings.
 */
void mapVirtToPhys(struct pageTable_s *table, unsigned int virt, unsigned int phys,
		unsigned int size, unsigned int attrs)
{
	unsigned int i, j, k;
	unsigned int allocated = 0;
	for (i = (((virt + allocated) & 0xC0000000) >> 30); i < LPAE_ENTRIES && allocated < size; i++) {
		if (size - allocated >= 0x40000000 && !((virt + allocated) & 0x3FFFFFFF)) {
			/* table entry can be a block instead of page table */
			struct pageDescriptor_s desc = createPageDescriptor(phys + allocated, 0x1, attrs);
			setPageTableEntry(table, desc, i);
			allocated += 0x40000000;
			continue;
		} else {	/* else */
			struct pageTable_s *secondLevel = getPageTable(table->entry[i]);
			if (secondLevel == NULL) {
				secondLevel = createPageTable();
				mapPageTable(1, table, secondLevel, (i*0x40000000));
			}
			for (j = (((virt + allocated) & 0x3FE00000) >> 21); j < LPAE_ENTRIES && allocated < size; j++) {
				if (size - allocated >= 0x00200000 && !((virt + allocated) & 0x001FFFFF)) {
					/* table entry can be a block instead of page table */
					struct pageDescriptor_s desc = createPageDescriptor(phys + allocated, 0x1, attrs);
					setPageTableEntry(secondLevel, desc, j);
					allocated += 0x00200000;
					continue;
				} else {
					struct pageTable_s *thirdLevel = getPageTable(secondLevel->entry[j]);
					if (thirdLevel == NULL && !(secondLevel->entry[j].type & 0x1)) {
						thirdLevel = createPageTable();
						mapPageTable(2, secondLevel, thirdLevel, 
							(i*0x40000000) + (j*0x00200000));
					}
					for (k = (((virt + allocated) & 0x001FF000) >> 12); k < LPAE_ENTRIES && allocated < size; k++) {
						struct pageDescriptor_s desc = createPageDescriptor(phys + allocated, 0x3, attrs);
						setPageTableEntry(thirdLevel, desc, k);
						allocated += 0x1000;
					}
				}
			}
		}
	}
}

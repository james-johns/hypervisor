
#include <config.h>
#include <memory.h>

#define LPAE_ENTRIES 0x200	// 512 entries of 8 bytes each; 4k table size
struct pageDescriptor_s pageTable[LPAE_ENTRIES]
    __attribute__ ((__aligned__(4096)));

void setHTTBR(unsigned int lo, unsigned int hi);
void setMAIR0VAL(unsigned int val);
void setMAIR1VAL(unsigned int val);
void enableMMU();

void mapBlockToPhys();
void mapPageL2ToPhys();
void mapPageL3ToPhys();

/* table must be top level page table, virt and phys will be page aligned */
void mapVirtToPhys(struct pageDescriptor_s *table, void *virt, void *phys,
		unsigned int size, unsigned short attrs)
{
	unsigned int i;
	unsigned int offset = ((((unsigned int)virt)>>30) & 0x03);
	for (i = offset;
	     size >= 0x40000000;
	     i++, (size -= 0x40000000), (phys += 0x40000000), (virt += 0x40000000)) {

		table[i].block.type = 0x1;
		table[i].block.outputAddr = (((unsigned int)phys) & (0xFFFFFF00)) >> 21;
		table[i].block.upperBlockAttrs = (attrs & 0xC00)>>10;
		table[i].block.lowerBlockAttrs = attrs & 0x3FF;
	        table[i].block.zero1 = 0;
		table[i].block.zero2 = 0;
	}
}

void init_mmu()
{
	unsigned int i;

	/* zero the table */
	for (i = 0; i < LPAE_ENTRIES; i++) {
		pageTable[i].block.type = 0x0;
		pageTable[i].block.outputAddr = 0;
		pageTable[i].block.upperBlockAttrs = 0x0;
		pageTable[i].block.lowerBlockAttrs = 0x0;
		pageTable[i].block.zero1 = 0;
		pageTable[i].block.zero2 = 0;
	}

	/* devices */
	mapVirtToPhys(pageTable, 0x0, 0x0, 0x40000000, 0x39C);
	/* RAM */
	mapVirtToPhys(pageTable, (void *)0x40000000, (void *)0x40000000, 
		0x80000000, 0x3DF);
	/* System memory */
	mapVirtToPhys(pageTable, (void *)0xC0000000, (void *)0xC0000000, 
		0x40000000, 0x3DF);

	unsigned int HTTBR_hi, HTTBR_lo;
	HTTBR_hi = 0;
	HTTBR_lo = (unsigned int) pageTable;
	setHTTBR(HTTBR_lo, HTTBR_hi);
	setMAIR0VAL(0xEEAA4400);
	setMAIR1VAL(0xFF000004);
	enableMMU();
}



#include <vm.h>
#include <malloc.h>
#include <pageTable.h>
#include <printh.h>

extern struct guestVM_s *nextScheduledGuest;

/* contextual wrapper for mapping memory to a VM guest */
void mapMemoryToVM(struct guestVM_s *guest, unsigned int baseAddr, 
		unsigned int targetAddr, unsigned int size, unsigned int attrs)
{
	mapVirtToPhys(guest->stageOneTable, baseAddr, targetAddr, size, attrs);
}

void runVM(struct guestVM_s *guest)
{
	nextScheduledGuest = guest;
}

/* baseAddr is the base address of RAM to be assigned to VM. 
 * Kernel must be placed 32KiB into this section of RAM.  */
struct guestVM_s *createVM(unsigned int baseAddr, unsigned int memorySize)
{
	struct guestVM_s *guest = malloc(sizeof(struct guestVM_s));

	guest->stageOneTable = createPageTable();
	mapMemoryToVM(guest, baseAddr, 0x40000000, memorySize, 0x3DF);
	mapMemoryToVM(guest, 0x01c28000, 0x01c28000, 0x4000, 0x39C);

	guest->regs.pc = (baseAddr + 0x8000);
	guest->regs.cpsr = 0x000001D3;
	guest->regs.r0 = 0;
	guest->regs.r1 = 0x000010bb;
	guest->regs.r2 = 0;
	guest->regs.r3 = 0;


	return guest;
}




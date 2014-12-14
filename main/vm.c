

#include <vm.h>
#include <malloc.h>
#include <pageTable.h>
#include <printh.h>

void print_regs(struct cpuRegs_s *regs);

/* contextual wrapper for mapping memory to a VM guest */
void mapMemoryToVM(struct guestVM_s *guest, unsigned int baseAddr, 
		unsigned int targetAddr, unsigned int size, unsigned int attrs)
{
	mapVirtToPhys(guest->stageOneTable, targetAddr, baseAddr, size, attrs);
}

/* baseAddr is the base address of RAM to be assigned to VM. 
 * Kernel must be placed 32KiB into this section of RAM.  */
struct guestVM_s *createVM(unsigned int baseAddr, unsigned int memorySize)
{
	struct guestVM_s *guest = malloc(sizeof(struct guestVM_s));

	guest->stageOneTable = createPageTable();
	mapMemoryToVM(guest, baseAddr, 0x40000000, memorySize, 0x1FF); // 01 1111 1111
	mapMemoryToVM(guest, 0x01c28000, 0x01c28000, 0x1000, 0x1B1);   // 01 1011 0001

	guest->regs.pc = (0x40008000);
	guest->regs.cpsr = 0x000001D3;
	guest->regs.r0 = 0;
	guest->regs.r1 = 0x000010bb;
	guest->regs.r2 = 0;
	guest->regs.r3 = 0;

	printh("New Guest Regs:\r\n");
	print_regs(&(guest->regs));
	return guest;
}




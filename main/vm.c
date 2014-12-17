

#include <vm.h>
#include <malloc.h>
#include <pageTable.h>
#include <printh.h>
#include <config.h>
#include <gic.h>

void print_regs(struct cpuRegs_s *regs);

/* contextual wrapper for mapping memory to a VM guest */
void mapMemoryToVM(struct guestVM_s *guest, unsigned int baseAddr, 
		unsigned int targetAddr, unsigned int size, unsigned int attrs)
{
	printh("Mapping %d to %d (size %d) for VM\r\n", baseAddr, targetAddr, size);
	mapVirtToPhys(guest->stageOneTable, targetAddr, baseAddr, size, attrs);
}

/* baseAddr is the base address of RAM to be assigned to VM. 
 * Kernel must be placed 32KiB into this section of RAM.  */
struct guestVM_s *createVM(unsigned int baseAddr, unsigned int memorySize)
{
	struct guestVM_s *guest = malloc(sizeof(struct guestVM_s));

	guest->stageOneTable = createPageTable();
	mapMemoryToVM(guest, baseAddr, 0x40000000, memorySize, 0x1FF); /* base memory map */
	mapMemoryToVM(guest, 0x01c28000, 0x01c28000, 0x1000, 0x1B1);   /* UART0 */
	mapMemoryToVM(guest, 0x01c00000, 0x01c00000, 0x1000, 0x1B1);   /* SRAM config regs */
	mapMemoryToVM(guest, (unsigned int)GICV(0), (unsigned int)GICC, 
		0x1000, 0x1B1);   /* VGIC mappings */

	guest->regs.pc = (0x40008000);
	guest->regs.cpsr = 0x00000013;
	guest->regs.r0 = 0;
	guest->regs.r1 = 0x000010bb;
	guest->regs.r2 = 0x40000000;
	guest->regs.r3 = 0;
	guest->vgic.ctlr = 0;

	printh("New Guest Regs:\r\n");
	print_regs(&(guest->regs));
	return guest;
}






#include <vm.h>
#include <malloc.h>
#include <pageTable.h>
#include <printh.h>
#include <config.h>
#include <gic.h>
#include <irq.h>
#include <virtdevice.h>
#include <vtty.h>

void print_regs(struct cpuRegs_s *regs);

/* contextual wrapper for mapping memory to a VM guest */
void mapMemoryToVM(struct guestVM_s *guest, unsigned int baseAddr, 
		unsigned int targetAddr, unsigned int size, unsigned int attrs)
{
	printh("Mapping %d to %d (size %d) for VM\r\n", baseAddr, targetAddr, size);
	mapVirtToPhys(guest->vcpu.stageOneTable, targetAddr, baseAddr, size, attrs);
}

void dummyIRQHandler(struct cpuRegs_s *regs)
{
	regs->r0 = regs->r0;
}

/* baseAddr is the base address of RAM to be assigned to VM. 
 * Kernel must be placed 32KiB into this section of RAM.  */
struct guestVM_s *createVM(const char *name, unsigned int baseAddr, unsigned int memorySize)
{
	struct guestVM_s *guest = malloc(sizeof(struct guestVM_s));
	guest->name = name;

//	registerIRQHandler(0x1B, dummyIRQHandler);
	guest->vcpu.stageOneTable = createPageTable();
	mapMemoryToVM(guest, baseAddr, 0x40000000, memorySize, 0x1FF); /* base memory map */
	mapMemoryToVM(guest, 0x00000000, 0x00000000, 0x10000, 0x1B1);   /* SRAM */
//	mapMemoryToVM(guest, 0x01c28000, 0x01c28000,  0x1000, 0x1B1);   /* UART0 */
	mapMemoryToVM(guest, 0x01c00000, 0x01c00000,  0x1000, 0x1B1);   /* SRAM config regs */
	mapMemoryToVM(guest, 0x01c01000, 0x01c01000,  0x1000, 0x1B1);   /* DRAM config regs */
	mapMemoryToVM(guest, 0x01c02000, 0x01c02000,  0x1000, 0x1B1);   /* DMA config regs */
	mapMemoryToVM(guest, 0x01c0a000, 0x01c0a000,  0x1000, 0x1B1);   /* TVE0 config regs */
	mapMemoryToVM(guest, 0x01c0C000, 0x01c0C000,  0x2000, 0x1B1);   /* LCD config regs */
	mapMemoryToVM(guest, 0x01c0F000, 0x01c0F000,  0x4000, 0x1B1);   /* MMC device regs */
	mapMemoryToVM(guest, 0x01c13000, 0x01c13000,  0x1000, 0x1B1);   /* USB0 config regs */
	mapMemoryToVM(guest, 0x01c14000, 0x01c14000,  0x1000, 0x1B1);   /* USB1 config regs */
	mapMemoryToVM(guest, 0x01c16000, 0x01c16000,  0x1000, 0x1B1);   /* HDMI config regs */
	mapMemoryToVM(guest, 0x01c1B000, 0x01c1B000,  0x1000, 0x1B1);   /* TVE config regs */
	mapMemoryToVM(guest, 0x01c1C000, 0x01c1C000,  0x1000, 0x1B1);   /* USB2 config regs */
	mapMemoryToVM(guest, 0x01c18000, 0x01c18000,  0x1000, 0x1B1);   /* SATA config regs */
//	mapMemoryToVM(guest, 0x01c20000, 0x01c20000,  0x1000, 0x1B1);   /* PIO Config regs */
	mapMemoryToVM(guest, 0x01c22000, 0x01c22000,  0x1000, 0x1B1);   /* IIS1 config regs */
	mapMemoryToVM(guest, 0x01c25000, 0x01c25000,  0x3000, 0x1B1);   /* CPU config regs */
	mapMemoryToVM(guest, 0x01c2a000, 0x01c2a000,  0x2000, 0x1B1);   /* PS2 config regs */
	mapMemoryToVM(guest, 0x01c2c000, 0x01c2c000,  0x1000, 0x1B1);   /* TWI4 config regs */
	mapMemoryToVM(guest, 0x01c50000, 0x01c50000, 0x10000, 0x1B1);   /* GMAC config regs */
	mapMemoryToVM(guest, 0x01c60000, 0x01c60000,  0x1000, 0x1B1);   /* HSTIMER config regs */
	mapMemoryToVM(guest, 0x01e00000, 0x01e00000, 0x40000, 0x1B1);   /* DEFE config regs */
	mapMemoryToVM(guest, 0x01e40000, 0x01e40000, 0x40000, 0x1B1);   /* DEBE config regs */
	mapMemoryToVM(guest, (unsigned int)GICV(0), (unsigned int)GICC, 
		0x1000, 0x1B1);   /* VGIC mappings */

//	printPageTable(guest->stageOneTable, 0x0, 1);

	guest->vcpu.vtty.guestPage = malloc_aligned(0x1000, 0x1000);
	memcpy((void *)0x01c28000, guest->vcpu.vtty.guestPage, 0x400);
	mapMemoryToVM(guest, (unsigned int)guest->vcpu.vtty.guestPage, 0x01c28000, 0x1000, 0x191);
	vttyInit(guest->vcpu.vtty.guestPage);

	memset(&guest->vcpu.regs, 0, sizeof(struct cpuRegs_s));

	guest->vcpu.regs.pc = (0x40008000);
	guest->vcpu.regs.SP_svc = (0x40000000 + (memorySize-1));
	guest->vcpu.regs.cpsr = 0x00000013;
	guest->vcpu.regs.r0 = 0;
	guest->vcpu.regs.r1 = 0x000010bb;
	guest->vcpu.regs.r2 = 0x40000000;
	guest->vcpu.regs.r3 = 0;

	guest->vcpu.vgic.ctlr = 0;
	guest->vcpu.vgic.lr_lines = (GICH[GICH_VTR] & 0x3F)+1;
	guest->vcpu.vgic.lr = malloc(sizeof(unsigned int)*(guest->vcpu.vgic.lr_lines));
	memset(&guest->vcpu.vgic.lr, 0, sizeof(unsigned int)*guest->vcpu.vgic.lr_lines);

	memset(&guest->vcpu.coproc15, 0, sizeof(struct cp15_s));

	printh("New Guest Regs:\r\n");
	print_regs(&(guest->vcpu.regs));
	return guest;
}




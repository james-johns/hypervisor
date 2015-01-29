

#ifndef _VM_H_
#define _VM_H_

#include <cpu.h>
#include <vgic.h>
#include <vtty.h>

struct guestVM_s {
	struct cpuRegs_s regs;
	struct pageTable_s *stageOneTable;
	struct vgic_s vgic;
	struct vtty_s vtty;
};

struct guestVM_s *createVM(unsigned int kernelAddr, unsigned int memorySize);

#endif

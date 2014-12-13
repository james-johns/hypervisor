

#ifndef _VM_H_
#define _VM_H_

#include <cpu.h>

struct guestVM_s {
	struct cpuRegs_s regs;
	struct pageTable_s *stageOneTable;
};

struct guestVM_s *createVM(unsigned int kernelAddr, unsigned int memorySize);
void runVM(struct guestVM_s *guest);

#endif

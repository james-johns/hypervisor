

#ifndef _VM_H_
#define _VM_H_

#include <cpu.h>
#include <vgic.h>
#include <vtty.h>

struct guestVM_s {
	const char *name;
	struct cpu_s vcpu;
};

struct guestVM_s *createVM(const char *name, unsigned int kernelAddr, unsigned int memorySize);

#endif

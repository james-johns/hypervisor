


#ifndef _VGIC_H_
#define _VGIC_H_

#include <cpu.h>

struct vgic_s {
	unsigned int ctlr;
	unsigned int typer;
	unsigned int enabled[0x80];
	unsigned int priority[0x400];
};

void vgicHandler(unsigned int hsr, unsigned int hpfar, 
		unsigned int hdfar, struct cpuRegs_s *regs);

void triggerVIRQ(unsigned int irqNum);

#endif

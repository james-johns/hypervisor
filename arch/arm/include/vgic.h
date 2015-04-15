


#ifndef _VGIC_H_
#define _VGIC_H_

struct vgic_s {
	unsigned int ctlr, apr, vmcr;
	unsigned int typer;
	unsigned int lr_lines;
	unsigned int *lr;
	unsigned int enabled[0x80];
	unsigned int priority[0x400];
	unsigned int target[0x100];
};

#include <cpu.h>

void saveVGIC(struct vgic_s *vgic);
void restoreVGIC(struct vgic_s *vgic);

void vgicHandler(unsigned int hsr, unsigned int hpfar, 
		unsigned int hdfar, struct cpuRegs_s *regs);
void vgicVirtDeviceHandler(struct cpuRegs_s *regs);

void triggerVIRQ(unsigned int irqNum);

#endif

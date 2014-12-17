


#ifndef _VTIMER_H_
#define _VTIMER_H_

#include <cpu.h>

struct vtimerx_s {
	unsigned int ctrl;
	unsigned int intrvl;
	unsigned int curval;
};

struct vtimer_s {
	unsigned int status;
};


void vtimerHandler(unsigned int hsr, unsigned int hpfar, unsigned int hdfar, struct cpuRegs_s *regs);

#endif

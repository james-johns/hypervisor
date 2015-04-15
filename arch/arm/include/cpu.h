


#ifndef _CPU_H_
#define _CPU_H_

struct cpuRegs_s {
	unsigned int r0;
	unsigned int r1;
	unsigned int r2;
	unsigned int r3;
	unsigned int r4;
	unsigned int r5;
	unsigned int r6;
	unsigned int r7;
	unsigned int r8;
	unsigned int r9;
	unsigned int r10;
	unsigned int r11;
	unsigned int r12;
	unsigned int pc;
	unsigned int cpsr;
	unsigned int SPSR_svc;
	unsigned int lr;
	unsigned int SP_usr;
	unsigned int SP_svc;
	unsigned int lr_svc;
} __attribute__((packed));

#include <vtty.h>
#include <vgic.h>

/* coprocessor 15 register values */
struct cp15_s {
	unsigned int ttbr0_lo, ttbr0_hi, ttbr1_lo, ttbr1_hi;
	unsigned int ttbcr, dacr;                  /* Translation table regs */
	unsigned int mair0, mair1, amair0, amair1; /* Memory attribute regs */
	unsigned int vbar;			   /* IRQ base address register */
	unsigned int par_lo, par_hi;               /* Physical address register */
	unsigned int sctlr;
};

struct cpu_s {
	struct cpuRegs_s regs;
	struct cp15_s coproc15;
	struct vgic_s vgic;
	struct vtty_s vtty;
	struct pageTable_s *stageOneTable;
};

#endif




#ifndef _GIC_H_
#define _GIC_H_

#ifndef GIC_BASE
#error "GIC_BASE must be defined before including gic.h, maybe you forgot to include config.h?"
#endif

#define GICD ((unsigned int *)(GIC_BASE + 0x1000))
#define GICC ((unsigned int *)(GIC_BASE + 0x2000))
#define GICH ((unsigned int *)(GIC_BASE + 0x4000))

#define GICV(n) ((unsigned int *)(GIC_BASE + 0x6000 + (0x1000 * n)))

#define GICD_CTLR           0	// 0x00
#define GICD_TYPER          (0x04/4)	// 0x04 / 4
#define GICD_IIDR           (0x08/4)	// 0x08 / 4
#define GICD_IGROUPR(n)     n+(0x80/4)
#define GICD_ISENABLER(n)   n+(0x100/4)
#define GICD_ICENABLER(n)   n+(0x180/4)
#define GICD_ISPENDR(n)     n+(0x200/4)
#define GICD_ICPENDR(n)     n+(0x280/4)
#define GICD_ISACTIVER(n)   n+(0x300/4)
#define GICD_ICACTIVER(n)   n+(0x380/4)
#define GICD_IPRIORITYR(n)  n+(0x400/4)
#define GICD_ITARGETSR(n)   n+(0x800/4)
#define GICD_ICFGR(n)       n+(0xc00/4)
#define GICD_NSACR(n)       n+(0xe00/4)
#define GICD_SGIR           (0xf00/4)	// 0xf00 / 4
#define GICD_CPENDSGIR(n)   n+(0xf10/4)
#define GICD_SPENDSGIR(n)   n+(0xf20/4)

#define GICC_CTLR           0
#define GICC_PMR            1
#define GICC_BPR            2
#define GICC_IAR            3
#define GICC_EOIR           4
#define GICC_DIR            (0x1000/4)

#define GICH_HCR            (0x0)
#define GICH_VTR            (0x4/4)
#define GICH_VMCR           (0x8/4)
#define GICH_MISR           (0x10/4)
#define GICH_EOISR(n)       n+(0x20/4)
#define GICH_ELSR(n)        n+(0x30/4)
#define GICH_APR            (0xF0/4)
#define GICH_LR(n)          n+(0x100/4)

void init_gic();

void enable_irq(unsigned int irqn);
void enableIRQS();
void disableIRQS();


#endif

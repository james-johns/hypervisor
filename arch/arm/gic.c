
#include <config.h>
#include <gic.h>
#include <vgic.h>
#include <printh.h>
#include <irq.h>
#include <virtdevice.h>


void printGICHypState()
{
	printh("GICH_HCR: %d\r\n", GICH[GICH_HCR]);
	printh("GICH_VMCR: %d\r\n", GICH[GICH_VMCR]);
	printh("GICH_MISR: %d\r\n", GICH[GICH_MISR]);
	printh("GICH_EOISR0: %d\r\n", GICH[GICH_EOISR(0)]);
	printh("GICH_EOISR1: %d\r\n", GICH[GICH_EOISR(1)]);
	printh("GICH_ELSR0: %d\r\n", GICH[GICH_ELSR(0)]);
	printh("GICH_ELSR1: %d\r\n", GICH[GICH_ELSR(1)]);
	printh("GICH_APR: %d\r\n", GICH[GICH_APR]);
}

void enable_irq(unsigned int irqn) {
	GICD[GICD_ISENABLER(irqn / 32)] = 1 << (irqn % 32);
	GICD[GICD_ITARGETSR(irqn / 4)] |= (0x01 << ((irqn % 4) * 8));
	GICD[GICD_IPRIORITYR(irqn / 4)] |= (0xa0 << ((irqn % 4) * 8));
}

void init_gic_distributor() {
	GICD[GICD_CTLR] = 0x0;	// disable GIC

	unsigned int typer = GICD[GICD_TYPER];
	unsigned int lines = 32 * ((typer & 0x1F) + 1);
	unsigned int i;
	/* set global interrupts to active low, level sensitive */
	for (i = 32; i < lines; i += 16) {
		GICD[GICD_ICFGR(i / 16)] = 0x0;
	}
	for (i = 32; i < lines; i += 16) {
		GICD[GICD_ITARGETSR(i / 4)] = 0x01010101;
	}
	for (i = 32; i < lines; i += 4) {
		GICD[GICD_IPRIORITYR(i / 4)] = 0xa0a0a0a0;
	}
	for (i = 32; i < lines; i += 32) {
		GICD[GICD_ICENABLER(i / 32)] = 0xFFFFFFFF;
	}

	GICD[GICD_CTLR] = 0x01;
}

void init_gic_cpu() {
	unsigned int i;
	GICD[GICD_ICENABLER(0)] = 0xFFFF0000;
	GICD[GICD_ISENABLER(0)] = 0x0000FFFF;
	for (i = 0; i < 32; i += 4)
		GICD[GICD_IPRIORITYR(i / 4)] = 0xa0a0a0a0;

	GICC[GICC_PMR] = 0xff;
	GICC[GICC_BPR] = 0;
	GICC[GICC_CTLR] = 0x201;
}

void init_gic() {
	initIRQHandlers();
	init_gic_distributor();
	init_gic_cpu();
	registerVirtDeviceHandler((unsigned int)GICD, vgicVirtDeviceHandler);
}

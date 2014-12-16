

#include <config.h>
#include <cpu.h>
#include <vgic.h>
#include <printh.h>
#include <gic.h>
#include <vm.h>
#include <schedule.h>

struct hfarRegs_s {
	unsigned int hsr, hdfar, hpfar;
};

void vgicHandlerDistRead(struct guestVM_s *guest, unsigned int distOffset, 
			unsigned int *destReg)
{
	printh("vgicHandlerDistRead\r\n");
	switch (distOffset) {
	default:
		printh("Read to Distributor reg %d is not implemented\r\n", distOffset);
		while (1);
		break;
	case GICD_CTLR * 4:
		*destReg = GICV(0)[GICD_CTLR] & 0x1;
		break;
	case GICD_TYPER * 4:
		*destReg = guest->vgic.typer;
		break;
	case GICD_IPRIORITYR(0)*4 ... GICD_IPRIORITYR(0x3FF)*4:
		*destReg = guest->vgic.priority[distOffset&0x3FF];
		break;
	}
}

void vgicHandlerDistWrite(struct guestVM_s *guest, unsigned int distOffset, 
			unsigned int *srcReg)
{
	printh("vgicHandlerDistWrite\r\n");
	switch (distOffset) {
	default:
		printh("Write to Distributor reg %d is not implemented\r\n", distOffset);
		while (1);
		break;
	case GICD_CTLR:
		guest->vgic.ctlr = *srcReg;
		break;
	case GICD_ICENABLER(0)*4 ... GICD_ICENABLER(0x7F)*4:
		guest->vgic.enabled[distOffset&0x7F] &= ~(*srcReg);
		break;
	case GICD_ISENABLER(0)*4 ... GICD_ISENABLER(0x7F)*4:
		printh("Enabling %d\r\n", *srcReg);
		guest->vgic.enabled[distOffset&0x7F] |= *srcReg;
		break;
	case GICD_IPRIORITYR(0)*4 ... GICD_IPRIORITYR(0x3FF)*4:
		guest->vgic.priority[distOffset&0x3FF] = *srcReg;
		break;
	case 0xC00:
	case 0xC04:
		/* ignore */
		break;
	}
}

void vgicHandlerDist(struct hfarRegs_s *hfar, struct cpuRegs_s *regs)
{
	unsigned int regInUse;
	unsigned int *ptrToReg;
	unsigned int iss = (hfar->hsr & 0x01FFFFFF);
	printh("vgicHandlerDist\r\n");
	if (iss & (1 << 24)) {
		/* valid instruction syndrome */
		regInUse = (iss & (0xF << 16)) >> 16;
		switch (regInUse) {
		default:
			printh("Invalid register\r\n");
			while(1);
			break;
		case 0 ... 12:
			ptrToReg = ((&regs->r0) + regInUse);
			break;
		case 14:
			ptrToReg = &regs->lr_svc;
			break;
		}

		if (iss & (1 << 6)) {
			vgicHandlerDistWrite(getCurrentVM(), (hfar->hdfar & 0xFFF), ptrToReg);
		} else {
			vgicHandlerDistRead(getCurrentVM(), (hfar->hdfar & 0xFFF), ptrToReg);
		}
	} else {
		printh("Fault does not hold a valid Instruction Syndrome, ignoring and bailing out\r\n");
		return;
	}
}

void vgicHandler(unsigned int hsr, unsigned int hpfar, unsigned int hdfar, 
		struct cpuRegs_s *regs)
{
	struct hfarRegs_s hfar;
	hfar.hsr = hsr;
	hfar.hpfar = hpfar;
	hfar.hdfar = hdfar;
	if (hpfar == (((unsigned int)GICD) >> 8))
		vgicHandlerDist(&hfar, regs);
}

void triggerVIRQ(unsigned int irqNum)
{
	struct guestVM_s *guest = getCurrentVM();
	unsigned int priority = (guest->vgic.priority[irqNum/4] & (irqNum % 4)) >> ((irqNum % 4) * 8);
	unsigned int listReg = 0x80000000; // assume hardware interrupt, state pending

//	if (guest->vgic.enabled[irqNum/32] & (1 << (irqNum % 32))) {
		listReg |= priority << 23;
		listReg |= irqNum;          // virtual irq ID
		listReg |= 56 << 10;    // physical irq ID

		printh("Triggering VIRQ %d (%d)\r\n", irqNum, listReg);
		GICH[GICH_LR(0)] = listReg;
//	}
}


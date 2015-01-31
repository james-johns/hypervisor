

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

void printGICHypState();


void saveVGIC(struct vgic_s *vgic)
{
	unsigned int i;
	vgic->ctlr = GICH[GICH_HCR];
	vgic->apr  = GICH[GICH_APR];
	vgic->vmcr = GICH[GICH_VMCR];
	for (i = 0; i < vgic->lr_lines; i++) {
		vgic->lr[i] = GICH[GICH_LR(i)];
	}
}


void restoreVGIC(struct vgic_s *vgic)
{
	unsigned int i;
	GICH[GICH_HCR] = vgic->ctlr;
	GICH[GICH_APR] = vgic->apr;
	GICH[GICH_VMCR] = vgic->vmcr;
	for (i = 0; i < vgic->lr_lines; i++) {
		GICH[GICH_LR(i)] = vgic->lr[i];
	}
}

void vgicHandlerDistRead(struct guestVM_s *guest, unsigned int distOffset, 
			unsigned int *destReg)
{
//	printh("vgicHandlerDistRead\r\n");
	switch (distOffset) {
	default:
		printh("Read from Distributor reg %d is not implemented\r\n", distOffset);
		while (1);
		break;
	case GICD_CTLR * 4:
		*destReg = guest->vcpu.vgic.ctlr;
		break;
	case GICD_TYPER * 4:
		*destReg = 0x00000003;//guest->vgic.typer;
		break;
	case GICD_ISENABLER(0)*4 ... GICD_ISENABLER(0x7F)*4:
		*destReg = guest->vcpu.vgic.enabled[(distOffset & 0x7F) / 4];
		break;
	case GICD_IPRIORITYR(0)*4 ... GICD_IPRIORITYR(0x3FF)*4:
		*destReg = guest->vcpu.vgic.priority[distOffset&0x3FF];
		break;
	case GICD_ITARGETSR(0)*4 ... GICD_ITARGETSR(0xF8)*4:
		*destReg = guest->vcpu.vgic.target[distOffset & 0xFF];
		break;
	case 0xC00 ... 0xDFC:
		// ignore ICFGRn
		break;
	}
}

void vgicHandlerDistWrite(struct guestVM_s *guest, unsigned int distOffset, 
			unsigned int *srcReg)
{
//	printh("vgicHandlerDistWrite\r\n");
	switch (distOffset) {
	default:
		printh("Write to Distributor reg %d is not implemented\r\n", distOffset);
		while (1);
		break;
	case GICD_CTLR:
		guest->vcpu.vgic.ctlr = *srcReg;
		break;
	case GICD_ICENABLER(0)*4 ... GICD_ICENABLER(0x7F)*4:
//		printh("Disabling %d offset %d\r\n", *srcReg, ((distOffset & 0x7F)/4));
		guest->vcpu.vgic.enabled[(distOffset & 0x7F) / 4] &= ~(*srcReg);
		break;
	case GICD_ISENABLER(0)*4 ... GICD_ISENABLER(0x7F)*4:
//		printh("Enabling %d offset %d\r\n", *srcReg, ((distOffset & 0x7F)/4));
		guest->vcpu.vgic.enabled[(distOffset & 0x7F) / 4] |= *srcReg;
		GICD[GICD_ISENABLER((distOffset & 0x7F))] = *srcReg;
//		GICD[GICD_ITARGETSR(irqn / 4)] |= (0x01 << ((irqn % 4) * 8));
//		GICD[GICD_IPRIORITYR(irqn / 4)] |= (0xa0 << ((irqn % 4) * 8));
		break;
	case GICD_IPRIORITYR(0)*4 ... GICD_IPRIORITYR(0x3FF)*4:
//		printh("Priority(%d) %d\r\n", distOffset&0x3FF, *srcReg);
		guest->vcpu.vgic.priority[distOffset&0x3FF] = *srcReg;
		break;
	case GICD_ITARGETSR(0)*4 ... GICD_ITARGETSR(0xF8)*4:
//		printh("ITARGETSR set to %d\r\n", *srcReg);
		guest->vcpu.vgic.target[distOffset & 0xFF] = *srcReg;
		break;
	case GICD_ICFGR(0)*4 ... GICD_ICFGR(0xFC)*4:
		/* ignore */
//		printh("ICFGR set to %d\r\n", *srcReg);
		break;
	}
}

void vgicHandlerDist(struct hfarRegs_s *hfar, struct cpuRegs_s *regs)
{
	unsigned int regInUse;
	unsigned int *ptrToReg;
	unsigned int iss = (hfar->hsr & 0x01FFFFFF);
//	printh("vgicHandlerDist\r\n");
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
//	printGICHypState();
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

void vgicVirtDeviceHandler(struct cpuRegs_s *regs)
{
	unsigned int hdfar, hpfar, hsr;
	asm volatile("mrc p15, 4, %0, c5, c2, 0":"=r"(hsr):);
	asm volatile("mrc p15, 4, %0, c6, c0, 0":"=r"(hdfar):);
	asm volatile("mrc p15, 4, %0, c6, c0, 4":"=r"(hpfar):);
	vgicHandler(hsr, hpfar, hdfar, regs);
}

void triggerVIRQ(unsigned int irqNum)
{
	struct guestVM_s *guest = getCurrentVM();
	unsigned int priority;
	unsigned int listReg;

	if (guest->vcpu.vgic.enabled[irqNum/32] & (1 << (irqNum % 32))) {
		priority = (guest->vcpu.vgic.priority[irqNum/4] & (irqNum % 4)) >> ((irqNum % 4) * 8);

		listReg = 0x90000000;               // assume hardware interrupt, state pending
		listReg |= (priority & 0xFF) << 23;
		listReg |= irqNum;                  // virtual irq ID
		listReg |= irqNum << 10;            // physical irq ID

//	        printh("Triggering VIRQ %d (%d)\r\n", irqNum, listReg);
		GICH[GICH_LR(0)] = listReg;
	} else {
		//printh("Not triggering VIRQ because vgic.enabled[%d] = %d\r\n", irqNum/32, guest->vgic.enabled[irqNum/32]);
	}
}


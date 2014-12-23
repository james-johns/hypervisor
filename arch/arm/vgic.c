/**
 * \file
 * \author James Johns
 */

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

/**
 * \fn setVGIC(struct vgic_s *vgic)
 *
 * Restore VGIC state
 */
void setVGIC(struct vgic_s *vgic)
{
	GICH[GICH_HCR] = vgic->ctlr;
}

/**
 * \fn vgicHandlerDistRead(struct guestVM_s *guest, unsigned int distOffset, unsigned int *destReg)
 *
 * Handle read of VGIC Distributor virtual device.
 */
void vgicHandlerDistRead(struct guestVM_s *guest, unsigned int distOffset, 
			unsigned int *destReg)
{
//	printh("vgicHandlerDistRead\r\n");
	switch (distOffset) {
	default:
		printh("Read to Distributor reg %d is not implemented\r\n", distOffset);
		while (1);
		break;
	case GICD_CTLR * 4:
		*destReg = guest->vgic.ctlr;
		break;
	case GICD_TYPER * 4:
		*destReg = 0x00000003;//guest->vgic.typer;
		break;
	case GICD_IPRIORITYR(0)*4 ... GICD_IPRIORITYR(0x3FF)*4:
		*destReg = guest->vgic.priority[distOffset&0x3FF];
		break;
	case GICD_ITARGETSR(0)*4 ... GICD_ITARGETSR(0xF8)*4:
		*destReg = guest->vgic.target[distOffset & 0xFF];
		break;
	}
}

/**
 * \fn vgicHandlerDistWrite(struct guestVM_s *guest, unsigned int distOffset, unsigned int *srcReg)
 *
 * Handle write to VGIC Distributor virtual device.
 */
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
		guest->vgic.ctlr = *srcReg;
		break;
	case GICD_ICENABLER(0)*4 ... GICD_ICENABLER(0x7F)*4:
//		printh("Disabling %d offset %d\r\n", *srcReg, ((distOffset & 0x7F)/4));
		guest->vgic.enabled[(distOffset & 0x7F) / 4] &= ~(*srcReg);
		break;
	case GICD_ISENABLER(0)*4 ... GICD_ISENABLER(0x7F)*4:
//		printh("Enabling %d offset %d\r\n", *srcReg, ((distOffset & 0x7F)/4));
		guest->vgic.enabled[(distOffset & 0x7F) / 4] |= *srcReg;
		break;
	case GICD_IPRIORITYR(0)*4 ... GICD_IPRIORITYR(0x3FF)*4:
//		printh("Priority(%d) %d\r\n", distOffset&0x3FF, *srcReg);
		guest->vgic.priority[distOffset&0x3FF] = *srcReg;
		break;
	case GICD_ITARGETSR(0)*4 ... GICD_ITARGETSR(0xF8)*4:
//		printh("ITARGETSR set to %d\r\n", *srcReg);
		guest->vgic.target[distOffset & 0xFF] = *srcReg;
		break;
	case GICD_ICFGR(0)*4 ... GICD_ICFGR(0xFC)*4:
		/* ignore */
//		printh("ICFGR set to %d\r\n", *srcReg);
		break;
	}
}

/**
 * \fn vgicHandlerDist(struct hfarRegs_s *hfar, struct cpuRegs_s *regs)
 *
 * Handle VGIC Distributor virtual device access.
 */
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

/**
 * \fn vgicVirtDeviceHandler(struct cpuRegs_s *regs)
 *
 * Handle VGIC virtual device access
 */
void vgicVirtDeviceHandler(struct cpuRegs_s *regs)
{
	unsigned int hdfar, hpfar, hsr;
	asm volatile("mrc p15, 4, %0, c5, c2, 0":"=r"(hsr):);
	asm volatile("mrc p15, 4, %0, c6, c0, 0":"=r"(hdfar):);
	asm volatile("mrc p15, 4, %0, c6, c0, 4":"=r"(hpfar):);
	struct hfarRegs_s hfar;
	hfar.hsr = hsr;
	hfar.hpfar = hpfar;
	hfar.hdfar = hdfar;
	if (hpfar == (((unsigned int)GICD) >> 8))
		vgicHandlerDist(&hfar, regs);
}

/**
 * \fn triggerVIRQ(unsigned int irqNum)
 *
 * Send virtual IRQ to VM via GICH interface
 */
void triggerVIRQ(unsigned int irqNum)
{
	struct guestVM_s *guest = getCurrentVM();
	unsigned int priority;
	unsigned int listReg;

	if (guest->vgic.enabled[irqNum/32] & (1 << (irqNum % 32))) {
		priority = (guest->vgic.priority[irqNum/4] & (irqNum % 4)) >> ((irqNum % 4) * 8);

		listReg = 0x90000000;               // assume hardware interrupt, state pending
		listReg |= (priority & 0xFF) << 23;
		listReg |= irqNum;                  // virtual irq ID
		listReg |= 56 << 10;            // physical irq ID

//	        printh("Triggering VIRQ %d (%d)\r\n", irqNum, listReg);
		GICH[GICH_LR(0)] = listReg;
	} else {
		printh("Not triggering VIRQ because vgic.enabled[%d] = %d\r\n", irqNum/32, guest->vgic.enabled[irqNum/32]);
	}
}


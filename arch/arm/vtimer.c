

#include <vtimer.h>
#include <cpu.h>
#include <printh.h>

#define TIMER_BASE       ((unsigned int *)0x01C20C00)
#define TIMER_IRQ_EN     *(TIMER_BASE+0x0)
#define TIMER_IRQ_STATUS *(TIMER_BASE+(0x04/4))

#define TIMERx_BASE(x)   (TIMER_BASE + ((0x10 * (x + 1)) / 4))
#define TIMER_CTRL       (0x00)
#define TIMER_INTRVL     (0x04 / 4)
#define TIMER_CURVL      (0x08 / 4)

#define TIMER2_OFF       (TIMER_BASE+(0x30/4))
#define TIMER2_CTRL      *(TIMER2_OFF)
#define TIMER2_INTRVL    *(TIMER2_OFF+(0x04/4))
#define TIMER2_CUR_INTRVL *((unsigned int *)(TIMER2_OFF+(0x08/4)))

struct vtimerx_s vtimer[4];

struct vtimer_s vtimerctrl;

void vtimerHandlerRead(unsigned int hdfar, unsigned int hpfar, unsigned int *destReg)
{
//	printh("vtimerHandlerRead\r\n");
	unsigned int timer = hdfar & 0x70;
	unsigned int offset = hdfar & 0x0F;
	if ((hdfar & 0xF00) == 0xC00) {
	  //		printh("Reading timer\r\n");
		if (timer == 0) {
			switch (offset & 0x0F) {
			case 0x4:
//			printh("TIMER_STATUS %d\r\n", vtimerctrl.status);
				*destReg = vtimerctrl.status;
				break;
			default:
				*destReg = 0;
				break;
			}
		} else {
			switch (offset & 0x0F) {
			case TIMER_CTRL:
//			printh("TIMER_CTRL %d\r\n", timer);
				*destReg = vtimer[timer].ctrl;
				break;
			case TIMER_INTRVL:
//			printh("TIMER_INTRVL %d\r\n", timer);
				*destReg = vtimer[timer].intrvl;
				break;
			case TIMER_CURVL:
//			printh("TIMER_CURVL %d\r\n", timer);
				*destReg = vtimer[timer].curval;
				break;
			}
		}
	} else {
		unsigned int ptr = (hpfar << 8) | (hdfar & 0xFFF);
		*destReg = *(unsigned int *)ptr;
		//		printh("Reading ptr %d = %d\r\n", ptr, *destReg);
	}
}

void vtimerHandlerWrite(unsigned int hdfar, unsigned int hpfar, unsigned int *srcReg)
{
//	printh("vtimerHandlerWrite\r\n");
	unsigned int timer = hdfar & 0x70;
	unsigned int offset = hdfar & 0x0F;
	if ((hdfar & 0xF00) == 0xC00) {
	  //printh("Writing timer\r\n");
		if (timer == 0) {
			switch (offset & 0x0F) {
			case 0x4:
//			printh("TIMER_STATUS %d\r\n", *srcReg);
				vtimerctrl.status = *srcReg;
				break;
			default:
				break;
			}
		} else {
			switch (offset & 0x0F) {
			case TIMER_CTRL:
//			printh("TIMER_CTRL %d\r\n", *srcReg);
				vtimer[timer].ctrl = *srcReg;
				break;
			case TIMER_INTRVL:
//			printh("TIMER_INTRVL %d\r\n", *srcReg);
				vtimer[timer].intrvl = *srcReg;
				break;
			case TIMER_CURVL:
//			printh("TIMER_CURVL %d\r\n", *srcReg);
				vtimer[timer].curval = *srcReg;
				break;
			}
		}
	} else {
		unsigned int ptr = (hpfar << 8) | (hdfar & 0xFFF);
		//		printh("Writing ptr %d = %d\r\n", ptr, *srcReg);
       		*((unsigned int *)ptr) = *srcReg;
	}
}

void vtimerHandler(unsigned int hsr, unsigned int hpfar, unsigned int hdfar, 
		struct cpuRegs_s *regs)
{
	unsigned int regInUse;
	unsigned int *ptrToReg = (unsigned int *)hpfar;
	unsigned int iss = (hsr & 0x01FFFFFF);
//	printh("vtimerHandler\r\n");
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
			/* write */
			vtimerHandlerWrite(hdfar, hpfar, ptrToReg);
		} else {
			/* read */
			vtimerHandlerRead(hdfar, hpfar, ptrToReg);
		}
	} else {
		printh("Fault does not hold a valid Instruction Syndrome, ignoring and bailing out\r\n");
		return;
	}
}

void print_regs(struct cpuRegs_s *regs);

void vtimerVirtDeviceHandler(struct cpuRegs_s *regs)
{
	unsigned int hdfar, hpfar, hsr;
	asm volatile("mrc p15, 4, %0, c5, c2, 0":"=r"(hsr):);
	asm volatile("mrc p15, 4, %0, c6, c0, 0":"=r"(hdfar):);
	asm volatile("mrc p15, 4, %0, c6, c0, 4":"=r"(hpfar):);
	vtimerHandler(hsr, hpfar, hdfar, regs);
}

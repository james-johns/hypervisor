
#include <cpu.h>
#include <printh.h>
#include <schedule.h>
#include <virtdevice.h>
#include <vtty.h>


void vttyHandlerWrite(unsigned int off, unsigned int *srcReg)
{
//	printh("vttyHandlerWrite\r\n");
	struct guestVM_s *vm = getCurrentVM();
	struct vtty_s vtty = vm->vcpu.vtty;

	switch (off) {
	case 0:
		srcReg = srcReg;
		vtty.buffer[vtty.end++] = *srcReg;
		if (*srcReg == '\n') {
			vtty.buffer[vtty.end] = '\0';
			vtty.end = 0;
		} else if (vtty.end >= (VTTY_BUFFER_SIZE - 1)) {
			vtty.buffer[VTTY_BUFFER_SIZE - 1] = '\0';
			vtty.end = 0;
		}
//		*((unsigned int *)0x01c28000) = *srcReg;
		break;
	default:
		break;
	}

	if (vtty.end == 0 && vtty.buffer[0] != '\0') {
		printh("\r[%s] %s", vm->name, &vtty.buffer[1]);
		vtty.buffer[0] = '\0';
	}
	
}

void vttyHandlerRead(unsigned int off, unsigned int *destReg)
{
	switch (off) {
/*	case 0:
		printh("0\r\n");
		*destReg = *((unsigned int *)0x01c28000);
		break;
	case 4:
		printh("4\r\n");
		*destReg = *((unsigned int *)0x01c28004);
		break;
	case 8:
		printh("8\r\n");
		*destReg = *((unsigned int *)0x01c28008);
		break;
	case 12:
		printh("12\r\n");
		*destReg = *((unsigned int *)0x01c2800C);
		break;*/
	default:
//		printh("default %d\r\n", off);
		*destReg = *((unsigned int *)(0x01c28000 + off));
		break;
	}
}

void print_regs(struct cpuRegs_s *regs);

void vttyHandler(struct cpuRegs_s *regs)
{
//	printh("vtty\r\n");
//	print_regs(regs);
	unsigned int hdfar, hpfar, hsr;
	asm volatile("mrc p15, 4, %0, c5, c2, 0":"=r"(hsr):);
	asm volatile("mrc p15, 4, %0, c6, c0, 0":"=r"(hdfar):);
	asm volatile("mrc p15, 4, %0, c6, c0, 4":"=r"(hpfar):);
	unsigned int iss = (hsr & 0x01FFFFFF);
	unsigned int regInUse, *ptrToReg;
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

		if (hsr & (1 << 6))
			vttyHandlerWrite((hdfar & 0xFF), ptrToReg);
		else
			vttyHandlerRead((hdfar & 0xFF), ptrToReg);
	}
}

void vttyInit(unsigned int *address)
{
	registerVirtDeviceHandler((unsigned int)address, vttyHandler);
	address[0] = 0x00;
	address[1] = 0x00;
	address[2] = 0x01;
	address[3] = 0x00;
	address[4] = 0x00;
	address[5] = 0x60;
	address[6] = 0x00;
	address[7] = 0x00;
	address[19] = 0x06;
	address[20] = 0x00;
	address[21] = 0x00;
}


#include <config.h>
#include <printh.h>
#include <gic.h>
#include <vgic.h>
#include <cpu.h>
#include <memory.h>

void install_hyp_vectors();
void timer_interrupt();

void init_irqs()
{
	install_hyp_vectors();

	init_gic();

	printh("HCR %d\r\n", getHCR());
	setHCR(0x00000038);
}

void print_regs(struct cpuRegs_s *regs)
{
	print_str("\r\nR0 : ");
	print_hex(regs->r0);
	print_str("\r\nR1 : ");
	print_hex(regs->r1);
	print_str("\r\nR2 : ");
	print_hex(regs->r2);
	print_str("\r\nR3 : ");
	print_hex(regs->r3);
	print_str("\r\nR4 : ");
	print_hex(regs->r4);
	print_str("\r\nR5 : ");
	print_hex(regs->r5);
	print_str("\r\nR6 : ");
	print_hex(regs->r6);
	print_str("\r\nR7 : ");
	print_hex(regs->r7);
	print_str("\r\nR8 : ");
	print_hex(regs->r8);
	print_str("\r\nR9 : ");
	print_hex(regs->r9);
	print_str("\r\nR10: ");
	print_hex(regs->r10);
	print_str("\r\nR11: ");
	print_hex(regs->r11);
	print_str("\r\nR12: ");
	print_hex(regs->r12);
	print_str("\r\nPC : ");
	print_hex(regs->pc);
	print_str("\r\nPSR: ");
	print_hex(regs->cpsr);
	print_str("\r\nlr: ");
	print_hex(regs->lr);
	print_str("\r\nSP_usr: ");
	print_hex(regs->SP_usr);
	print_str("\r\nlr_svc: ");
	print_hex(regs->lr_svc);
	print_str("\r\n\r\n");
}

void handle_trap_undefined_instruction(struct cpuRegs_s *regs)
{
	print_str("\r\nUndefine Instruction Trap");
	print_regs(regs);
}

void handle_trap_supervisor_call(struct cpuRegs_s *regs)
{
	print_str("\r\nSupervisor Call Trap");
	print_regs(regs);
}

void handle_trap_prefetch_abort(struct cpuRegs_s *regs)
{
	print_str("\r\nPrefetch Abort Trap");
	print_regs(regs);
}

void handle_trap_data_abort(struct cpuRegs_s *regs)
{
	print_str("\r\nData Abort Trap");
	print_regs(regs);
}

void handle_trap_hyp_call(struct cpuRegs_s *regs)
{
	unsigned int hpfar, hifar, hdfar, hsr;
	print_str("\r\nHYP Call Trap\r\n");
	asm volatile("mrc p15, 4, %0, c5, c2, 0":"=r"(hsr):);

	asm volatile("mrc p15, 4, %0, c6, c0, 0":"=r"(hdfar):);
	asm volatile("mrc p15, 4, %0, c6, c0, 2":"=r"(hifar):);
	asm volatile("mrc p15, 4, %0, c6, c0, 4":"=r"(hpfar):);

	if ((hsr & 0xFC000000)) {
		printh("EC: %d,     IL: %d,     ISS: %d\r\n", ((hsr & 0xFC000000) >> 26), ((hsr & 0x02000000) >> 25), (hsr & 0x01FFFFFF));
		printh("HPFAR (%d)\r\n", hpfar);
		printh("HIFAR (%d)\r\n", hifar);
		printh("HDFAR (%d)\r\n", hdfar);
		if (hpfar == (((unsigned int)GICD) >> 8)) {
			printh("calling vgicHandler\r\n");
			vgicHandler(hsr, hpfar, hdfar, regs);
		}
		regs->pc += 4;
	} else {
		print_regs(regs);
	}
}

void handle_trap_irq(struct cpuRegs_s *regs)
{
	unsigned int interrupt = GICC[GICC_IAR];
	if (interrupt == 0x38) {
		timer_interrupt(regs);
	} else {
		print_str("\r\nIRQ Trap");
		print_hex(interrupt);
		print_regs(regs);
	}
	GICC[GICC_DIR] = interrupt;	// deactivate interrupt to reset priority
	GICC[GICC_EOIR] = interrupt;	// end of interrupt
}

void handle_trap_fiq(struct cpuRegs_s *regs)
{
	print_str("\r\nFIQ Trap");
	print_regs(regs);
}

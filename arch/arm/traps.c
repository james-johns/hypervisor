
#include <config.h>
#include <printh.h>
#include <gic.h>
#include <cpu.h>

void install_hyp_vectors();
void timer_interrupt();

void init_irqs()
{
	install_hyp_vectors();

	init_gic();
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
	print_str("\r\nHYP Call Trap");
	print_regs(regs);
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

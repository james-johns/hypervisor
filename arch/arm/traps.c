

#define GIC_BASE 0x01c80000

#include <printh.h>
#include <gic.h>

void install_hyp_vectors();

void init_irqs() {

  install_hyp_vectors();

  init_gic();
}

struct cpu_regs {
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
};

void print_regs(struct cpu_regs *regs) {
  print_str("\r\nR0 : "); print_hex(regs->r0);
  print_str("\r\nR1 : "); print_hex(regs->r1);
  print_str("\r\nR2 : "); print_hex(regs->r2);
  print_str("\r\nR3 : "); print_hex(regs->r3);
  print_str("\r\nR4 : "); print_hex(regs->r4);
  print_str("\r\nR5 : "); print_hex(regs->r5);
  print_str("\r\nR6 : "); print_hex(regs->r6);
  print_str("\r\nR7 : "); print_hex(regs->r7);
  print_str("\r\nR8 : "); print_hex(regs->r8);
  print_str("\r\nR9 : "); print_hex(regs->r9);
  print_str("\r\nR10: "); print_hex(regs->r10);
  print_str("\r\nR11: "); print_hex(regs->r11);
  print_str("\r\nR12: "); print_hex(regs->r12);
  print_str("\r\nPC : "); print_hex(regs->pc);
  print_str("\r\nPSR: "); print_hex(regs->cpsr);
  print_str("\r\n\r\n");
}

void handle_trap_undefined_instruction(struct cpu_regs *regs) {
  print_str("\r\nUndefine Instruction Trap");
  print_regs(regs);
}

void handle_trap_supervisor_call(struct cpu_regs *regs) {
  print_str("\r\nSupervisor Call Trap");
  print_regs(regs);
}

void handle_trap_prefetch_abort(struct cpu_regs *regs) {
  print_str("\r\nPrefetch Abort Trap");
  print_regs(regs);
}

void handle_trap_data_abort(struct cpu_regs *regs) {
  print_str("\r\nData Abort Trap");
  print_regs(regs);
}

void handle_trap_hyp_call(struct cpu_regs *regs) {
  print_str("\r\nHYP Call Trap");
  print_regs(regs);
}

void handle_trap_irq(struct cpu_regs *regs) {
  print_str("\r\nIRQ Trap");
  unsigned int interrupt = GICC[GICC_IAR];
  print_hex(interrupt);
  GICC[GICC_EOIR] = interrupt;
  print_regs(regs);
}

void handle_trap_fiq(struct cpu_regs *regs) {
  print_str("\r\nFIQ Trap");
  print_regs(regs);
}

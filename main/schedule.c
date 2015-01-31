

#include <cpu.h>
#include <vm.h>
#include <schedule.h>
#include <memory.h>
#include <printh.h>
#include <vgic.h>

void print_regs(struct cpuRegs_s *regs);

struct guestVM_s *virtualMachines[4];
signed int allocatedVMs;
signed int currentVMID;

struct guestVM_s *getCurrentVM()
{
	if (currentVMID < 0)
		return ((void *)0);
	return virtualMachines[currentVMID];
}

void scheduleVM(struct guestVM_s *guest)
{
	printh("Scheduling vm %d %d ", allocatedVMs, guest);
	virtualMachines[allocatedVMs] = guest;
	printh("%d\r\n", virtualMachines[allocatedVMs]);
	allocatedVMs++;
}

void switchToVM(struct guestVM_s *nextVM, struct cpuRegs_s *regs)
{
	struct cpu_s *vcpu = &nextVM->vcpu;
	memcpy((void *)&vcpu->regs, (void *)regs, sizeof(struct cpuRegs_s));
	setVTCR(0x80003540);// 0x80003540
	restoreVGIC(&vcpu->vgic);
	setHCR(getHCR() | 0x01);
	setGuestTTBR((unsigned int)vcpu->stageOneTable, currentVMID << 16);
	asm volatile("mcrr p15, 0, %0, %1, c2"::
		"r"(vcpu->coproc15.ttbr0_lo), "r"(vcpu->coproc15.ttbr0_hi));
	asm volatile("mcrr p15, 1, %0, %1, c2"::
		"r"(vcpu->coproc15.ttbr1_lo), "r"(vcpu->coproc15.ttbr1_hi));
	asm volatile("mcr p15, 0, %0, c2, c0, 0" ::"r"(vcpu->coproc15.ttbcr));
	asm volatile("mcr p15, 0, %0, c10, c2, 0"::"r"(vcpu->coproc15.mair0));
	asm volatile("mcr p15, 0, %0, c10, c2, 1"::"r"(vcpu->coproc15.mair1));
	asm volatile("mcr p15, 0, %0, c10, c3, 0"::"r"(vcpu->coproc15.amair0));
	asm volatile("mcr p15, 0, %0, c10, c3, 1"::"r"(vcpu->coproc15.amair1));
	asm volatile("mcr p15, 0, %0, c3, c0, 0" ::"r"(vcpu->coproc15.dacr));
	asm volatile("mcrr p15, 0, %0, %1, c7"::
		"r"(vcpu->coproc15.par_lo), "r"(vcpu->coproc15.par_hi));

	asm volatile("mcr p15, 0, %0, c12, c0, 0"::"r"(vcpu->coproc15.vbar));
	asm volatile("mcr p15, 0, %0, c1, c0, 0"::"r"(vcpu->coproc15.sctlr));
	asm volatile("mcr p15, 4, %0, c0, c0, 5 \n isb"::"r"(0xC0000000));
	asm volatile("dsb \n mcr p15, 0, %0, c8, c7, 0\n"::"r"(0x0));
}

void saveVMState(struct cpuRegs_s *regs, struct guestVM_s* guest)
{
	if (currentVMID >= 0) {
		memcpy((void *)regs, (void *)&guest->vcpu.regs, sizeof(struct cpuRegs_s));
		guest = guest;
		struct cpu_s *vcpu = &guest->vcpu;
		saveVGIC(&vcpu->vgic);
		asm volatile("isb \n mrrc p15, 0, %0, %1, c2":
			"=r"(vcpu->coproc15.ttbr0_lo), "=r"(vcpu->coproc15.ttbr0_hi):);
		asm volatile("mrrc p15, 1, %0, %1, c2":
			"=r"(vcpu->coproc15.ttbr1_lo), "=r"(vcpu->coproc15.ttbr1_hi):);
		asm volatile("mrc p15, 0, %0, c2, c0, 0" :"=r"(vcpu->coproc15.ttbcr):);
		asm volatile("mrc p15, 0, %0, c10, c2, 0":"=r"(vcpu->coproc15.mair0):);
		asm volatile("mrc p15, 0, %0, c10, c2, 1":"=r"(vcpu->coproc15.mair1):);
		asm volatile("mrc p15, 0, %0, c10, c3, 0":"=r"(vcpu->coproc15.amair0):);
		asm volatile("mrc p15, 0, %0, c10, c3, 1":"=r"(vcpu->coproc15.amair1):);
		asm volatile("mrc p15, 0, %0, c3, c0, 0" :"=r"(vcpu->coproc15.dacr):);
		asm volatile("mrrc p15, 0, %0, %1, c7":
			"=r"(vcpu->coproc15.par_lo), "=r"(vcpu->coproc15.par_hi):);

		asm volatile("mrc p15, 0, %0, c1, c0, 0":"=r"(vcpu->coproc15.sctlr):);
		asm volatile("mrc p15, 0, %0, c12, c0, 0":"=r"(vcpu->coproc15.vbar):);
		
	}
}

void schedule(struct cpuRegs_s *regs)
{
//	printh("Scheduling\r\n");
	if (allocatedVMs > 0) {
//		print_regs(regs);
		saveVMState(regs, getCurrentVM());
		currentVMID++;
		if (currentVMID >= allocatedVMs)
			currentVMID = 0;
		switchToVM(virtualMachines[currentVMID], regs);
//		print_regs(regs);
	}
}

void init_scheduler()
{
	allocatedVMs = 0;
	currentVMID = -1;
}

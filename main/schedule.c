

#include <cpu.h>
#include <vm.h>
#include <schedule.h>
#include <memory.h>
#include <printh.h>

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
	memcpy((void *)&nextVM->regs, (void *)regs, sizeof(struct cpuRegs_s));
	setVTCR(0x80003540);
	setGuestTTBR((unsigned int)nextVM->stageOneTable);
	setHCR(getHCR() | 0x01);
}

void saveVMState(struct cpuRegs_s *regs)
{
	if (currentVMID >= 0) {
		memcpy((void *)regs, (void *)&virtualMachines[currentVMID]->regs, sizeof(struct cpuRegs_s));
	}
}

void schedule(struct cpuRegs_s *regs)
{
	if (allocatedVMs > 0) {
		saveVMState(regs);
		currentVMID++;
		if (currentVMID >= allocatedVMs)
			currentVMID = 0;
		switchToVM(virtualMachines[currentVMID], regs);
	}
}

void init_scheduler()
{
	allocatedVMs = 0;
	currentVMID = -1;
}

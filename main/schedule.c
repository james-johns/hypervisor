/**
 * \file
 * \author James Johns
 */

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

/**
 * \fn getCurrentVM
 *
 * Return the current scheduled VM state structure
 */
struct guestVM_s *getCurrentVM()
{
	if (currentVMID < 0)
		return ((void *)0);
	return virtualMachines[currentVMID];
}

/**
 * \fn scheduleVM(struct guestVM_s *guest)
 *
 * Add new guestVM to schedule queue.
 */
void scheduleVM(struct guestVM_s *guest)
{
	printh("Scheduling vm %d %d ", allocatedVMs, guest);
	virtualMachines[allocatedVMs] = guest;
	printh("%d\r\n", virtualMachines[allocatedVMs]);
	allocatedVMs++;
}

/**
 * \fn switchToVM(struct guestVM_s *nextVM, struct cpuRegs_s *regs)
 *
 * Switch current executing VM to nextVM.
 */
void switchToVM(struct guestVM_s *nextVM, struct cpuRegs_s *regs)
{
	memcpy((void *)&nextVM->regs, (void *)regs, sizeof(struct cpuRegs_s));
	setVTCR(0x80003540);
	setGuestTTBR((unsigned int)nextVM->stageOneTable);
	setHCR(getHCR() | 0x01);
	setVGIC(&nextVM->vgic);
	asm volatile("mcr p15, 4, %0, c0, c0, 5"::"r"(0xC0000000));
}

/**
 * \fn saveVMState(struct cpuRegs_s *regs)
 *
 * Store current VM state into current VM storage struct
 */
void saveVMState(struct cpuRegs_s *regs)
{
	if (currentVMID >= 0) {
		memcpy((void *)regs, (void *)&virtualMachines[currentVMID]->regs, sizeof(struct cpuRegs_s));
	}
}

/**
 * \fn schedule(struct cpuRegs_s *regs)
 *
 * Save and switch the current executing VM if appropriate
 */
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

/**
 * \fn init_scheduler
 *
 * Initialise scheduler subsystem
 */
void init_scheduler()
{
	allocatedVMs = 0;
	currentVMID = -1;
}

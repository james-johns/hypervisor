

#include <cpu.h>
#include <vm.h>
#include <schedule.h>
#include <memory.h>
#include <printh.h>

void print_regs(struct cpuRegs_s *regs);

struct guestVM_s *nextScheduledVM;
struct guestVM_s *currentVM;

void scheduleVM(struct guestVM_s *guest)
{
	nextScheduledVM = guest;
}

void switchToVM(struct guestVM_s *nextVM, struct cpuRegs_s *regs)
{
//	printh("Switching to %d\r\n", nextVM);
	*regs = nextVM->regs;
	setVTCR(0x80003540);
	setGuestTTBR((unsigned int)nextVM->stageOneTable);
	currentVM = nextVM;
}

void saveVMState(struct cpuRegs_s *regs)
{
	if (currentVM != ((void *) 0)) {
//		printh("CurrentVM (%d)\r\n", currentVM);
		currentVM->regs = (*regs);
		/* move VM onto scheduler queue */
	}
}

void schedule(struct cpuRegs_s *regs)
{
	if (nextScheduledVM != ((void *) 0)) {
		printh("Before:\r\n");
		print_regs(regs);
		saveVMState(regs);
		switchToVM(nextScheduledVM, regs);
		printh("After:\r\n");
		print_regs(regs);
		setHCR(getHCR() | 0x01);
		nextScheduledVM = ((void *)0);
	}
}

void init_scheduler()
{
	nextScheduledVM = ((void *)0);
	currentVM = ((void *)0);
}

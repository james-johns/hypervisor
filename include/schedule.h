

#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#include <cpu.h>
#include <vm.h>

void scheduleVM(struct guestVM_s *guest);
void schedule(struct cpuRegs_s *regs);
struct guestVM_s *getCurrentVM();

void init_scheduler();

struct guestVM_s *nextScheduledGuest;

#endif


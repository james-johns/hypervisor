

#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#include <cpu.h>

void scheduleVM(struct guestVM_s *guest);
void schedule(struct cpuRegs_s *regs);

void init_scheduler();

struct guestVM_s *nextScheduledGuest;

#endif


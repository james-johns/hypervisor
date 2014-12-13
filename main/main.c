
#include <config.h>
#include <tests.h>
#include <printh.h>
#include <gic.h>
#include <memory.h>
#include <vm.h>
#include <schedule.h>

void init_irqs();
void init_timer();

extern struct guestVM_s *nextScheduledVM;

void hyp_main()
{
	_end = (unsigned int) &_end;
	print_str("Starting hyp main\r\n");

	init_irqs();
	init_scheduler();
	init_timer();

	init_mmu();

	struct guestVM_s *guest = createVM(0x48000000, 0x08000000);

	printh("Created guest (%d)\r\n", guest);

	test_hypervisor();

	printh("Running guest\r\n");

	scheduleVM(guest);

	print_str("\r\nHalting!");
	while (1) {
		//    asm volatile("wfe"::);
	}
}


#include <config.h>
#include <tests.h>
#include <printh.h>
#include <gic.h>
#include <memory.h>

void init_irqs();
void init_timer();

void hyp_main()
{
	_end = (unsigned int) &_end;
	print_str("Starting hyp main\r\n");

	init_irqs();
	init_timer();

	init_mmu();

	test_hypervisor();
	print_str("\r\nHalting!");
	while (1) {
		//    asm volatile("wfe"::);
	}
}

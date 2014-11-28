
#define GIC_BASE 0x01c80000
#include <printh.h>
#include <gic.h>
#include <memory.h>

void init_irqs();
void init_timer();

void hyp_main()
{

	print_str("Starting hyp main\r\n");

	init_irqs();
	init_timer();

	init_mmu();

	print_str("\r\nHalting!");
	while (1) {
		//    asm volatile("wfe"::);
	}
}

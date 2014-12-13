
#include <config.h>
#include <gic.h>
#include <printh.h>
#include <vm.h>
#include <schedule.h>


#define TIMER_BASE       ((unsigned int *)0x01C20C00)
#define TIMER_IRQ_EN     *(TIMER_BASE+0x0)
#define TIMER_IRQ_STATUS *(TIMER_BASE+(0x04/4))

#define TIMER2_OFF       (TIMER_BASE+(0x30/4))
#define TIMER2_CTRL      *(TIMER2_OFF)
#define TIMER2_INTRVL    *(TIMER2_OFF+(0x04/4))
#define TIMER2_CUR_INTRVL *((unsigned int *)(TIMER2_OFF+(0x08/4)))

void init_timer()
{

	TIMER2_INTRVL = 0x00004000;
	TIMER2_CTRL = 0x00000007;

	TIMER_IRQ_EN = 0x04;

	enable_irq(56);
}

void print_timer_value()
{
	print_str("\r\nTimer value:");
	print_hex(TIMER2_CUR_INTRVL);
	print_str("      Status: ");
	print_hex(TIMER_IRQ_STATUS);
	print_str("\r\nTimer IRQ en:");
	print_hex(TIMER_IRQ_EN);
}

void timer_interrupt(struct cpuRegs_s *regs)
{
	TIMER_IRQ_STATUS |= 0x04;
	TIMER_IRQ_EN = 0x04;
	TIMER2_CTRL |= 0x01;
//	print_str("Tick\r\n");
	schedule(regs);
}

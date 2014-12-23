/**
 * \file
 * \author James Johns
 */

#include <config.h>
#include <gic.h>
#include <printh.h>
#include <vm.h>
#include <schedule.h>
#include <vgic.h>
#include <vtimer.h>
#include <irq.h>
#include <virtdevice.h>


#define TIMER_BASE       ((unsigned int *)0x01C20C00)
#define TIMER_IRQ_EN     *(TIMER_BASE+0x0)
#define TIMER_IRQ_STATUS *(TIMER_BASE+(0x04/4))

#define TIMERx_BASE(x)   (TIMER_BASE + ((0x10 * (x + 1)) / 4))
#define TIMER_CTRL       (0x00)
#define TIMER_INTRVL     (0x04 / 4)
#define TIMER_CURVL      (0x08 / 4)

#define TIMER2_OFF       (TIMER_BASE+(0x30/4))
#define TIMER2_CTRL      *(TIMER2_OFF)
#define TIMER2_INTRVL    *(TIMER2_OFF+(0x04/4))
#define TIMER2_CUR_INTRVL *((unsigned int *)(TIMER2_OFF+(0x08/4)))
void timer_interrupt(struct cpuRegs_s *regs);

void vtimerVirtDeviceHandler(struct cpuRegs_s *regs);

/**
 * \fn init_timer
 *
 * Initialise system timer
 */
void init_timer()
{
	TIMERx_BASE(0)[TIMER_INTRVL] = 0x00400000;
	TIMERx_BASE(0)[TIMER_CTRL] = 0x07;
	TIMERx_BASE(1)[TIMER_INTRVL] = 0x00400000;
	TIMERx_BASE(1)[TIMER_CTRL] = 0x07;
	TIMERx_BASE(2)[TIMER_INTRVL] = 0x00400000;
	TIMERx_BASE(2)[TIMER_CTRL] = 0x07;

	TIMER_IRQ_EN = 0x04; // only interrupt on timer 2

	registerVirtDeviceHandler((((unsigned int)TIMER_BASE) & 0xFFFFF000), vtimerVirtDeviceHandler);
	registerIRQHandler(56, timer_interrupt);
//	enable_irq(54);
//	enable_irq(55);
	enable_irq(56);
}

/**
 * \fn print_timer_value
 *
 * Debug routine to print current value of timer
 */
void print_timer_value()
{
	print_str("\r\nTimer value:");
	print_hex(TIMER2_CUR_INTRVL);
	print_str("      Status: ");
	print_hex(TIMER_IRQ_STATUS);
	print_str("\r\nTimer IRQ en:");
	print_hex(TIMER_IRQ_EN);
}
extern struct vtimer_s vtimerctrl;
extern struct vtimerx_s vtimer[4];

/**
 * \fn timer_interrupt
 *
 * Timer interrupt handler
 */
void timer_interrupt(struct cpuRegs_s *regs)
{
	static unsigned int count = 0;
	unsigned int i;
	TIMER_IRQ_STATUS |= 0x04;
	TIMER_IRQ_EN = 0x04;
	TIMER2_CTRL |= 0x01;
//	print_str("Tick\r\n");

	triggerVIRQ(54 + (count % 2));
	vtimerctrl.status |= 0x07;
	for (i = 0; i < 4; i++) {
		vtimer[i].curval = TIMERx_BASE(i)[TIMER_CURVL];
	}
	schedule(regs);
	count++;
}

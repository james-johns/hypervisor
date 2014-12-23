/**
 * \file
 * \author James Johns
 */

#include <cpu.h>
#include <printh.h>
#include <irq.h>

#define NULL ((void *)0)

irqHandler_t handlers[IRQ_COUNT];

/**
 * \fn registerIRQHandler(unsigned int irq, irqHandler_t handler)
 *
 * Register handler to service irq
 */
void registerIRQHandler(unsigned int irq, irqHandler_t handler)
{
	if (irq >= IRQ_COUNT)
		return;
	handlers[irq] = handler;
}

/**
 * \fn callIRQHandler(unsigned int irq, struct cpuRegs_s *regs)
 *
 * Call registered IRQ handler if available
 */
void callIRQHandler(unsigned int irq, struct cpuRegs_s *regs)
{
	if (irq >= IRQ_COUNT)
		return;
	if (handlers[irq] != NULL) {
		handlers[irq](regs);
	} else {
		printh("Unhandled Interrupt %d\r\n", irq);
	}
}

/**
 * \fn initIRQHandlers
 *
 * Initialise IRQ handlers list
 */
void initIRQHandlers()
{
	int i;
	for (i = 0; i < IRQ_COUNT; i++) {
		handlers[i] = NULL;
	}
}

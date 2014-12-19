

#include <cpu.h>
#include <printh.h>
#include <irq.h>
#include <vgic.h>

#define NULL ((void *)0)

irqHandler_t handlers[IRQ_COUNT];

void registerIRQHandler(unsigned int irq, irqHandler_t handler)
{
	if (irq >= IRQ_COUNT)
		return;
	handlers[irq] = handler;
}

void print_regs(struct cpuRegs_s *regs);
void callIRQHandler(unsigned int irq, struct cpuRegs_s *regs)
{
	if (irq >= IRQ_COUNT)
		return;
//	print_regs(regs);
	if (handlers[irq] != NULL) {
		handlers[irq](regs);
	} else {
		printh("Unhandled Interrupt %d\r\n", irq);
//		while(1);		
		triggerVIRQ(irq);
	}
}

void initIRQHandlers()
{
	int i;
	for (i = 0; i < IRQ_COUNT; i++) {
		handlers[i] = NULL;
	}
}



#ifndef _IRQ_H_
#define _IRQ_H_

#include <cpu.h>

#define IRQ_COUNT 128

typedef void (*irqHandler_t)(struct cpuRegs_s *regs);

void registerIRQHandler(unsigned int irq, irqHandler_t handler);
void callIRQHandler(unsigned int irq, struct cpuRegs_s *regs);

void initIRQHandlers();


#endif

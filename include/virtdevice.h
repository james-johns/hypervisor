

#ifndef _VIRTDEVICE_H_
#define _VIRTDEVICE_H_

#include <cpu.h>

typedef void (*virtDeviceHandler_t)(struct cpuRegs_s *regs);

void initVirtDevice();
signed int virtDeviceOrderPredicate(void *l, void *r);
void callVirtDeviceHandler(unsigned int address, struct cpuRegs_s *regs);
void registerVirtDeviceHandler(unsigned int address, virtDeviceHandler_t handler);

#endif

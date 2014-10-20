


#ifndef _GIC_H_
#define _GIC_H_

#ifndef GIC_BASE
#error "GIC_BASE must be defined before including gic.h"
#endif

#define GICD ((unsigned int *)(GIC_BASE + 0x1000))
#define GICC ((unsigned int *)(GIC_BASE + 0x2000))
#define GICH ((unsigned int *)(GIC_BASE + 0x4000))

void init_gic();

#endif


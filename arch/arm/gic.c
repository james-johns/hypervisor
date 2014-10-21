
#define GIC_BASE 0x01c80000
#include <gic.h>
#include <printh.h>

void init_gic_distributor() {
  GICD[GICD_CTLR] = 0x0; // disable GIC
  
  unsigned int typer = GICD[GICD_TYPER];
  unsigned int lines = 32*((typer & 0x1F)+1);
  unsigned int i;
  /* set global interrupts to active low, level sensitive */
  for (i = 32; i < lines; i += 16) {
    GICD[GICD_ICFGR(i/16)] = 0x0;
  }
  for (i = 32; i < lines; i += 16) {
    GICD[GICD_ITARGETSR(i/4)] = 0x01010101;
  }
  for (i = 32; i < lines; i += 4) {
    GICD[GICD_IPRIORITYR(i/4)] = 0xa0a0a0a0;
  }
  for (i = 32; i < lines; i += 32) {
    GICD[GICD_ICENABLER(i/32)] = 0xFFFFFFFF;
  }

  GICD[GICD_CTLR] = 0x01;
}

void init_gic_cpu() {
  unsigned int i;
  GICD[GICD_ICENABLER(0)] = 0xFFFF0000;
  GICD[GICD_ISENABLER(0)] = 0x0000FFFF;
  for (i = 0; i < 32; i += 4)
    GICD[GICD_IPRIORITYR(i/4)] = 0xa0a0a0a0;

  GICC[GICC_PMR] = 0xff;
  GICC[GICC_BPR] = 0;
  GICC[GICC_CTLR] = 0x201;
}

void init_gic() {
  init_gic_distributor();
  init_gic_cpu();
  
}


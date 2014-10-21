
#define GIC_BASE 0x01c80000
#include <printh.h>
#include <gic.h>

void init_irqs();
void init_timer();

void hyp_main() {

  print_str("Starting hyp main\r\n");

  init_irqs();
  init_timer();

  GICD[GICD_SGIR] = 0x02008005;
  
  print_str("Halting!");
  while(1) {
    //    asm volatile("wfe"::);
  }
}

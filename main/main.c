
#include <printh.h>

void init_irqs();
void init_timer();

void hyp_main() {

  print_str("Starting hyp main\r\n");

  init_irqs();
  init_timer();

  print_str("Halting!");
  while(1) {
    asm volatile("wfe"::);
  }
}

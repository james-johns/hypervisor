


#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_

#ifndef __ASSEMBLY__
#error "Only include this file in an assembly file"
#endif

#ifndef GLOBAL
#define GLOBAL(sym) \
.global sym
#endif

#ifndef SAVE_REGS
#define SAVE_REGS 				\
  sub sp, #8;					\
  push {r0-r12};				\
  mrs r11, ELR_hyp;				\
  str r11, [sp, #52];				\
  mrs r11, spsr;				\
  str r11, [sp, #56]
#endif

#ifndef RESTORE_REGS
#define RESTORE_REGS				\
  pop {r0-r12};					\
  add sp, #8
#endif

#endif

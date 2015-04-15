


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
	sub sp, #28;				\
	push {r0-r12};				\
	mrs r11, ELR_hyp;			\
	str r11, [sp, #52];			\
	mrs r11, spsr;				\
	str r11, [sp, #56];			\
	mrs r11, SPSR_svc;			\
	str r11, [sp, #60];			\
	str lr, [sp, #64];			\
	mrs r11, SP_usr;			\
	str r11, [sp, #68];			\
	mrs r11, SP_svc;			\
	str r11, [sp, #72];			\
	mrs r11, lr_svc;			\
	str r11, [sp, #76];
#endif

#ifndef RESTORE_REGS
#define RESTORE_REGS				\
	ldr r11, [sp, #52];			\
	msr ELR_hyp, r11;			\
	ldr r11, [sp, #56];			\
	msr spsr, r11;				\
	ldr lr, [sp, #60];			\
	msr SPSR_svc, r11;			\
	ldr lr, [sp, #64];			\
	ldr r11, [sp, #68];			\
	msr SP_usr, r11;			\
	ldr r11, [sp, #72];			\
	msr SP_svc, r11;			\
	ldr r11, [sp, #76];			\
	msr lr_svc, r11;			\
	pop {r0-r12};				\
	add sp, #28;
#endif

#endif

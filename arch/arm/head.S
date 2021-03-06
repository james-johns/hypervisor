
.arm
#include <asm/assembler.h>
#include <printh.h>
	
.extern hyp_main
/* easy use print macro. requires usable stack,
	clobers r0 */
#ifndef PRINT
#define PRINT(_s) \
	ldr r0, =100f; \
	bl print_str; \
	b 101f; \
100:	.asciz _s; \
	.align 2; \
101:	
#endif

GLOBAL(_start)
_start:
	cpsid aif
	ldr sp, =boot_stack
	bl init_uart
	mov r0, #'A'
	bl putc
	mov r0, #'\r'
	bl putc
	mov r0, #'\n'
	bl putc
check_mode:
	mrs r0, cpsr
	and r0, r0, #0x1F
	cmp r0, #0x1A
	bne non_hyp
	PRINT("Hyp mode active\r\n")

	b _start_main
non_hyp:
	PRINT("Hyp mode not active. ~~~ Halting! ~~~\r\n")
	b halt

_start_main:
	mrs r0, cpsr
	bl print_hex
	bl hyp_main

halt:
	wfe
	b halt

GLOBAL(enableIRQS)
enableIRQS:
	cpsie aif
	mov pc, lr
GLOBAL(disableIRQS)
disableIRQS:
	cpsid aif
	mov pc, lr
	
GLOBAL(install_hyp_vectors)
install_hyp_vectors:
	push {lr}
	ldr r0, =hyp_vectors
	mcr p15, 4, r0, c12, c0, 0 /* set HVBAR */
	pop {lr}
	bx lr

.align 5
hyp_vectors:
	.word 0
	b trap_undefined_instruction
	b trap_supervisor_call
	b trap_prefetch_abort
	b trap_data_abort
	b trap_hyp_call
	b trap_irq
	b trap_fiq

.extern handle_trap_undefined_instruction
trap_undefined_instruction:
	SAVE_REGS
	mov r0, sp
	bl handle_trap_undefined_instruction
	RESTORE_REGS
	eret
trap_supervisor_call:
	SAVE_REGS
	mov r0, sp
	bl handle_trap_supervisor_call
	RESTORE_REGS
	eret
trap_prefetch_abort:
	SAVE_REGS
	mov r0, sp
	bl handle_trap_prefetch_abort
	RESTORE_REGS
	eret
trap_data_abort:
	SAVE_REGS
	mov r0, sp
	bl handle_trap_data_abort
	RESTORE_REGS
	eret
trap_hyp_call:
	SAVE_REGS
	mov r0, sp
	bl handle_trap_hyp_call
	RESTORE_REGS
	eret
trap_irq:
	SAVE_REGS
	mov r0, sp
	bl handle_trap_irq
	RESTORE_REGS
	eret
trap_fiq:
	SAVE_REGS
	mov r0, sp
	bl handle_trap_fiq
	RESTORE_REGS
	eret

GLOBAL(setGuestTTBR)
setGuestTTBR:
	mov r1, #0
	mcrr p15, 6, r0, r1, c2
	mov pc, lr
GLOBAL(setVTCR)
setVTCR:
	mcr p15, 4, r0, c2, c1, 2
	mov pc, lr
GLOBAL(getVTCR)
getVTCR:
	mrc p15, 4, r0, c2, c1, 2
	mov pc, lr
GLOBAL(setHCR)
setHCR:
	mcr p15, 4, r0, c1, c1, 0
	mov pc, lr
GLOBAL(getHCR)
getHCR:
	mrc p15, 4, r0, c1, c1, 0
	mov pc, lr

GLOBAL(setHTTBR)
setHTTBR:
	mcrr p15, 4, r0, r1, c2
	mov pc, lr
GLOBAL(setMAIR0VAL)
setMAIR0VAL:
	mcr p15, 0, r0, c10, c2, 0
	mcr p15, 4, r0, c10, c2, 0
	mov pc, lr
GLOBAL(setMAIR1VAL)
setMAIR1VAL:
	mcr p15, 0, r0, c10, c2, 1
	mcr p15, 4, r0, c10, c2, 1
	mov pc, lr
GLOBAL(enableMMU)
enableMMU:	
	mov r0, #0
	orr r0, r0, #0x01 /* T0SZ */
	orr r0, r0, #0x80000000 /* always 1 */
	orr r0, r0, #0x3000 /* SH0 */
	orr r0, r0, #0xC00 /* ORGN0, IRGN0 */
	mcr p15, 4, r0, c2, c0, 2
	mrc p15, 4, r0, c1, c0, 0
	orr r0, r0, #0x03 /* MMU and D-Cache enable */
	dsb
	mcr p15, 4, r0, c1, c0, 0
	isb
	mov pc, lr
	
#define UART_BASE 	0x01c28000
#define UART_TX		0x00
#define	UART_IER	0x04
#define UART_FCR	0x08
#define UART_LCR	0x0C
#define UART_MCR	0x10


/* init_uart
 * IN:
 * OUT:
 * SMASHED: r0, r1, r2
 *
 * Initialise UART0 to 8n1 with interrupts and fifos disabled
 */
init_uart:
	ldr r0, =UART_BASE
	mov r1, #0x03
	mov r2, #0x00
	strb r1, [r0, #UART_LCR]
	strb r2, [r0, #UART_IER]
	strb r2, [r0, #UART_FCR]
	strb r1, [r0, #UART_MCR]
	mov pc, lr
/* putc
 * IN: r0 - character to print
 * OUT:
 * SMASHED: r1
 */
putc:
	ldr r1, =UART_BASE
	strb r0, [r1, #UART_TX]
	mov pc, lr

.comm boot_stack, 0x1000


	
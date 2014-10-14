
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

	ldr r0, =hyp_vectors
	mcr p15, 4, r0, c12, c0, 0 /* set HVBAR */

	b _start_main
non_hyp:
	PRINT("Hyp mode not active. ~~~ Halting! ~~~\r\n")
	b halt

_start_main:
	bl hyp_main

halt:
	wfe
	b halt

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

trap_undefined_instruction:
	PRINT("Undefined instruction in hyp mode")
	eret
trap_supervisor_call:
	PRINT("Supervisor call trapped in hyp mode")
	eret
trap_prefetch_abort:
	PRINT("Prefetch abort in hyp mode")
	eret
trap_data_abort:
	PRINT("Data abort in hyp mode")
	eret
trap_hyp_call:
	PRINT("Hyp call handler")
	eret
trap_irq:
	PRINT("IRQ trapped in hyp mode")
	eret
trap_fiq:
	PRINT("FIQ trapped in hyp mode")
	eret

#define TEST_VAR 0x01
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


	
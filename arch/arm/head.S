.arm
.section ".text"
.extern hyp_main
.global _start
_start:
	mov r0, #'A'
	bl putc
	b hyp_main
1:
	wfe
	b 1b

.equ UART_BASE, 0x01c28000
.equ UART_TX,	0x00
# putc
# IN: r0 - character to print
# OUT:
# SMASHED: r1
#
putc:
	ldr r1, =UART_BASE
	strb r0, [r1, #UART_TX]
	mov pc, lr


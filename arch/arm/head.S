.arm
.section ".text"
.extern hyp_main
.extern print_hex
.extern print_str
.global _start
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
	bl print_hex
# read SCR to find whether we are in secure mode
	mrc p15, 0, r1, c1, c1, 0
	and r1, r1, #0x01
	tst r1, #0x01
	bne non_secure
	ldr r0, =secure_mode_msg
	bl print_str
	b _start_main
non_secure:
	ldr r0, =nonsecure_mode_msg
	bl print_str
_start_main:
	bl hyp_main
1:
	wfe
	b 1b

.equ UART_BASE, 0x01c28000
.equ UART_TX,	0x00
.equ UART_IER,  0x04
.equ UART_FCR,  0x08
.equ UART_LCR,  0x0C
.equ UART_MCR,  0x10

# init_uart
# IN:
# OUT:
# SMASHED: r0, r1, r2
#
# Initialise UART0 to 8n1 with interrupts and fifos disabled
#
init_uart:
	ldr r0, =UART_BASE
	mov r1, #0x03
	mov r2, #0x00
	strb r1, [r0, #UART_LCR]
	strb r2, [r0, #UART_IER]
	strb r2, [r0, #UART_FCR]
	strb r1, [r0, #UART_MCR]
	mov pc, lr
# putc
# IN: r0 - character to print
# OUT:
# SMASHED: r1
#
putc:
	ldr r1, =UART_BASE
	strb r0, [r1, #UART_TX]
	mov pc, lr

secure_mode_msg:	.asciz "Secure mode active\r\n"
nonsecure_mode_msg:	.asciz "non-Secure mode active\r\n"

.comm boot_stack, 0x1000


	
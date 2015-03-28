/**
 * \file
 * \author James Johns
 *
 * Provide methods for simple debugging via text output on UART
 */

#include <va_arg.h>
#include <printh.h>

#define UART_BASE ((unsigned char *)0x01C28000)
#define UART_TX 0x00
#define UART_LINE_STATUS 0x14

#define STATUS_Tx_EMPTY (1 << 5)

/**
 * \fn printh_format(const char c, va_list *args)
 * \param[in] c Format code character
 * \param[in] args Variable arguments list
 * \brief Convenience method for printh
 *
 * Used by printh to acheive formatted output.
 */
void printh_format(const char c, va_list *args)
{
	switch (c) {
	case 'd':
		print_hex(va_arg(*args, int));
		break;
	case 's':
		print_str(va_arg(*args, const char *));
		break;
	case 'c':
		putc(va_arg(*args, char));
		break;
	default:
		print_str("~~ Error in printh_format ~~");
		break;
	}
}

/**
 * \fn printh(const char *fmt, ...)
 *
 * Similar to printf/printk, printh allows formatted output via
 * inline escape codes in the given string. Each escape code requires
 * an additional argument to be passed.
 */
void printh(const char *fmt, ...)
{
	va_list args;
	int i=0;
	va_start(args, fmt);
	for (; fmt[i] != 0x00; i++) {
		switch (fmt[i]) {
		case '%':
			i++; // skip the next character
			printh_format(fmt[i], &args);
			break;
		default:
			putc(fmt[i]);
			break;
		}
	}
	va_end(args);
}

/**
 * \fn print_str(const char *str)
 *
 * Convenience method for outputting multi-character strings.
 * Uses putc.
 */
void print_str(const char *str)
{
	int i;
	for (i = 0; str[i] != 0x00; i++) {	// loop until NULL terminator at end of string
		putc(str[i]);
	}
}

/**
 * \fn print_hex(unsigned int value)
 *
 * Output a formatted HEX value on the default UART.
 * String will begin with '0x'
 */
void print_hex(unsigned int value)
{
	signed int i;
	unsigned char off;
	print_str("0x");
	for (i = 7; i >= 0; i--) {
		off = (value >> (i * 4)) & 0x0F;
		if (off < 10)
			putc('0' + off);
		else
			putc('A' + (off - 10));
	}
}

/**
 * \fn putc(char c)
 * \param c Character to output
 * 
 * Output a single character on the default UART
 */
void putc(char c)
{
	// wait for Tx to empty before outputting next character
	while ((UART_BASE[UART_LINE_STATUS] & STATUS_Tx_EMPTY) == 0x00);
	UART_BASE[UART_TX] = c;
}

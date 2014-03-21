

#define UART_BASE ((unsigned char *)0x01C28000)
#define UART_TX 0x00
#define UART_LINE_STATUS 0x14 

#define STATUS_Tx_EMPTY (1 << 5)

const char hex_data[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void print_hex(unsigned int value);
void print_str(const char *str);
void putc(char c);


void print_str(const char *str) {
  int i;
  for (i = 0; str[i] != 0x00; i++) { // loop until NULL terminator at end of string
    putc(str[i]);
  }
}

void print_hex(unsigned int value) {
  signed int i;
  print_str("0x");
  for (i = 7; i >= 0; i--) {
    putc(*(((value >> (i * 4)) & 0x0F) + &hex_data[0]));
  }
}

void putc(char c) {
  // wait for Tx to empty before outputting next character
  while ((UART_BASE[UART_LINE_STATUS] & STATUS_Tx_EMPTY) == 0x00);
  UART_BASE[UART_TX] = c;
}

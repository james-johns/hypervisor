
extern void print_str(const char *str);

void hyp_main() {
  print_str("Starting hyp main\r\n");
  print_hex(0x12345678);
  while(1);
}

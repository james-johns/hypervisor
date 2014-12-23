

#ifndef PRINTH_H
#define PRINTH_H

#ifdef __ASSEMBLY__
.extern print_str
.extern print_hex
.extern printh
#else

void printh(const char *fmt, ...);
void print_str(const char *str);
void print_hex(unsigned int val);
void putc(char c);
#endif

#endif

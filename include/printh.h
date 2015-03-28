

#ifndef PRINTH_H
#define PRINTH_H

#ifdef __ASSEMBLY__
.extern print_str
.extern print_hex
.extern printh
#else

#include <cpu.h>

void printh(const char *fmt, ...);
void print_str(const char *str);
void print_hex(unsigned int val);
void putc(char c);
void print_regs(struct cpuRegs_s *regs);
#endif

#endif

/*
 * va_arg.h
 *
 * Implementation of variable argument lists.
 */

#ifndef _VA_ARG_H_
#define _VA_ARG_H_

typedef char * va_list;

#define RND (sizeof(int)-1)
#define _bnd(X, bnd) (((sizeof(X)) + (bnd)) & (~bnd))

/* assign arg the start address of the variable argument list
 * calculated as an offset from the previous fixed argument */
#define va_start(arg, S)       (arg = (((char *) &(S)) + (_bnd(S, RND))))
/* return the variable at address pointed to by arg, as type T, and
 * move arg by size of T */
#define va_arg(arg, T)         (*(T *)((arg += (_bnd(T, RND)))-(_bnd(T, RND))))
#define va_end(arg)            (arg = 0x0)

void test_va_arg();

#endif

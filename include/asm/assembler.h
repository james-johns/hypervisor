


#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_

#ifndef __ASSEMBLY__
#error "Only include this file in an assembly file"
#endif

#ifndef GLOBAL
#define GLOBAL(sym) \
.global sym
#endif



#endif

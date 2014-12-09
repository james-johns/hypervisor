ENTRY(_start)
SECTIONS
{
 . = 0x40008000;
/* .head . : { arch/arm/head.o(.text) } */
 .text : { *(.text*) }
 .rodata : ALIGN(0x1000) { *(.rodata) }
 .data : ALIGN(0x1000) { *(.data*) }
 .bss : ALIGN(0x1000) { *(.bss*) *(COMMON*) }
 . = ALIGN(8);
 _end = .;
}
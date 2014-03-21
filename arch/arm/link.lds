ENTRY(_start)
SECTIONS
{
 . = 0x40000000;
 .head . : { arch/arm/head.o(.text) }
 .text : { *(.text) }
 .data : { *(.data) }
 .bss : { *(.bss COMMON) }
 . = ALIGN(8);
}
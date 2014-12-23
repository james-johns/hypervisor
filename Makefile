################################################################################
# Makefile
#
# Environment variables:
#     CROSS_COMPILE - string to be prepended to toolchain used to build
#
#     ARCH - architecture to build for. Used to include architecture specific 
#		code from arch/ subdirectory.
#
################################################################################
CROSS_COMPILE?=
ARCH?=arm

AS:=$(CROSS_COMPILE)as
CC:=$(CROSS_COMPILE)gcc
LD:=$(CROSS_COMPILE)ld

ASFLAGS+=-c -I./include -I./arch/$(ARCH)/include -D__ASSEMBLY__ -mcpu=cortex-a7
CFLAGS+=-ffreestanding -Wall -Wextra -Werror -nostdlib -nostartfiles -mcpu=cortex-a7 -g \
	-I./include -I./arch/$(ARCH)/include

Q:=@-

.SUFFIXES:

# objects required to build hypervisor. Add an dependency via +=
hypervisor-obj:=

# include arch specific code
include arch/$(ARCH)/Makefile
include main/Makefile
include lib/Makefile
include tests/Makefile

.PHONY: all clean distclean hypervisor TAGS FORCE

default: uImage

all: hypervisor uImage

clean:
	$(Q)echo " [RM] *.o"
	$(Q)rm $(hypervisor-obj) 
	$(Q)rm $(hypervisor-obj:.o=.d)
	$(Q)echo " [RM] hypervisor"
	$(Q)rm hypervisor hypervisor.bin uImage

distclean: clean

TAGS:
	$(Q)find . -name '*.c' | etags -
	$(Q)find . -name '*.h' | etags --append -
	$(Q)find . -name '*.S' | etags --append -


###
# Specfic build targets
###

hypervisor: $(hypervisor-obj)
	$(Q)echo " [LD] $@"
	$(Q)$(LD) $(LDFLAGS) -T arch/$(ARCH)/link.lds -o $@ $(hypervisor-obj)

uImage: hypervisor
	$(Q)$(CROSS_COMPILE)objcopy -O binary hypervisor hypervisor.bin
	$(Q)mkimage -A $(ARCH) -T kernel -C none -a 0x40008000 -d hypervisor.bin uImage

docs: FORCE
	$(Q)mkdir docs/
	$(Q)doxygen doxyfile

###
# Build templates
###
.SUFFIXES:
.SUFFIXES: .c .o .S

# Include dependency files generated by gcc (-MM flag)
-include $(hypervisor-obj:.o=.d)

.S.o:
	$(Q)echo " [AS] $@"
	$(Q)$(CC) $(ASFLAGS) -o $@ $<
	$(Q)$(CC) $(ASFLAGS) -MM $< > $*.d
	$(Q)mv $*.d $*.d.tmp
	$(Q)sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	$(Q)rm $*.d.tmp

.c.o:
	$(Q)echo " [CC] $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<
	$(Q)$(CC) $(CFLAGS) -MM $< > $*.d
	$(Q)mv $*.d $*.d.tmp
	$(Q)sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	$(Q)rm $*.d.tmp


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

AS:=$(CROSS_COMPILE)as
CC:=$(CROSS_COMPILE)gcc
LD:=$(CROSS_COMPILE)ld

ASFLAGS+=-c -I./include -D__ASSEMBLY__
CFLAGS+=-ffreestanding -Wall -Wextra -Werror -nostdlib -nostartfiles -g -I./include

Q:=@-

.SUFFIXES:

# objects required to build hypervisor. Add an dependency via +=
hypervisor-obj:=

# include arch specific code
ARCH?=arm
include arch/$(ARCH)/Makefile
include main/Makefile

.PHONY: all clean distclean hypervisor

all: hypervisor

clean:
	$(Q)echo " [RM] *.o"
	$(Q)rm $(hypervisor-obj) 
	$(Q)echo " [RM] hypervisor"
	$(Q)rm hypervisor

distclean:


###
# Specfic build targets
###

hypervisor: $(hypervisor-obj)
	$(Q)echo " [LD] $@"
	$(Q)$(LD) $(LDFLAGS) -T arch/$(ARCH)/link.lds -o $@ $(hypervisor-obj)



###
# Build templates
###
.SUFFIXES:
.SUFFIXES: .c .o .S

.S.o:
	$(Q)echo " [AS] $@"
	$(Q)$(CC) $(ASFLAGS) -o $@ $<

.c.o:
	$(Q)echo " [CC] $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

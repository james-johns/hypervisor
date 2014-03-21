
CROSS_COMPILE?=

AS:=$(CROSS_COMPILE)as
CC:=$(CROSS_COMPILE)gcc
LD:=$(CROSS_COMPILE)ld

CFLAGS+=-ffreestanding -Wall -Wextra -Werror -nostdlib -nostartfiles

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


hypervisor: $(hypervisor-obj)
	$(Q)echo " [LD] $@"
	$(Q)$(LD) $(LDFLAGS) -T arch/$(ARCH)/link.lds -o $@ $(hypervisor-obj)

.SUFFIXES:
.SUFFIXES: .c .o .S

.S.o:
	$(Q)echo " [AS] $@"
	$(Q)$(AS) $(ASFLAGS) -o $@ $<

.c.o:
	$(Q)echo " [CC] $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

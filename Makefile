
CROSS_COMPILE?=

AS:=$(CROSS_COMPILE)as
CC:=$(CROSS_COMPILE)gcc
LD:=$(CROSS_COMPILE)ld

Q:=@-

.SUFFIXES:

# objects required to build hypervisor. Add an dependency via +=
hypervisor-obj:=

# include arch specific code
ARCH?=arm
include arch/$(ARCH)/Makefile


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
	$(Q)$(LD) $(LDFLAGS) -o $@ $(hypervisor-obj)

.SUFFIXES:
.SUFFIXES: .c .o .S

.S.o:
	$(Q)echo " [AS] $@"
	$(Q)$(AS) $(ASFLAGS) -o $@ $<

.c.o:
	$(Q)echo " [CC] $@"
	$(CC) $(CFLAGS) -c $@ $<

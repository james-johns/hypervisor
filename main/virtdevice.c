/**
 * \file
 * \author James Johns
 */


#include <types.h>
#include <virtdevice.h>
#include <orderedList.h>
#include <cpu.h>
#include <malloc.h>
#include <printh.h>


#define VIRT_DEVICE_COUNT 16

struct orderedList_s *virtDevices;

struct virtDeviceTuple_s {
	unsigned int address;
	virtDeviceHandler_t handler;
};

/**
 * \fn registerVirtDeviceHandler(unsigned int address, virtDeviceHandler_t handler)
 *
 * Register virtual device handler for device address.
 *
 * Device handler will be called whenever a guest VM accesses the specified address.
 * (Must be sure address is not mapped r/w to VM in order to guarantee all accesses are
 * served by device handler)
 */
void registerVirtDeviceHandler(unsigned int address, virtDeviceHandler_t handler)
{
	struct virtDeviceTuple_s *tuple = malloc(sizeof(struct virtDeviceTuple_s));
	tuple->address = address;
	tuple->handler = handler;
	orderedListInsertItem(virtDevices, tuple);
	printh("Device handler item %d\r\n", orderedListGetItem(virtDevices, 0));
}

extern signed int currentVMID;

/**
 * \fn callVirtDeviceHandler(unsigned int address, struct cpuRegs_s *regs)
 *
 * Call registered virtual device handler to handled access to address.
 */
void callVirtDeviceHandler(unsigned int address, struct cpuRegs_s *regs)
{
	struct virtDeviceTuple_s tmp = { .address = address };
	struct virtDeviceTuple_s *handler = orderedListFindItem(virtDevices, &tmp);
	if (handler != NULL)
		handler->handler(regs);
	else {
		printh("No registered handler for %d %d\r\n", address, handler);
		printh("Current VMID: %d\r\n", currentVMID);
		print_regs(regs);
		while(1);
	}
}

/**
 * \fn virtDeviceOrderPredicate(void *l, void *r)
 *
 * Predicate used by virtDevices ordered list to compare entries
 */
signed int virtDeviceOrderPredicate(void *l, void *r)
{
	struct virtDeviceTuple_s *left, *right;
	left = (struct virtDeviceTuple_s *)l;
	right = (struct virtDeviceTuple_s *)r;
	return (left->address > right->address) ? 1 : ((left->address < right->address) ? -1 : 0);
}

/**
 * \fn initVirtDevice
 *
 * Initialise virt device subsystem
 */
void initVirtDevice()
{
	virtDevices = createOrderedList(VIRT_DEVICE_COUNT, virtDeviceOrderPredicate);
}

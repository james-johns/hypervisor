
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

void registerVirtDeviceHandler(unsigned int address, virtDeviceHandler_t handler)
{
	struct virtDeviceTuple_s *tuple = malloc(sizeof(struct virtDeviceTuple_s));
	tuple->address = address;
	tuple->handler = handler;
	orderedListInsertItem(virtDevices, tuple);
	printh("Device handler item %d\r\n", orderedListGetItem(virtDevices, 0));
}

void callVirtDeviceHandler(unsigned int address, struct cpuRegs_s *regs)
{
	struct virtDeviceTuple_s tmp = { .address = address };
	struct virtDeviceTuple_s *handler = orderedListFindItem(virtDevices, &tmp);
	if (handler != NULL)
		handler->handler(regs);
	else
		printh("No registered handler for %d %d\r\n", address, handler);
}

signed int virtDeviceOrderPredicate(void *l, void *r)
{
	struct virtDeviceTuple_s *left, *right;
	left = (struct virtDeviceTuple_s *)l;
	right = (struct virtDeviceTuple_s *)r;
	return (left->address > right->address) ? 1 : ((left->address < right->address) ? -1 : 0);
}

void initVirtDevice()
{
	virtDevices = createOrderedList(VIRT_DEVICE_COUNT, virtDeviceOrderPredicate);
}

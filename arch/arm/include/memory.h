

#ifndef _MEMORY_H_
#define _MEMORY_H_

#define LPAE_ENTRIES 0x200

struct pageDescriptor_s {
	unsigned int type:2;
	unsigned int lowAttrs:10;
	unsigned int lowAddr:9;
	unsigned int highAddr:19;
	unsigned int zero:12;
	unsigned int highAttrs:12;
} __attribute__ ((packed));

struct pageTable_s {
	struct pageDescriptor_s entry[LPAE_ENTRIES];
} __attribute__ ((packed));

void setHTTBR(unsigned int lo, unsigned int hi);
void setMAIR0VAL(unsigned int val);
void setMAIR1VAL(unsigned int val);
void setGuestTTBR(unsigned int tableAddr, unsigned int VMID);
void setVTCR(unsigned int vtcr);
unsigned int getVTCR();
void setHCR(unsigned int vtcr);
unsigned int getHCR();
void enableMMU();
void memcpy(void *source, void *dest, unsigned int size);
void memset(void *dest, unsigned char value, unsigned int size);

void init_mmu();

extern unsigned int _end;

#endif



#ifndef _MEMORY_H_
#define _MEMORY_H_

struct pageDescriptor_block_s {
	unsigned int type:2;
	unsigned int lowerBlockAttrs:10;
	unsigned int zero2:9;
	unsigned int outputAddr:18;
	unsigned int zero1:12;
	unsigned int upperBlockAttrs:12;
} __attribute__ ((packed));

struct pageDescriptor_table_s {
	unsigned int type:2;
	unsigned int lowerBlockAttrs:10;
	unsigned int zero2:9;
	unsigned int outputAddr:18;
	unsigned int zero1:12;
	unsigned int upperBlockAttrs:12;
} __attribute__ ((packed));

struct pageDescriptor_s {
	union {
		struct pageDescriptor_block_s block;
		struct pageDescriptor_table_s table;
	};
} __attribute__ ((packed));

void init_mmu();

#endif

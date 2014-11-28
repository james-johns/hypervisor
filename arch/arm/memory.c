
#include <config.h>
#include <memory.h>
#include <printh.h>

#define LPAE_ENTRIES 0x200 // 512 entries of 8 bytes each; 4k table size
struct pageDescriptor_s pageTable[LPAE_ENTRIES] __attribute__((__aligned__(4096)));

void setHTTBR(unsigned int lo, unsigned int hi);
void setMAIR0VAL(unsigned int val);
void setMAIR1VAL(unsigned int val);
void enableMMU();

void init_mmu() {
  print_str("\r\nSize of block page desciptor: ");
  print_hex(sizeof(struct pageDescriptor_block_s));
  print_str("\r\nSize of table page desciptor: ");
  print_hex(sizeof(struct pageDescriptor_table_s));
  print_str("\r\nSize of page desciptor: ");
  print_hex(sizeof(struct pageDescriptor_s));
  print_str("\r\nSize of page table: ");
  print_hex(sizeof(pageTable));
  unsigned int i;

  /* zero the table */
  for (i = 0; i < LPAE_ENTRIES; i++) {
    pageTable[i].block.type = 0x0;
    pageTable[i].block.outputAddr = 0;
    pageTable[i].block.upperBlockAttrs = 0x0;
    pageTable[i].block.lowerBlockAttrs = 0x0;
    pageTable[i].block.zero1 = 0;
    pageTable[i].block.zero2 = 0;
  }

  /* devices */
  for (i = 0; i < LPAE_ENTRIES/4; i++) {
    pageTable[i].block.type = 0x1;
    pageTable[i].block.outputAddr = i*(1<<9);
    pageTable[i].block.upperBlockAttrs = 0x00;
    pageTable[i].block.lowerBlockAttrs = 0x39c; // 11 1001 1100
    pageTable[i].block.zero1 = 0;
    pageTable[i].block.zero2 = 0;
    print_str("\r\npageTable entry: ");
    print_hex(((unsigned int *)&pageTable[i])[0]);
    print_hex(((unsigned int *)&pageTable[i])[1]);
  }

  /* memory */
  for (; i < LPAE_ENTRIES; i++) {
    pageTable[i].block.type = 0x1;
    pageTable[i].block.outputAddr = i*(1<<9);
    pageTable[i].block.upperBlockAttrs = 0x00;
    pageTable[i].block.lowerBlockAttrs = 0x3df; // 11 1101 1111
    pageTable[i].block.zero1 = 0;
    pageTable[i].block.zero2 = 0;
    print_str("\r\npageTable entry: ");
    print_hex(((unsigned int *)&pageTable[i])[0]);
    print_hex(((unsigned int *)&pageTable[i])[1]);
  }

  unsigned int HTTBR_hi, HTTBR_lo;
  HTTBR_hi = 0; HTTBR_lo = (unsigned int)pageTable;
  setHTTBR(HTTBR_lo, HTTBR_hi);
  setMAIR0VAL(0xEEAA4400);
  setMAIR1VAL(0xFF000004);
  enableMMU();
}





#define PIO_BASE   0x01c20800

#define Pn_CFG0(n) (PIO_BASE + ((n * 0x24) + 0x00))
#define Pn_CFG1(n) (PIO_BASE + ((n * 0x24) + 0x04))
#define Pn_CFG2(n) (PIO_BASE + ((n * 0x24) + 0x08))
#define Pn_CFG3(n) (PIO_BASE + ((n * 0x24) + 0x0C))

#define Pn_DAT(n)  (PIO_BASE + ((n * 0x24) + 0x10))
#define Pn_DRV0(n) (PIO_BASE + ((n * 0x24) + 0x14))
#define Pn_DRV1(n) (PIO_BASE + ((n * 0x24) + 0x18))
#define Pn_PUL0(n) (PIO_BASE + ((n * 0x24) + 0x1C))
#define Pn_PUL1(n) (PIO_BASE + ((n * 0x24) + 0x20))


void init_pio()
{
  *(unsigned int *)Pn_CFG0(1) = 0x00000022;
  *(unsigned int *)Pn_PUL0(1) = 0x0000000A;
}



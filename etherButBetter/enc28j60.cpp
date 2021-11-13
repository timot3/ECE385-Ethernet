
#include "enc28j60.h"

#include <system.h>

uint16_t ENC28J60::bufferSize;
bool ENC28J60::broadcast_enabled = false;
bool ENC28J60::promiscuous_enabled = false;

// ENC28J60 Control Registers
// Control register definitions are a combination of address,
// bank number, and Ethernet/MAC/PHY indicator bits.
// - Register address        (bits 0-4)
// - Bank number        (bits 5-6)
// - MAC/PHY indicator        (bit 7)
#define ADDR_MASK 0x1F
#define BANK_MASK 0x60
#define SPRD_MASK 0x80
// All-bank registers
#define EIE 0x1B
#define EIR 0x1C
#define ESTAT 0x1D
#define ECON2 0x1E
#define ECON1 0x1F
// Bank 0 registers
#define ERDPT (0x00 | 0x00)
#define EWRPT (0x02 | 0x00)
#define ETXST (0x04 | 0x00)
#define ETXND (0x06 | 0x00)
#define ERXST (0x08 | 0x00)
#define ERXND (0x0A | 0x00)
#define ERXRDPT (0x0C | 0x00)
// #define ERXWRPT         (0x0E|0x00)
#define EDMAST (0x10 | 0x00)
#define EDMAND (0x12 | 0x00)
// #define EDMADST         (0x14|0x00)
#define EDMACS (0x16 | 0x00)
// Bank 1 registers
#define EHT0 (0x00 | 0x20)
#define EHT1 (0x01 | 0x20)
#define EHT2 (0x02 | 0x20)
#define EHT3 (0x03 | 0x20)
#define EHT4 (0x04 | 0x20)
#define EHT5 (0x05 | 0x20)
#define EHT6 (0x06 | 0x20)
#define EHT7 (0x07 | 0x20)
#define EPMM0 (0x08 | 0x20)
#define EPMM1 (0x09 | 0x20)
#define EPMM2 (0x0A | 0x20)
#define EPMM3 (0x0B | 0x20)
#define EPMM4 (0x0C | 0x20)
#define EPMM5 (0x0D | 0x20)
#define EPMM6 (0x0E | 0x20)
#define EPMM7 (0x0F | 0x20)
#define EPMCS (0x10 | 0x20)
// #define EPMO            (0x14|0x20)
#define EWOLIE (0x16 | 0x20)
#define EWOLIR (0x17 | 0x20)
#define ERXFCON (0x18 | 0x20)
#define EPKTCNT (0x19 | 0x20)
// Bank 2 registers
#define MACON1 (0x00 | 0x40 | 0x80)
#define MACON3 (0x02 | 0x40 | 0x80)
#define MACON4 (0x03 | 0x40 | 0x80)
#define MABBIPG (0x04 | 0x40 | 0x80)
#define MAIPG (0x06 | 0x40 | 0x80)
#define MACLCON1 (0x08 | 0x40 | 0x80)
#define MACLCON2 (0x09 | 0x40 | 0x80)
#define MAMXFL (0x0A | 0x40 | 0x80)
#define MAPHSUP (0x0D | 0x40 | 0x80)
#define MICON (0x11 | 0x40 | 0x80)
#define MICMD (0x12 | 0x40 | 0x80)
#define MIREGADR (0x14 | 0x40 | 0x80)
#define MIWR (0x16 | 0x40 | 0x80)
#define MIRD (0x18 | 0x40 | 0x80)
// Bank 3 registers
#define MAADR1 (0x00 | 0x60 | 0x80)
#define MAADR0 (0x01 | 0x60 | 0x80)
#define MAADR3 (0x02 | 0x60 | 0x80)
#define MAADR2 (0x03 | 0x60 | 0x80)
#define MAADR5 (0x04 | 0x60 | 0x80)
#define MAADR4 (0x05 | 0x60 | 0x80)
#define EBSTSD (0x06 | 0x60)
#define EBSTCON (0x07 | 0x60)
#define EBSTCS (0x08 | 0x60)
#define MISTAT (0x0A | 0x60 | 0x80)
#define EREVID (0x12 | 0x60)
#define ECOCON (0x15 | 0x60)
#define EFLOCON (0x17 | 0x60)
#define EPAUS (0x18 | 0x60)

// ENC28J60 ERXFCON Register Bit Definitions
#define ERXFCON_UCEN 0x80
#define ERXFCON_ANDOR 0x40
#define ERXFCON_CRCEN 0x20
#define ERXFCON_PMEN 0x10
#define ERXFCON_MPEN 0x08
#define ERXFCON_HTEN 0x04
#define ERXFCON_MCEN 0x02
#define ERXFCON_BCEN 0x01
// ENC28J60 EIE Register Bit Definitions
#define EIE_INTIE 0x80
#define EIE_PKTIE 0x40
#define EIE_DMAIE 0x20
#define EIE_LINKIE 0x10
#define EIE_TXIE 0x08
#define EIE_WOLIE 0x04
#define EIE_TXERIE 0x02
#define EIE_RXERIE 0x01
// ENC28J60 EIR Register Bit Definitions
#define EIR_PKTIF 0x40
#define EIR_DMAIF 0x20
#define EIR_LINKIF 0x10
#define EIR_TXIF 0x08
#define EIR_WOLIF 0x04
#define EIR_TXERIF 0x02
#define EIR_RXERIF 0x01
// ENC28J60 ESTAT Register Bit Definitions
#define ESTAT_INT 0x80
#define ESTAT_LATECOL 0x10
#define ESTAT_RXBUSY 0x04
#define ESTAT_TXABRT 0x02
#define ESTAT_CLKRDY 0x01
// ENC28J60 ECON2 Register Bit Definitions
#define ECON2_AUTOINC 0x80
#define ECON2_PKTDEC 0x40
#define ECON2_PWRSV 0x20
#define ECON2_VRPS 0x08
// ENC28J60 ECON1 Register Bit Definitions
#define ECON1_TXRST 0x80
#define ECON1_RXRST 0x40
#define ECON1_DMAST 0x20
#define ECON1_CSUMEN 0x10
#define ECON1_TXRTS 0x08
#define ECON1_RXEN 0x04
#define ECON1_BSEL1 0x02
#define ECON1_BSEL0 0x01
// ENC28J60 MACON1 Register Bit Definitions
#define MACON1_LOOPBK 0x10
#define MACON1_TXPAUS 0x08
#define MACON1_RXPAUS 0x04
#define MACON1_PASSALL 0x02
#define MACON1_MARXEN 0x01
// ENC28J60 MACON3 Register Bit Definitions
#define MACON3_PADCFG2 0x80
#define MACON3_PADCFG1 0x40
#define MACON3_PADCFG0 0x20
#define MACON3_TXCRCEN 0x10
#define MACON3_PHDRLEN 0x08
#define MACON3_HFRMLEN 0x04
#define MACON3_FRMLNEN 0x02
#define MACON3_FULDPX 0x01
// ENC28J60 MICMD Register Bit Definitions
#define MICMD_MIISCAN 0x02
#define MICMD_MIIRD 0x01
// ENC28J60 MISTAT Register Bit Definitions
#define MISTAT_NVALID 0x04
#define MISTAT_SCAN 0x02
#define MISTAT_BUSY 0x01

// ENC28J60 EBSTCON Register Bit Definitions
#define EBSTCON_PSV2 0x80
#define EBSTCON_PSV1 0x40
#define EBSTCON_PSV0 0x20
#define EBSTCON_PSEL 0x10
#define EBSTCON_TMSEL1 0x08
#define EBSTCON_TMSEL0 0x04
#define EBSTCON_TME 0x02
#define EBSTCON_BISTST 0x01

// PHY registers
#define PHCON1 0x00
#define PHSTAT1 0x01
#define PHHID1 0x02
#define PHHID2 0x03
#define PHCON2 0x10
#define PHSTAT2 0x11
#define PHIE 0x12
#define PHIR 0x13
#define PHLCON 0x14

// ENC28J60 PHY PHCON1 Register Bit Definitions
#define PHCON1_PRST 0x8000
#define PHCON1_PLOOPBK 0x4000
#define PHCON1_PPWRSV 0x0800
#define PHCON1_PDPXMD 0x0100
// ENC28J60 PHY PHSTAT1 Register Bit Definitions
#define PHSTAT1_PFDPX 0x1000
#define PHSTAT1_PHDPX 0x0800
#define PHSTAT1_LLSTAT 0x0004
#define PHSTAT1_JBSTAT 0x0002
// ENC28J60 PHY PHCON2 Register Bit Definitions
#define PHCON2_FRCLINK 0x4000
#define PHCON2_TXDIS 0x2000
#define PHCON2_JABBER 0x0400
#define PHCON2_HDLDIS 0x0100

// ENC28J60 Packet Control Byte Bit Definitions
#define PKTCTRL_PHUGEEN 0x08
#define PKTCTRL_PPADEN 0x04
#define PKTCTRL_PCRCEN 0x02
#define PKTCTRL_POVERRIDE 0x01

// SPI operation codes
#define ENC28J60_READ_CTRL_REG 0x00
#define ENC28J60_READ_BUF_MEM 0x3A
#define ENC28J60_WRITE_CTRL_REG 0x40
#define ENC28J60_WRITE_BUF_MEM 0x7A
#define ENC28J60_BIT_FIELD_SET 0x80
#define ENC28J60_BIT_FIELD_CLR 0xA0
#define ENC28J60_SOFT_RESET 0xFF

// max frame length which the controller will accept:
// (note: maximum ethernet frame length would be 1518)
#define MAX_FRAMELEN 1500

#define FULL_SPEED 1 // switch to full-speed SPI for bulk transfers

#define HIGH 1
#define LOW 0

#define ETHERNET_CHIP_SLAVE 0

volatile unsigned int *GPIO_PIO = (unsigned int*) GPIO_PIN_BASE;  

static uint8_t Enc28j60Bank;
static uint8_t selectPin; // slave select

// Returns nth bit of x
uint8_t bitRead(uint8_t x, uint8_t n) {
    return (x >> n) & 0x1;
}

// disableChip, enableChip -- pass in 
// IRQ ID's after creating the SPI module for the ethernet controller
void disableChip() {
  // TODO set slave select to high (it's active low)
  alt_ic_irq_disable(/*TODO */);
}

// disableChip, enableChip -- pass in 
// IRQ ID's after creating the SPI module for the ethernet controller
void enableChip() {
  alt_ic_irq_enable(/*TODO*/);
  // TODO set slave select to low (it's active low)
}


// status -- 1 (HIGH), 0 (LOW)
int digitalWrite(uint8_t whichPin, uint8_t status) {
  if (status != LOW || status != HIGH) return -1;
  if (status == HIGH) {
    *GPIO_PIO |= 1 << whichPin;
  } else {// status == LOW 
    *GPIO_PIO &= ~(1 << whichPin);
  }
  return 0;
}

static void xferSPI (uint8_t data) {

  uint8_t write_data[1] = {data};
		// int alt_avalon_spi_command(alt_u32 base, alt_u32 slave,
    //                         alt_u32 write_length,
    //                        const alt_u8* wdata,
    //                        alt_u32 read_length,
    //                        alt_u8* read_data,
    //                        alt_u32 flags)
    alt_avalon_spi_command( SPI_0_BASE, 
                            ETHERNET_CHIP_SLAVE,
                            1, // write one byte
                            write_data, // write data
                            0, // no read
                            NULL, // don't care about read data
                            0); // no flags
}

static void writeOp (uint8_t op, uint8_t address, uint8_t data) {
    enableChip();
    xferSPI(op | (address & ADDR_MASK));
    xferSPI(data);
    disableChip();
}

static uint8_t readOp (uint8_t op, uint8_t address) {
    enableChip();
    xferSPI(op | (address & ADDR_MASK));
    xferSPI(0x00);
    if (address & 0x80)
        xferSPI(0x00);
    uint8_t result = SPDR;
    disableChip();
    return result;
}

uint8_t ENC28J60::initialize(uint16_t size, const uint8_t *macaddr,
                             uint8_t csPin) {
  bufferSize = size;

  selectPin = csPin;

  // if (bitRead(SPCR, SPE) == 0)
  //   initSPI();

  // pinMode(selectPin, OUTPUT);
  disableChip();

  writeOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
  delay(2); // errata B7/2
  while (!(readOp(ENC28J60_READ_CTRL_REG, ESTAT) & ESTAT_CLKRDY))
    ;

  writeReg(ERXST, RXSTART_INIT);
  writeReg(ERXRDPT, RXSTART_INIT);
  writeReg(ERXND, RXSTOP_INIT);
  writeReg(ETXST, TXSTART_INIT);
  writeReg(ETXND, TXSTOP_INIT);

  // Stretch pulses for LED, LED_A=Link, LED_B=activity
  writePhy(PHLCON, 0x476);

  writeRegByte(ERXFCON,
               ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_PMEN | ERXFCON_BCEN);
  writeReg(EPMM0, 0x303f);
  writeReg(EPMCS, 0xf7f9);
  writeRegByte(MACON1, MACON1_MARXEN);
  writeOp(ENC28J60_BIT_FIELD_SET, MACON3,
          MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN);
  writeReg(MAIPG, 0x0C12);
  writeRegByte(MABBIPG, 0x12);
  writeReg(MAMXFL, MAX_FRAMELEN);
  writeRegByte(MAADR5, macaddr[0]);
  writeRegByte(MAADR4, macaddr[1]);
  writeRegByte(MAADR3, macaddr[2]);
  writeRegByte(MAADR2, macaddr[3]);
  writeRegByte(MAADR1, macaddr[4]);
  writeRegByte(MAADR0, macaddr[5]);
  writePhy(PHCON2, PHCON2_HDLDIS);
  SetBank(ECON1);
  writeOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE | EIE_PKTIE);
  writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);

  uint8_t rev = readRegByte(EREVID);
  // microchip forgot to step the number on the silicon when they
  // released the revision B7. 6 is now rev B7. We still have
  // to see what they do when they release B8. At the moment
  // there is no B8 out yet
  if (rev > 5)
    ++rev;
  return rev;
} 

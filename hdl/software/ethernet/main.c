#include "altera_avalon_spi.h"
#include "altera_avalon_spi_regs.h"
#include "altera_avalon_pio_regs.h"
#include <sys/alt_stdio.h>
#include <sys/alt_irq.h>
#include <stdint.h>
#include <system.h>
#include <stdio.h>
#include <stdlib.h>


#define BANK_MASK 0x60
#define ENC28J60_BIT_FIELD_CLR 0xA0
#define ECON1 0x1F
#define ECON1_BSEL1 0x02
#define ECON1_BSEL0 0x01
#define ENC28J60_BIT_FIELD_SET 0x80
#define ADDR_MASK 0x1F
#define ENC28J60_READ_CTRL_REG 0x00

#define ADDR_MASK        0x1F
#define BANK_MASK        0x60
#define SPRD_MASK        0x80
// All-bank registers
#define EIE              0x1B
#define EIR              0x1C
#define ESTAT            0x1D
#define ECON2            0x1E
#define ECON1            0x1F
// Bank 0 registers
#define ERDPT           (0x00|0x00)
#define EWRPT           (0x02|0x00)
#define ETXST           (0x04|0x00)
#define ETXND           (0x06|0x00)
#define ERXST           (0x08|0x00)
#define ERXND           (0x0A|0x00)
#define ERXRDPT         (0x0C|0x00)
// #define ERXWRPT         (0x0E|0x00)
#define EDMAST          (0x10|0x00)
#define EDMAND          (0x12|0x00)
// #define EDMADST         (0x14|0x00)
#define EDMACS          (0x16|0x00)
// Bank 1 registers
#define EHT0             (0x00|0x20)
#define EHT1             (0x01|0x20)
#define EHT2             (0x02|0x20)
#define EHT3             (0x03|0x20)
#define EHT4             (0x04|0x20)
#define EHT5             (0x05|0x20)
#define EHT6             (0x06|0x20)
#define EHT7             (0x07|0x20)
#define EPMM0            (0x08|0x20)
#define EPMM1            (0x09|0x20)
#define EPMM2            (0x0A|0x20)
#define EPMM3            (0x0B|0x20)
#define EPMM4            (0x0C|0x20)
#define EPMM5            (0x0D|0x20)
#define EPMM6            (0x0E|0x20)
#define EPMM7            (0x0F|0x20)
#define EPMCS           (0x10|0x20)
// #define EPMO            (0x14|0x20)
#define EWOLIE           (0x16|0x20)
#define EWOLIR           (0x17|0x20)
#define ERXFCON          (0x18|0x20)
#define EPKTCNT          (0x19|0x20)
// Bank 2 registers
#define MACON1           (0x00|0x40|0x80)
#define MACON3           (0x02|0x40|0x80)
#define MACON4           (0x03|0x40|0x80)
#define MABBIPG          (0x04|0x40|0x80)
#define MAIPG           (0x06|0x40|0x80)
#define MACLCON1         (0x08|0x40|0x80)
#define MACLCON2         (0x09|0x40|0x80)
#define MAMXFL          (0x0A|0x40|0x80)
#define MAPHSUP          (0x0D|0x40|0x80)
#define MICON            (0x11|0x40|0x80)
#define MICMD            (0x12|0x40|0x80)
#define MIREGADR         (0x14|0x40|0x80)
#define MIWR            (0x16|0x40|0x80)
#define MIRD            (0x18|0x40|0x80)
// Bank 3 registers
#define MAADR1           (0x00|0x60|0x80)
#define MAADR0           (0x01|0x60|0x80)
#define MAADR3           (0x02|0x60|0x80)
#define MAADR2           (0x03|0x60|0x80)
#define MAADR5           (0x04|0x60|0x80)
#define MAADR4           (0x05|0x60|0x80)
#define EBSTSD           (0x06|0x60)
#define EBSTCON          (0x07|0x60)
#define EBSTCS          (0x08|0x60)
#define MISTAT           (0x0A|0x60|0x80)
#define EREVID           (0x12|0x60)
#define ECOCON           (0x15|0x60)
#define EFLOCON          (0x17|0x60)
#define EPAUS           (0x18|0x60)

// ENC28J60 ERXFCON Register Bit Definitions
#define ERXFCON_UCEN     0x80
#define ERXFCON_ANDOR    0x40
#define ERXFCON_CRCEN    0x20
#define ERXFCON_PMEN     0x10
#define ERXFCON_MPEN     0x08
#define ERXFCON_HTEN     0x04
#define ERXFCON_MCEN     0x02
#define ERXFCON_BCEN     0x01
// ENC28J60 EIE Register Bit Definitions
#define EIE_INTIE        0x80
#define EIE_PKTIE        0x40
#define EIE_DMAIE        0x20
#define EIE_LINKIE       0x10
#define EIE_TXIE         0x08
#define EIE_WOLIE        0x04
#define EIE_TXERIE       0x02
#define EIE_RXERIE       0x01
// ENC28J60 EIR Register Bit Definitions
#define EIR_PKTIF        0x40
#define EIR_DMAIF        0x20
#define EIR_LINKIF       0x10
#define EIR_TXIF         0x08
#define EIR_WOLIF        0x04
#define EIR_TXERIF       0x02
#define EIR_RXERIF       0x01
// ENC28J60 ESTAT Register Bit Definitions
#define ESTAT_INT        0x80
#define ESTAT_LATECOL    0x10
#define ESTAT_RXBUSY     0x04
#define ESTAT_TXABRT     0x02
#define ESTAT_CLKRDY     0x01
// ENC28J60 ECON2 Register Bit Definitions
#define ECON2_AUTOINC    0x80
#define ECON2_PKTDEC     0x40
#define ECON2_PWRSV      0x20
#define ECON2_VRPS       0x08
// ENC28J60 ECON1 Register Bit Definitions
#define ECON1_TXRST      0x80
#define ECON1_RXRST      0x40
#define ECON1_DMAST      0x20
#define ECON1_CSUMEN     0x10
#define ECON1_TXRTS      0x08
#define ECON1_RXEN       0x04
#define ECON1_BSEL1      0x02
#define ECON1_BSEL0      0x01
// ENC28J60 MACON1 Register Bit Definitions
#define MACON1_LOOPBK    0x10
#define MACON1_TXPAUS    0x08
#define MACON1_RXPAUS    0x04
#define MACON1_PASSALL   0x02
#define MACON1_MARXEN    0x01
// ENC28J60 MACON3 Register Bit Definitions
#define MACON3_PADCFG2   0x80
#define MACON3_PADCFG1   0x40
#define MACON3_PADCFG0   0x20
#define MACON3_TXCRCEN   0x10
#define MACON3_PHDRLEN   0x08
#define MACON3_HFRMLEN   0x04
#define MACON3_FRMLNEN   0x02
#define MACON3_FULDPX    0x01
// ENC28J60 MICMD Register Bit Definitions
#define MICMD_MIISCAN    0x02
#define MICMD_MIIRD      0x01
// ENC28J60 MISTAT Register Bit Definitions
#define MISTAT_NVALID    0x04
#define MISTAT_SCAN      0x02
#define MISTAT_BUSY      0x01

// ENC28J60 EBSTCON Register Bit Definitions
#define EBSTCON_PSV2     0x80
#define EBSTCON_PSV1     0x40
#define EBSTCON_PSV0     0x20
#define EBSTCON_PSEL     0x10
#define EBSTCON_TMSEL1   0x08
#define EBSTCON_TMSEL0   0x04
#define EBSTCON_TME      0x02
#define EBSTCON_BISTST    0x01

// PHY registers
#define PHCON1           0x00
#define PHSTAT1          0x01
#define PHHID1           0x02
#define PHHID2           0x03
#define PHCON2           0x10
#define PHSTAT2          0x11
#define PHIE             0x12
#define PHIR             0x13
#define PHLCON           0x14

// ENC28J60 PHY PHCON1 Register Bit Definitions
#define PHCON1_PRST      0x8000
#define PHCON1_PLOOPBK   0x4000
#define PHCON1_PPWRSV    0x0800
#define PHCON1_PDPXMD    0x0100
// ENC28J60 PHY PHSTAT1 Register Bit Definitions
#define PHSTAT1_PFDPX    0x1000
#define PHSTAT1_PHDPX    0x0800
#define PHSTAT1_LLSTAT   0x0004
#define PHSTAT1_JBSTAT   0x0002
// ENC28J60 PHY PHCON2 Register Bit Definitions
#define PHCON2_FRCLINK   0x4000
#define PHCON2_TXDIS     0x2000
#define PHCON2_JABBER    0x0400
#define PHCON2_HDLDIS    0x0100

// ENC28J60 Packet Control Byte Bit Definitions
#define PKTCTRL_PHUGEEN  0x08
#define PKTCTRL_PPADEN   0x04
#define PKTCTRL_PCRCEN   0x02
#define PKTCTRL_POVERRIDE 0x01

// SPI operation codes
#define ENC28J60_READ_CTRL_REG       0x00
#define ENC28J60_READ_BUF_MEM        0x3A
#define ENC28J60_WRITE_CTRL_REG      0x40
#define ENC28J60_WRITE_BUF_MEM       0x7A
#define ENC28J60_BIT_FIELD_SET       0x80
#define ENC28J60_BIT_FIELD_CLR       0xA0
#define ENC28J60_SOFT_RESET          0xFF

#define RXSTART_INIT        0x0000
#define RXSTOP_INIT         0x0BFF
#define TXSTART_INIT        0x0C00
#define TXSTOP_INIT         0x11FF

#define MAX_FRAMELEN      1500

#define MAADR1           (0x00|0x60|0x80)
#define MAADR0           (0x01|0x60|0x80)
#define MAADR3           (0x02|0x60|0x80)
#define MAADR2           (0x03|0x60|0x80)
#define MAADR5           (0x04|0x60|0x80)
#define MAADR4           (0x05|0x60|0x80)


static uint8_t Enc28j60Bank;
uint8_t SPDR[1];

static char macaddr[] = {0x74, 0x69, 0x69, 0x2D, 0x30, 0x31};

static void xferSPI (uint8_t data) {
  SPDR[0] = data;
  uint8_t write_data[1];
  write_data[0] = data;
		// int alt_avalon_spi_command(alt_u32 base, alt_u32 slave,
    //                         alt_u32 write_length,
    //                        const alt_u8* wdata,
    //                        alt_u32 read_length,
    //                        alt_u8* read_data,
    //                        alt_u32 flags)
  	 //cout << "starting spi command" << endl;
    alt_avalon_spi_command( SPI_0_BASE,
                            0,
                            1, // write one byte
                            write_data, // write data
                            1, // Read one byte
                            SPDR, // read into SPDR
                            0); // no flags
   //cout << "done spi" << endl;
   //cout << "spdr is now: " << SPDR[0] << endl;
}

void enableChip() {
//  alt_ic_irq_enable(SPI_0_IRQ_INTERRUPT_CONTROLLER_ID, SPI_0_IRQ);
  // TODO set slave select to low (it's active low)
}

void disableChip() {
  // TODO set slave select to high (it's active low)
//  alt_ic_irq_disable(SPI_0_IRQ_INTERRUPT_CONTROLLER_ID, SPI_0_IRQ);
}

static void writeOp (uint8_t op, uint8_t address, uint8_t data) {
    enableChip();
    xferSPI(op | (address & ADDR_MASK));
    xferSPI(data);
    disableChip();
}


static void SetBank (uint8_t address) {
    if ((address & BANK_MASK) != Enc28j60Bank) {
        writeOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_BSEL1|ECON1_BSEL0);
        Enc28j60Bank = address & BANK_MASK;
        writeOp(ENC28J60_BIT_FIELD_SET, ECON1, Enc28j60Bank>>5);
    }
}

static uint8_t readOp (uint8_t op, uint8_t address) {
    enableChip();
    xferSPI(op | (address & ADDR_MASK));
    xferSPI(0x00);
    if (address & 0x80)
        xferSPI(0x00);
    uint8_t result = SPDR[0];
    disableChip();
    return result;
}

static uint8_t readRegByte (uint8_t address) {
    SetBank(address);
    return readOp(ENC28J60_READ_CTRL_REG, address);
}

static void writeRegByte (uint8_t address, uint8_t data) {
    SetBank(address);
    writeOp(ENC28J60_WRITE_CTRL_REG, address, data);
}

static void writeReg(uint8_t address, uint16_t data) {
    writeRegByte(address, data);
    writeRegByte(address + 1, data >> 8);
}


static void writePhy (uint8_t address, uint16_t data) {
    writeRegByte(MIREGADR, address);
    writeReg(MIWR, data);
    printf("starting while2\n");
    while (readRegByte(MISTAT) & MISTAT_BUSY)
        ;

    printf("end while2\n");
}

//int main() {
//	int i = 0;
//    disableChip();
//
//    writeOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
//    for(int j = 0; j < 1000000; j++);
//    printf("starting while1");
//    while (!(readOp(ENC28J60_READ_CTRL_REG, ESTAT) & ESTAT_CLKRDY))
//        ;
//    printf("end while1");
//
//    writeReg(ERXST, RXSTART_INIT);
//    writeReg(ERXRDPT, RXSTART_INIT);
//    writeReg(ERXND, RXSTOP_INIT);
//    writeReg(ETXST, TXSTART_INIT);
//    writeReg(ETXND, TXSTOP_INIT);
//
//    // Stretch pulses for LED, LED_A=Link, LED_B=activity
//    writePhy(PHLCON, 0x476);
//
//    writeRegByte(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN|ERXFCON_BCEN);
//    writeReg(EPMM0, 0x303f);
//    writeReg(EPMCS, 0xf7f9);
//    writeRegByte(MACON1, MACON1_MARXEN);
//    writeOp(ENC28J60_BIT_FIELD_SET, MACON3,
//            MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN);
//    writeReg(MAIPG, 0x0C12);
//    writeRegByte(MABBIPG, 0x12);
//    writeReg(MAMXFL, MAX_FRAMELEN);
//    writeRegByte(MAADR5, macaddr[0]);
//    writeRegByte(MAADR4, macaddr[1]);
//    writeRegByte(MAADR3, macaddr[2]);
//    writeRegByte(MAADR2, macaddr[3]);
//    writeRegByte(MAADR1, macaddr[4]);
//    writeRegByte(MAADR0, macaddr[5]);
//    writePhy(PHCON2, PHCON2_HDLDIS);
//    SetBank(ECON1);
//    writeOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE);
//    writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
//
//
//	while(1) {
//		uint8_t rev = readRegByte(0x00 | 0x60);
//		printf("rev:  %d\n", rev);
//		printf("done (iter %d)\n", i);
//		i++;
//		for(int j = 0; j < 1000000; j++);
//	}
//}

//
//int main() {
//
//	int ret = 0;
//    alt_u8 send_data[1], send_data2[1], send_data3[1], send_data4[1], *get_data, *get_data2;
//    printf("ret2: %d\n",ret);
//    send_data3[0] = 0x00;
//    send_data4[0] = 0x00;
//    send_data[0] = 0xFF;
//    send_data2[0] = 0x00;
//    for(int j = 0; j < 1000000/2; j++);
//    while(1) {
//        get_data = (alt_u8*)malloc(5*sizeof(uint8_t));
//        get_data2 = (alt_u8*)malloc(5*sizeof(uint8_t));
//        ret = alt_avalon_spi_command( SPI_0_BASE,
//                                0,
//                                1, // write one byte
//    							send_data3, // write data
//                                1, // Read one byte
//    							get_data, // read into SPDR
//                                0); // no flags
//        printf("ret3: %d\n",ret);
//        for(int j = 0; j < 1000000/2; j++);
//        ret = alt_avalon_spi_command( SPI_0_BASE,
//                                0,
//                                1, // write one byte
//    							send_data4, // write data
//                                1, // Read one byte
//    							get_data2, // read into SPDR
//                                0); // no flags
//        printf("ret4: %d\n",ret);
//        printf("first: %x %x %x, second: %s\n", get_data[0], get_data[1], get_data[2], get_data2);
//        free(get_data);
//        free(get_data2);
//        printf("reset\n");
//        send_data3[0] = 0xFF;
//
//        for(int j = 0; j < 1000000/2; j++);
//
//        ret = alt_avalon_spi_command( SPI_0_BASE,
//                                0,
//                                1, // write one byte
//    							send_data3, // write data
//                                0, // Read one byte
//    							0, // read into SPDR
//                                0); // no flags
//        printf("ret5: %d\n",ret);
////        for(int j = 0; j < 1000000/2; j++);
//    }
//}

int main() {

	int ret = 0;
    alt_u8 send_data[2], send_data2[1], *get_data;
    printf("ret2: %d\n",ret);

    while(1) {
		send_data[0] = 0x5F; // 0101 1111
		send_data[1] = 0x03;
		for(int j = 0; j < 1000000/2; j++);
		get_data = (alt_u8*)malloc(5*sizeof(uint8_t));

		// write econ1
		ret = alt_avalon_spi_command( SPI_0_BASE,
										0,
										2, // write one byte
										send_data, // write data
										0, // Read one byte
										0, // read into SPDR
										0); // no flags

		send_data2[0] = 0x12;

		printf("ret1: %d\n",ret);
		for(int j = 0; j < 1000000/2; j++);
		ret = alt_avalon_spi_command( SPI_0_BASE,
										0,
										1, // write one byte
										send_data2, // write data
										1, // Read one byte
										get_data, // read into SPDR
										0); // no flags
		printf("ret2: %d\n",ret);
		printf("first: %x \n", get_data[0]);
		free(get_data);
    }

}


//
//int main() {
//
//	int ret = 0;
//    alt_u8 send_data3[1], send_data4[1], *get_data, *get_data2;
//    printf("ret2: %d\n",ret);
//    send_data3[0] = 0x5F;
//    send_data4[0] = 0x03;
//    for(int j = 0; j < 1000000/2; j++);
//	get_data = (alt_u8*)malloc(5*sizeof(uint8_t));
//	get_data2 = (alt_u8*)malloc(5*sizeof(uint8_t));
//	ret = alt_avalon_spi_command( SPI_0_BASE,
//							0,
//							1, // write one byte
//							send_data3, // write data
//							0, // Read one byte
//							get_data, // read into SPDR
//							0); // no flags
//	printf("ret3: %d\n",ret);
//	for(int j = 0; j < 1000000/2; j++);
//	ret = alt_avalon_spi_command( SPI_0_BASE,
//							0,
//							1, // write one byte
//							send_data4, // write data
//							0, // Read one byte
//							get_data2, // read into SPDR
//							0); // no flags
//	for(int j = 0; j < 1000000/2; j++);
//	printf("ret4: %d\n",ret);
//	send_data4[0] = 0x12;
//	ret = alt_avalon_spi_command( SPI_0_BASE,
//							0,
//							1, // write one byte
//							send_data4, // write data
//							1, // Read one byte
//							get_data2, // read into SPDR
//							0); // no flags
//	printf("ret5: %d\n",ret);
//	printf("getdata: %x %x %x", get_data2[0], get_data2[1], get_data2[2]);
//}

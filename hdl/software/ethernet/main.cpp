#include <stdio.h>
#include <system.h>
#include <time.h>
#include "EtherCard/EtherCard.h"

#include "altera_avalon_spi.h"
#include "altera_avalon_spi_regs.h"
#include "altera_avalon_pio_regs.h"
#include "sys/alt_irq.h"

#define HEX_DIGITS_PIO_BASE 0x11100


// ethernet interface mac address, must be unique on the LAN
static char mymac[] = {0x74, 0x69, 0x69, 0x2D, 0x30, 0x31};

//EtherCard ether;

uint8_t Ethernet::buffer[700];

// called when a ping comes in (replies to it are automatic)
static void gotPinged(uint8_t *ptr) { ether.printIp(">>> ping from: ", ptr); }

void printSignedHex0(signed char value) {
	unsigned char tens = 0;
	unsigned char ones = 0;
	unsigned short int pio_val = IORD_ALTERA_AVALON_PIO_DATA(HEX_DIGITS_PIO_BASE);

	value = value % 100;
	tens = value / 10;
	ones = value % 10;

	pio_val &= 0x00FF;
	pio_val |= (tens << 12);
	pio_val |= (ones << 8);

	IOWR_ALTERA_AVALON_PIO_DATA(HEX_DIGITS_PIO_BASE, pio_val);
}

int main() {
  printf("\n[pings]");


  // Change 'SS' to your Slave Select pin, if you arn't using the default pin
  uint16_t sz = sizeof ether.buffer;
  printf("Size: %x\n", sz);
  if (ether.begin(sz, (const uint8_t*)mymac, SS) == 0)
    printf("Failed to access Ethernet controller");

////  return 0;
//   if (!ether.dhcpSetup())
//     printf("DHCP failed");

  const static uint8_t ip[] = {192,168,0,220};
  const static uint8_t gw[] = {192,168,0,1};
  const static uint8_t dns[] = {192,168,0,1};
  const static uint8_t mask[] = {255, 255, 255, 0};


  if(!ether.staticSetup(ip, gw, dns, mask)) {
      // handle failure to configure static IP address (current implementation always returns true!)
	  printf("pain and suffering");
  }

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);

  // use DNS to locate the IP address we want to ping
  if (!ether.dnsLookup("www.google.com"))
    printf("DNS failed");

//  ether.hisip[0] = 192;
//  ether.hisip[1] = 168;
//  ether.hisip[2] = 0;
//  ether.hisip[3] = 124;

  ether.printIp("SRV: ", ether.hisip);

  // call this to report others pinging us
  ether.registerPingCallback(gotPinged);

  //   timer = -9999999; // start timing out right away
  time_t prevTime = time(NULL);

  while (1) {
      uint16_t len = ether.packetReceive(); // go receive new packets
      uint16_t pos = ether.packetLoop(len); // respond to incoming pings

      // report whenever a reply to our outgoing ping comes back
      if (len > 0 && ether.packetLoopIcmpCheckReply(ether.hisip)) {
    	int pingAmnt = (int)(clock() - prevTime);
        printf("   %d ms\n", pingAmnt);
        printSignedHex0(pingAmnt);
      }

      // ping a remote server once every few seconds
      if (clock() > prevTime + 2500) {
        ether.printIp("Pinging: ", ether.hisip);
        prevTime = clock();
        ether.clientIcmpRequest(ether.hisip);
      }
    }
  return 0;
}

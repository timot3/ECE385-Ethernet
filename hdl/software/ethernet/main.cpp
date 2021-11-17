#include <stdio.h>
#include <system.h>
#include <time.h>
//
//int main() {
//    int i = 0;
//    volatile unsigned int *LED_PIO = (unsigned int*)LED_BASE; //make a pointer to access the PIO block
//    volatile unsigned int *GPIO_PIO = (unsigned int*)GPIO_BASE;
//
//    *LED_PIO = 0; //clear all LEDs
//    while ( (1+1) != 3) //infinite loop
//    {
//        for (i = 0; i < 100000; i++); //software delay
//        *LED_PIO |= 0x1; //set LSB
//        *GPIO_PIO = 1;
//        printf("SET\n");
//        for (i = 0; i < 100000; i++); //software delay
//        *LED_PIO &= ~0x1; //clear LSB
//        *GPIO_PIO = 0;
//        printf("CLEAR\n");
//    }
//    return 1; //never gets here
//}


// Ping a remote server, also uses DHCP and DNS.
// 2011-06-12 <jc@wippler.nl>
//
// License: GPLv2

#include "EtherCard/EtherCard.h"


// ethernet interface mac address, must be unique on the LAN
static char mymac[] = {0x74, 0x69, 0x69, 0x2D, 0x30, 0x31};

//EtherCard ether;

uint8_t Ethernet::buffer[700];

// called when a ping comes in (replies to it are automatic)
static void gotPinged(uint8_t *ptr) { ether.printIp(">>> ping from: ", ptr); }

int main() {
  printf("\n[pings]");


  // Change 'SS' to your Slave Select pin, if you arn't using the default pin
  if (ether.begin(sizeof ether.buffer, (const uint8_t*)mymac, SS) == 0)
    printf("Failed to access Ethernet controller");
  // if (!ether.dhcpSetup())
  //   printf("DHCP failed");

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);

  // use DNS to locate the IP address we want to ping
  if (!ether.dnsLookup("www.google.com"))
    printf("DNS failed");

  ether.printIp("SRV: ", ether.hisip);

  // call this to report others pinging us
  ether.registerPingCallback(gotPinged);

  //   timer = -9999999; // start timing out right away
  time_t prevTime = time(NULL);

  while (1) {
    uint16_t len = ether.packetReceive(); // go receive new packets
    uint16_t pos = ether.packetLoop(len); // respond to incoming pings

    // report whenever a reply to our outgoing ping comes back
    if (len > 0 && ether.packetLoopIcmpCheckReply(ether.hisip))
      printf("   %.f ms", difftime(time(NULL), prevTime) * 0.001);

    // ping a remote server once every few seconds
    if (difftime(time(NULL), prevTime) >= 5) {
      ether.printIp("Pinging: ", ether.hisip);
      prevTime = time(NULL);
      ether.clientIcmpRequest(ether.hisip);
    }
  }
  return 0;
}

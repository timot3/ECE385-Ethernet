#include <stdio.h>
#include <system.h>
#include <time.h>
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
   if (!ether.dhcpSetup())
     printf("DHCP failed");

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

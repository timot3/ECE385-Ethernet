// Ping a remote server, also uses DHCP and DNS.
// 2011-06-12 <jc@wippler.nl>
//
// License: GPLv2

#include <EtherCard.h>

#define SS 0x1 // TODO: add slave select pin

// ethernet interface mac address, must be unique on the LAN
static char mymac[] = {0x74, 0x69, 0x69, 0x2D, 0x30, 0x31};

char Ethernet::buffer[700];

// called when a ping comes in (replies to it are automatic)
static void gotPinged(char *ptr) { ether.printIp(">>> ping from: ", ptr); }

void setup() {
  printf("\n[pings]");

  // Change 'SS' to your Slave Select pin, if you arn't using the default pin
  if (ether.begin(sizeof Ethernet::buffer, mymac, SS) == 0)
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
      printf("   %.f ms", difftime(time(NULL), prevTime) * 0.001, 3);

    // ping a remote server once every few seconds
    if (difftime(time(NULL), prevTime) >= 5) {
      ether.printIp("Pinging: ", ether.hisip);
      prevTime = time(NULL);
      ether.clientIcmpRequest(ether.hisip);
    }
  }
}

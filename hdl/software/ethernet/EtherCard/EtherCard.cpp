#include "EtherCard.h"
#include <stdarg.h>
#include <stdio.h>


EtherCard ether;

uint8_t EtherCard::mymac[ETH_LEN];  // my MAC address
uint8_t EtherCard::myip[IP_LEN];   // my ip address
uint8_t EtherCard::netmask[IP_LEN]; // subnet mask
uint8_t EtherCard::broadcastip[IP_LEN]; // broadcast address
uint8_t EtherCard::gwip[IP_LEN];   // gateway
uint8_t EtherCard::dhcpip[IP_LEN]; // dhcp server
uint8_t EtherCard::dnsip[IP_LEN];  // dns server
uint8_t EtherCard::hisip[IP_LEN];  // ip address of remote host
uint16_t EtherCard::hisport = HTTP_PORT; // tcp port to browse to
bool EtherCard::using_dhcp = false;
bool EtherCard::persist_tcp_connection = false;
uint16_t EtherCard::delaycnt = 0; //request gateway ARP lookup

uint8_t EtherCard::begin(const uint16_t size, const uint8_t *macaddr,
                         uint8_t csPin) {
  using_dhcp = false;
  printf("starting begin");
  copyMac(mymac, macaddr);
  printf("copy mac done");
  uint8_t ret = initialize(size, mymac, csPin);
  printf("finished init (%x)", ret);
  return ret;
}

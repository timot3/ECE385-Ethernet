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
  using_dhcp = true;
  copyMac(mymac, macaddr);
  uint8_t ret = initialize(size, mymac, csPin);
  printf("mymac: ");
  for(int i = 0; i < 6; i++)
	  printf("%x, ", mymac[i]);
  printf("\nfinished init (ret: %x)\n", ret);
  return ret;
}

bool EtherCard::staticSetup (const uint8_t* my_ip,
                             const uint8_t* gw_ip,
                             const uint8_t* dns_ip,
                             const uint8_t* mask) {
    using_dhcp = true;

    if (my_ip != 0)
        copyIp(myip, my_ip);
    if (gw_ip != 0)
        setGwIp(gw_ip);
    if (dns_ip != 0)
        copyIp(dnsip, dns_ip);
    if(mask != 0)
        copyIp(netmask, mask);
    updateBroadcastAddress();
    delaycnt = 0; //request gateway ARP lookup
    return true;
}

char* EtherCard::wtoa(uint16_t value, char* ptr)
{
    if (value > 9)
        ptr = wtoa(value / 10, ptr);
    *ptr = '0' + value % 10;
    *++ptr = 0;
    return ptr;
}

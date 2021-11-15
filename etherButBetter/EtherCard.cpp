#include "EtherCard.h"
#include <stdarg.h>

EtherCard ether;

uint8_t EtherCard::begin(const uint16_t size, const uint8_t *macaddr,
                         uint8_t csPin) {
  using_dhcp = false;
  copyMac(mymac, macaddr);
  return initialize(size, mymac, csPin);
}


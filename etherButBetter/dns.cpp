#include "EtherCard.h"
#include "net.h"

#define gPB ether.buffer

static byte dnstid_l; // a counter for transaction ID
#define DNSCLIENT_SRC_PORT_H 0xE0

#define DNS_TYPE_A 1
#define DNS_CLASS_IN 1

bool EtherCard::dnsLookup (const char* name, bool fromRam) {
    uint16_t start = time(NULL);

    while(!isLinkUp())
    {
        if (difftime(time(NULL), start) >= 30000 / 1000)
            return false; //timeout waiting for link
    }
    while(clientWaitingDns())
    {
        packetLoop(packetReceive());
        if (difftime(time(NULL), start) >= 30000 / 1000)
            return false; //timeout waiting for gateway ARP
    }

    memset(hisip, 0, IP_LEN);
    dnsRequest(name, fromRam);

    start = time(NULL);
    while (hisip[0] == 0) {
        if (difftime(time(NULL), start) >= 30000 / 1000)
            return false; //timeout waiting for dns response
        word len = packetReceive();
        if (len > 0 && packetLoop(len) == 0) //packet not handled by tcp/ip packet loop
            if(checkForDnsAnswer(len))
                return false; //DNS response received with error
    }

    return true;
}

#include "EtherCard.h"

void EtherCard::copyIp (uint8_t *dst, const uint8_t *src) {
   memcpy(dst, src, IP_LEN);
}

void EtherCard::printIp (const uint8_t *buf) {
    for (uint8_t i = 0; i < IP_LEN; ++i) {
        printf("%c", buf[i]);
        if (i < 3)
            printf(".");
    }
}

void EtherCard::printIp (const char* msg, const uint8_t *buf) {
    printf("%s", msg);
    EtherCard::printIp(buf);
    printf("\n");
}
#include "EtherCard.h"
#include "net.h"

#define gPB ether.buffer

#define PINGPATTERN 0x42

// Avoid spurious pgmspace warnings - http://forum.jeelabs.net/node/327
// See also http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
//#undef PROGMEM
//#define PROGMEM __attribute__(( section(".progmem.data") ))
//#undef PSTR
//#define PSTR(s) (__extension__({static prog_char c[] PROGMEM = (s); &c[0];}))

#define TCP_STATE_SENDSYN       1
#define TCP_STATE_SYNSENT       2
#define TCP_STATE_ESTABLISHED   3
#define TCP_STATE_NOTUSED       4
#define TCP_STATE_CLOSING       5
#define TCP_STATE_CLOSED        6

#define TCPCLIENT_SRC_PORT_H 11 //Source port (MSB) for TCP/IP client connections - hardcode all TCP/IP client connection from ports in range 2816-3071
static uint8_t tcpclient_src_port_l=1; // Source port (LSB) for tcp/ip client connections - increments on each TCP/IP request
static uint8_t tcp_fd; // a file descriptor, will be encoded into the port
static uint8_t tcp_client_state; //TCP connection state: 1=Send SYN, 2=SYN sent awaiting SYN+ACK, 3=Established, 4=Not used, 5=Closing, 6=Closed
static uint8_t tcp_client_port_h; // Destination port (MSB) of TCP/IP client connection
static uint8_t tcp_client_port_l; // Destination port (LSB) of TCP/IP client connection
static uint8_t (*client_tcp_result_cb)(uint8_t,uint8_t,uint16_t,uint16_t); // Pointer to callback function to handle response to current TCP/IP request
static uint16_t (*client_tcp_datafill_cb)(uint8_t); //Pointer to callback function to handle payload data in response to current TCP/IP request
static uint8_t www_fd; // ID of current http request (only one http request at a time - one of the 8 possible concurrent TCP/IP connections)
static void (*client_browser_cb)(uint8_t,uint16_t,uint16_t); // Pointer to callback function to handle result of current HTTP request
static const char *client_additionalheaderline; // Pointer to c-string additional http request header info
static const char *client_postval;
static const char *client_urlbuf; // Pointer to c-string path part of HTTP request URL
static const char *client_urlbuf_var; // Pointer to c-string filename part of HTTP request URL
static const char *client_hoststr; // Pointer to c-string hostname of current HTTP request
static void (*icmp_cb)(uint8_t *ip); // Pointer to callback function for ICMP ECHO response handler (triggers when localhost receives ping response (pong))
static uint8_t destmacaddr[ETH_LEN]; // storing both dns server and destination mac addresses, but at different times because both are never needed at same time.
static boolean waiting_for_dns_mac = false; //might be better to use bit flags and bitmask operations for these conditions
static boolean has_dns_mac = false;
static boolean waiting_for_dest_mac = false;
static boolean has_dest_mac = false;
static uint8_t gwmacaddr[ETH_LEN]; // Hardware (MAC) address of gateway router
static uint8_t waitgwmac; // Bitwise flags of gateway router status - see below for states
//Define gateway router ARP statuses
#define WGW_INITIAL_ARP 1 // First request, no answer yet
#define WGW_HAVE_GW_MAC 2 // Have gateway router MAC
#define WGW_REFRESHING 4 // Refreshing but already have gateway MAC
#define WGW_ACCEPT_ARP_REPLY 8 // Accept an ARP reply

static uint16_t info_data_len; // Length of TCP/IP payload
static uint8_t seqnum = 0xa; // My initial tcp sequence number
static uint8_t result_fd = 123; // Session id of last reply
static const char* result_ptr; // Pointer to TCP/IP data
static unsigned long SEQ; // TCP/IP sequence number

#define CLIENTMSS 550
#define TCP_DATA_START ((uint16_t)TCP_SRC_PORT_H_P+(gPB[TCP_HEADER_LEN_P]>>4)*4) // Get offset of TCP/IP payload data

const unsigned char arpreqhdr[] PROGMEM = { 0,1,8,0,6,4,0,1 }; // ARP request header
const unsigned char iphdr[] PROGMEM = { 0x45,0,0,0x82,0,0,0x40,0,0x20 }; //IP header
const unsigned char ntpreqhdr[] PROGMEM = { 0xE3,0,4,0xFA,0,1,0,0,0,1 }; //NTP request header
extern const uint8_t allOnes[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }; // Used for hardware (MAC) and IP broadcast addresses


void EtherCard::updateBroadcastAddress() {
   for(uint8_t i=0; i<IP_LEN; i++)
       broadcastip[i] = myip[i] | ~netmask[i];
}

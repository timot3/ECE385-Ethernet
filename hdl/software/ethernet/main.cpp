#include <stdio.h>
#include <system.h>

// Set to 0 to disable keyboard code
#define USING_KEYBOARD 0

// 0x00 = pinging, 0x01 = fetch data from website, 0x02 = host uptime website, 0x03 = post request
// 0x04 = get user input from site, 0x05 = set alma lights from site (not working)
#define PROG_NUM 0x05

#include "EtherCard/EtherCard.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_spi.h"
#include "altera_avalon_spi_regs.h"
#include "sys/alt_irq.h"
#include <time.h>

#if USING_KEYBOARD
extern "C" {
  #include "usb_kb/GenericMacros.h"
  #include "usb_kb/GenericTypeDefs.h"
  #include "usb_kb/HID.h"
  #include "usb_kb/MAX3421E.h"
  #include "usb_kb/USB.h"
  #include "usb_kb/transfer.h"
  #include "usb_kb/usb_ch9.h"
}
#endif

#if PROG_NUM == 0x00
// ethernet interface mac addreBss, must be unique on the LAN
static char mymac[] = {0x74, 0x69, 0x69, 0x2D, 0x30, 0x31};
//static char mymac[] = {0x74, 0x00, 0x69, 0x2D, 0x30, 0x31};

uint8_t Ethernet::buffer[700];

#if USING_KEYBOARD
extern HID_DEVICE hid_device;

static BYTE addr = 1; // hard-wired USB address
const char *const devclasses[] = {" Uninitialized", " HID Keyboard",
                                  " HID Mouse", " Mass storage"};

BYTE GetDriverandReport() {
  BYTE i;
  BYTE rcode;
  BYTE device = 0xFF;
  BYTE tmpbyte;

  DEV_RECORD *tpl_ptr;
  printf("Reached USB_STATE_RUNNING (0x40)\n");
  for (i = 1; i < USB_NUMDEVICES; i++) {
    tpl_ptr = GetDevtable(i);
    if (tpl_ptr->epinfo != NULL) {
      printf("Device: %d", i);
      printf("%s \n", devclasses[tpl_ptr->devclass]);
      device = tpl_ptr->devclass;
    }
  }
  // Query rate and protocol
  rcode = XferGetIdle(addr, 0, hid_device.interface, 0, &tmpbyte);
  if (rcode) { // error handling
    printf("GetIdle Error. Error code: ");
    printf("%x \n", rcode);
  } else {
    printf("Update rate: ");
    printf("%x \n", tmpbyte);
  }
  printf("Protocol: ");
  rcode = XferGetProto(addr, 0, hid_device.interface, &tmpbyte);
  if (rcode) { // error handling
    printf("GetProto Error. Error code ");
    printf("%x \n", rcode);
  } else {
    printf("%d \n", tmpbyte);
  }
  return device;
}

void setKeycode(WORD keycode) {
  IOWR_ALTERA_AVALON_PIO_DATA(KEYCODE_BASE, keycode);
}
#endif

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

#if USING_KEYBOARD
  BYTE rcode;
  BOOT_MOUSE_REPORT buf; // USB mouse report
  BOOT_KBD_REPORT kbdbuf;

  BYTE runningdebugflag = 0; // flag to dump out a bunch of information when we
                             // first get to USB_STATE_RUNNING
  BYTE errorflag = 0; // flag once we get an error device so we don't keep
                      // dumping out state info
  BYTE device;
  WORD keycode;

  printf("initializing MAX3421E...\n");
  MAX3421E_init();
  printf("initializing USB...\n");
  USB_init();
#endif

  uint16_t sz = sizeof ether.buffer;
  printf("Size: %x\n", sz);
  if (ether.begin(sz, (const uint8_t *)mymac, SS) == 0)
    printf("Failed to access Ethernet controller");

  ////  return 0;
  if (!ether.dhcpSetup())
    printf("DHCP failed");

  const static uint8_t ip[] = {192, 168, 0, 220};
  const static uint8_t gw[] = {192, 168, 0, 1};
  const static uint8_t dns[] = {192, 168, 0, 1};
  const static uint8_t mask[] = {255, 255, 255, 0};

//  if (!ether.staticSetup(ip, gw, dns, mask)) {
//    // handle failure to configure static IP address (current implementation
//    // always returns true!)
//    printf("pain and suffering");
//  }

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);

  // use DNS to locate the IP address we want to ping
  if (!ether.dnsLookup("www.4chan.org"))
    printf("DNS failed");
  //  ether.hisip[0] = 34;
  //  ether.hisip[1] = 222;
  //  ether.hisip[2] = 248;
  //  ether.hisip[3] = 68;

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

#if USING_KEYBOARD
    printf(".");
    MAX3421E_Task();
    USB_Task();
    if (GetUsbTaskState() == USB_STATE_RUNNING) {
      if (!runningdebugflag) {
        runningdebugflag = 1;
        printf("SET LED\n");
        device = GetDriverandReport();
      } else if (device == 1) {
        // run keyboard debug polling
        rcode = kbdPoll(&kbdbuf);
        if (rcode == hrNAK) {
          continue; // NAK means no new data
        } else if (rcode) {
          printf("Rcode: ");
          printf("%x \n", rcode);
          continue;
        }
        printf("keycodes: ");
        for (int i = 0; i < 6; i++) {
          printf("%x ", kbdbuf.keycode[i]);
        }
        setKeycode(kbdbuf.keycode[0]);
        printf("\n");
      }

      else if (device == 2) {
        rcode = mousePoll(&buf);
        if (rcode == hrNAK) {
          // NAK means no new data
          continue;
        } else if (rcode) {
          printf("Rcode: ");
          printf("%x \n", rcode);
          continue;
        }
        printf("X displacement: ");
        printf("%d ", (signed char)buf.Xdispl);
        printf("Y displacement: ");
        printf("%d ", (signed char)buf.Ydispl);
        printf("Buttons: ");
        printf("%x\n", buf.button);
      }
    } else if (GetUsbTaskState() == USB_STATE_ERROR) {
      if (!errorflag) {
        errorflag = 1;
        printf("USB Error State\n");
        // print out string descriptor here
      }
    } else // not in USB running state
    {

      printf("USB task state: ");
      printf("%x\n", GetUsbTaskState());
      if (runningdebugflag) { // previously running, reset USB hardware just to
                              // clear out any funky state, HS/FS etc
        runningdebugflag = 0;
        MAX3421E_init();
        USB_init();
      }
      errorflag = 0;
    }
#endif
  }
  return 0;
}

#elif PROG_NUM == 0x01
// Demo using DHCP and DNS to perform a web client request.
// 2011-06-08 <jc@wippler.nl>
//
// License: GPLv2
//
// ethernet interface mac address, must be unique on the LAN
static char mymac[] = {0x74, 0x69, 0x69, 0x2D, 0x30, 0x31};

byte Ethernet::buffer[700];
static uint32_t timer;

const char website[] = "www.google.com";

// called when the client request is complete
static void my_callback(byte status, uint16_t off, uint16_t len) {
  printf(">>>\n");
  Ethernet::buffer[off + len] = 0;
  printf("status:%d, len:%d\n", status, len);
  printf("%s...\n", (const char *)Ethernet::buffer + off);
}

int main() {
  printf("\n[webClient]\n");

  uint16_t sz = sizeof ether.buffer;
  if (ether.begin(sz, (const uint8_t *)mymac, SS) == 0)
    printf("Failed to access Ethernet controller");
  printf("init");
  //  if (!ether.dhcpSetup())
  //    printf("DHCP failed\n");

  const static uint8_t ip[] = {192, 168, 0, 220};
  const static uint8_t gw[] = {192, 168, 0, 1};
  const static uint8_t dns[] = {192, 168, 0, 1};
  const static uint8_t mask[] = {255, 255, 255, 0};

  if (!ether.staticSetup(ip, gw, dns, mask)) {
    // handle failure to configure static IP address (current
    // implementation always returns true!)
    printf("pain and suffering");
  }

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);

  // use DNS to resolve the website's IP address
  if (!ether.dnsLookup(website))
    printf("DNS failed\n");

  ether.printIp("SRV: ", ether.hisip);

  while (1) {
    ether.packetLoop(ether.packetReceive());

    if (clock() > timer) {
      timer = clock() + 5000;
      printf("\n");
      printf("<<< REQ ");
      ether.browseUrl("/", "", website, my_callback);
    }
  }

  return 0;
}

#elif PROG_NUM == 0x02
// This is a demo of the RBBB running as webserver with the EtherCard
// 2010-05-28 <jc@wippler.nl>
//
// License: GPLv2

#include "EtherCard/bufferfiller.h"

// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = {0x74, 0x69, 0x69, 0x2D, 0x30, 0x31};
static byte myip[] = {192, 168, 0, 220};

byte Ethernet::buffer[2000];
BufferFiller bfill;
char keyPressedArr[50];
int locInArr = 0;

#if USING_KEYBOARD
extern HID_DEVICE hid_device;

static BYTE addr = 1; // hard-wired USB address
const char *const devclasses[] = {" Uninitialized", " HID Keyboard",
                                  " HID Mouse", " Mass storage"};

static char chars[57] = {'0','0','0','0','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','1','2','3','4','5','6','7','8','9','0','\n','0','\b','\t',' ','-','=','[',']','\\','0',';','\'','`',',','.'};

BYTE GetDriverandReport() {
  BYTE i;
  BYTE rcode;
  BYTE device = 0xFF;
  BYTE tmpbyte;

  DEV_RECORD *tpl_ptr;
  printf("Reached USB_STATE_RUNNING (0x40)\n");
  for (i = 1; i < USB_NUMDEVICES; i++) {
    tpl_ptr = GetDevtable(i);
    if (tpl_ptr->epinfo != NULL) {
      printf("Device: %d", i);
      printf("%s \n", devclasses[tpl_ptr->devclass]);
      device = tpl_ptr->devclass;
    }
  }
  // Query rate and protocol
  rcode = XferGetIdle(addr, 0, hid_device.interface, 0, &tmpbyte);
  if (rcode) { // error handling
    printf("GetIdle Error. Error code: ");
    printf("%x \n", rcode);
  } else {
    printf("Update rate: ");
    printf("%x \n", tmpbyte);
  }
  printf("Protocol: ");
  rcode = XferGetProto(addr, 0, hid_device.interface, &tmpbyte);
  if (rcode) { // error handling
    printf("GetProto Error. Error code ");
    printf("%x \n", rcode);
  } else {
    printf("%d \n", tmpbyte);
  }
  return device;
}

void setKeycode(WORD keycode) {
  IOWR_ALTERA_AVALON_PIO_DATA(KEYCODE_BASE, keycode);
}
#endif

static uint16_t homePage() {
  long t = clock() / 1000;
  uint16_t h = t / 3600;
  byte m = (t / 60) % 60;
  byte s = t % 60;
  bfill = ether.tcpOffset();
  bfill.emit_p("HTTP/1.0 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Pragma: no-cache\r\n"
                "\r\n"
                "<meta http-equiv='refresh' content='10'/>"
                "<title>ECE 385 FPGA Server</title>"
                "<center><h1>Uptime $D$D:$D$D:$D$D</h1>"
                "<h3>last 50 keys pressed: $S$</h3>"
                "<br><br>"
                "<p>ECE 385 FA21 Final Project - Ethernet on DE10-Lite</p>"
                "<p>Hassan Farooq, Tim Vitkin</p>"
                "<p><small>(auto refresh every 10s)</small></p></center>",
                h / 10, h % 10, m / 10, m % 10, s / 10, s % 10, keyPressedArr);

  return bfill.position();
}

int main() {
  printf("\n[RBBB Server]\n");

#if USING_KEYBOARD
  BYTE rcode;
  BOOT_KBD_REPORT kbdbuf;

  BYTE runningdebugflag = 0; // flag to dump out a bunch of information when we
                             // first get to USB_STATE_RUNNING
  BYTE errorflag = 0; // flag once we get an error device so we don't keep
                      // dumping out state info
  BYTE device;
  WORD keycode;

  printf("initializing MAX3421E...\n");
  MAX3421E_init();
  printf("initializing USB...\n");
  USB_init();
#endif

  // Change 'SS' to your Slave Select pin, if you arn't using the default pin
  if (ether.begin(sizeof Ethernet::buffer, mymac, SS) == 0)
    printf("Failed to access Ethernet controller\n");
  ether.staticSetup(myip);
  while (1) {
    uint16_t len = ether.packetReceive();
    uint16_t pos = ether.packetLoop(len);

    if (pos)                             // check if valid tcp data is received
      ether.httpServerReply(homePage()); // send web page data

#if USING_KEYBOARD
    MAX3421E_Task();
    USB_Task();
    if (GetUsbTaskState() == USB_STATE_RUNNING) {
      if (!runningdebugflag) {
        runningdebugflag = 1;
        device = GetDriverandReport();
      } else if (device == 1) {
        // run keyboard debug polling
        rcode = kbdPoll(&kbdbuf);
        if (rcode == hrNAK) {
          continue; // NAK means no new data
        } else if (rcode) {
          continue;
        }

        if (kbdbuf.keycode[0] > 0 && kbdbuf.keycode[0] < 57) {
        	keyPressedArr[locInArr] = chars[kbdbuf.keycode[0]];
        	locInArr++;
        	locInArr %= 50;
        }

        setKeycode(kbdbuf.keycode[0]);
      }
    } else if (GetUsbTaskState() == USB_STATE_ERROR) {
      if (!errorflag) {
        errorflag = 1;
        printf("USB Error State\n");
        // print out string descriptor here
      }
    } else { // not in USB running state

      printf("USB task state: ");
      printf("%x\n", GetUsbTaskState());
      if (runningdebugflag) { // previously running, reset USB hardware just to
                              // clear out any funky state, HS/FS etc
        runningdebugflag = 0;
        MAX3421E_init();
        USB_init();
      }
      errorflag = 0;
    }
#endif
  }

  return 0;
}

#elif PROG_NUM == 0x03

// Using
// https://stackoverflow.com/questions/49244535/arduino-uno-post-data-using-enc28j60
// as reference
#define REQUEST_RATE 2500 // milliseconds
#define API_PORT 80

#include <stdlib.h>

const char website[] = "www.alma.lol";
//const char website[] = "192.168.0.124";
static char mymac[] = {0x74, 0x69, 0x69, 0x2D, 0x30, 0x31};

byte Ethernet::buffer[500];
static long timer;

// called when the client request is complete
static void my_result_cb(byte status, uint16_t off, uint16_t len) {
  printf("%d ms\n", (clock() - timer));
  printf("%s\n", (const char *)Ethernet::buffer + off);
}

int main() {
  printf("\n[getStaticIP]\n");

  uint16_t sz = sizeof ether.buffer;
  printf("Size: %x\n", sz);
  if (ether.begin(sz, (const uint8_t *)mymac, SS) == 0)
    printf("Failed to access Ethernet controller");

  const static uint8_t ip[] = {192, 168, 0, 220};
  const static uint8_t gw[] = {192, 168, 0, 1};
  const static uint8_t dns[] = {192, 168, 0, 1};
  const static uint8_t mask[] = {255, 255, 255, 0};

  if (!ether.staticSetup(ip, gw, dns, mask)) {
    // handle failure to configure static IP address (current implementation
    // always returns true!)
    printf("pain and suffering");
  }

//   ether.hisip[0] = 192;
//   ether.hisip[1] = 168;
//   ether.hisip[2] = 0;
//   ether.hisip[3] = 124;

  if (!ether.dnsLookup(website))
    printf("DNS failed");

  ether.printIp("My IP: ", ether.myip);
  ether.printIp("Netmask: ", ether.netmask);
  ether.printIp("GW IP: ", ether.gwip);
  ether.printIp("DNS IP: ", ether.dnsip);
  ether.printIp("SRV: ", ether.hisip);

  ether.hisport = API_PORT;

  timer = -REQUEST_RATE; // start timing out right away

  while (1) {
    ether.packetLoop(ether.packetReceive());

    if (clock() > timer + REQUEST_RATE) {
      timer = clock();
      printf("\n>>> REQ SENDING \n");
      int r = rand() % 256, g = rand() % 256, b = rand() % 256;
      char sendStr[146];
      sprintf(sendStr, "{\"color\": \"{\\\"senderUID\\\":\\\"10000000\\\",\\\"receiverUID\\\":\\\"10203FFF\\\",\\\"functionID\\\":\\\"0\\\",\\\"data\\\":[%d,%d,%d]}\"}", r, g, b);
      printf("r:%d, g:%d, b:%d\n", r, g, b);
      ether.httpPost("/sendCommand", website, "Content-Type: application/json",
                     sendStr, my_result_cb);
      printf("done\n");
    }
  }

  return 0;
}

#elif PROG_NUM == 0x04

#include "EtherCard/bufferfiller.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <string>

static byte mymac[] = {0x74, 0x69, 0x69, 0x2D, 0x30, 0x31};

byte Ethernet::buffer[2000];
BufferFiller bfill;
char keyPressedArr[50];
int locInArr = 0;

static uint16_t homePage() {
  long t = clock() / 1000;
  uint16_t h = t / 3600;
  byte m = (t / 60) % 60;
  byte s = t % 60;
  bfill = ether.tcpOffset();
  bfill.emit_p("HTTP/1.0 200 OK\r\n"
               "Content-Type: text/html\r\n"
               "Pragma: no-cache\r\n"
               "\r\n"
               "<title>ECE 385 FPGA Server</title>"
               "<center><form>"
               "<label for=\"query\"></label>"
               "<input type=\"text\" id=\"DATA\" name=\"DATA\" "
               "placeholder=\"Enter some text here...\"><br><br>"
               "<input type=\"submit\" value=\"Send data\">"
               "</form>"
               "</center>",
               h / 10, h % 10, m / 10, m % 10, s / 10, s % 10, keyPressedArr);

  return bfill.position();
}

int main() {
  printf("\n[User Data Server]\n");

  // Change 'SS' to your Slave Select pin, if you arn't using the default pin
  if (ether.begin(sizeof Ethernet::buffer, mymac, SS) == 0)
    printf("Failed to access Ethernet controller\n");

  const static uint8_t ip[] = {192, 168, 0, 220};
  const static uint8_t gw[] = {192, 168, 0, 1};
  const static uint8_t dns[] = {192, 168, 0, 1};
  const static uint8_t mask[] = {255, 255, 255, 0};

  if (!ether.staticSetup(ip, gw, dns, mask)) {
    // handle failure to configure static IP address (current implementation
    // always returns true!)
    printf("pain and suffering");
  }

  printf("starting while loop\n");
  while (1) {
    uint16_t len = ether.packetReceive();
    uint16_t pos = ether.packetLoop(len);

    if (pos) { // check if valid tcp data is received
      if (strstr((char *)Ethernet::buffer + pos, "GET /?DATA=")) {
        std::string input((const char *)(Ethernet::buffer + pos + 11));
        std::string endStr = " HTTP/1.1";
        unsigned last = input.find(endStr);
        input = input.substr(0, last);

        printf("User input: %s\n", input.c_str());
      }
      ether.httpServerReply(homePage()); // send web page data
    }
  }

  return 0;
}

#elif PROG_NUM == 0x05

// doesn't work properly

#include "EtherCard/bufferfiller.h"

// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = {0x74, 0x69, 0x69, 0x2D, 0x30, 0x31};
static byte myip[] = {192, 168, 0, 220};

byte Ethernet::buffer[2000];
BufferFiller bfill;
char keyPressedArr[50];
int locInArr = 0;


static uint16_t homePage() {
  bfill = ether.tcpOffset();
  bfill.emit_p("HTTP/1.0 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Pragma: no-cache\r\n"
                "\r\n"
                "<title>ECE 385 FPGA Server</title>"
                "<input id=n1 type=number value=255> <input id=n2 type=number value=255> <input id=n3 type=number value=255>"
                "<button onclick=mF()>Send Color</button><script>function mF() {fetch('http://www.alma.lol/sendCommand', {method: 'POST',body:"
                " \"{\\\"color\\\": \\\"{\\\\\\\"senderUID\\\\\\\":\\\\\\\"10000000\\\\\\\",\\\\\\\"receiverUID\\\\\\\":\\\\\\\"10203FFF\\\\\\\""
                ",\\\\\\\"functionID\\\\\\\":\\\\\\\"0\\\\\\\",\\\\\\\"data\\\\\\\":[\"+parseInt(document.getElementById(\"n1\").value)+\",\"+"
                "parseInt(document.getElementById(\"n2\").value)+\",\"+parseInt(document.getElementById(\"n3\").value)+\"]}\\\"}\",headers: "
                "{'Content-type': 'application/json; charset=UTF-8'}}).then(response => response.json()).then(json => {console.log(json);});}</script>");

  return bfill.position();
}

int main() {
  printf("\n[RBBB Server]\n");


  // Change 'SS' to your Slave Select pin, if you arn't using the default pin
  if (ether.begin(sizeof Ethernet::buffer, mymac, SS) == 0)
    printf("Failed to access Ethernet controller\n");
  ether.staticSetup(myip);
  while (1) {
    uint16_t len = ether.packetReceive();
    uint16_t pos = ether.packetLoop(len);

    if (pos)                             // check if valid tcp data is received
      ether.httpServerReply(homePage()); // send web page data
  }

  return 0;
}










#endif

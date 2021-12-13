# ECE385-Ethernet
ECE 385 Final Project -- Ethernet on MAX10-DE10 Lite FPGA

This is a port of the [EtherCard](https://github.com/njh/EtherCard) library to run on the Nios II soft processor.


## To Run
Connect custom shield to FPGA, connect ethernet and keyboard (depending on which test program you're running)

Program FPGA through Quartus like normal. 

To run the C++ code (and all the networking stuff), select the program number you'd like to run via the macro near the top of main.cpp. If the program allows keyboard input and you'd like to use it, set the keyboard macro to 1.



## Introduction to SPI
SPI, or serial peripheral interface, uses a bus with four signals (SCLK, MOSI, MISO, and CS/SS) in order to communicate with the processor. The chip controllers connected to the SPI bus are synchronized with the processor. The processor is often referred to as a master device, as it controls the devices on the SPI bus. Likewise, the other devices on the SPI bus are slave devices. SCLK is the clock from the master (processor) in order to synchronize the devices connected via SPI. MOSI is Master out, slave in, which is data sent from the master to the slave. MISO is master in, slave out, which is the data coming from the slave or connected device. SS/CS is the chip select to determine what controller on the SPI bus the master is communicating with.
 
## Ethernet
The Ethernet driver we used was based on an Arduino ENC28J60 library called [EtherCard](https://github.com/njh/EtherCard). It provides support for DHCP, DNS lookup, and basic HTTP support. Using example programs, we can do things such as ping websites, host a static webpage on the FPGA, make POST/GET requests to APIs, fetch HTML data from webpages, and more. We were able to successfully port the entire library to the Nios II. 

### Porting Process 
We knew there would need to change all Arduino specific code to code supported by the Nios II. This included print statements (Arduino uses Serial.println() vs printf()), SPI commands (Arduino uses specific register values vs Avalon commands on Nios II), memory access (Arduino has RAM specific commands for memcpy, etc.), and more. 

Initially we pasted the minimum necessary files from the original library into Eclipse, going through all syntax errors and fixing them. We also commented out all SPI related functions during this stage. We then went through and converted all SPI related functions to use the alt_avalon_spi_command() function rather than the Arduino method of reading/setting various registers in a while loop. 

Once this was done, we wrote a basic main function to see if we were able to read and set certain registers as per how the datasheet for the ENC28J60 chip specified. We noticed that we were able to read/write to most registers correctly, but certain ones had issues. After going through our code and the datasheet multiple times, we noticed our readOp() function was slightly incorrect and was returning the wrong value. Similarly our readBuf() function was running two SPI commands where CS went high (is active low) in between which cancelled the command, returning bad data. Once fixed, we noticed our register reads and writes worked as expected. 

We also ran into issues with DHCP. We first tested using a static IP which was set on both our code as well as the router we were connected to since DHCP was not working. Initially we thought our buffer was being incorrectly read from the ENC28J60 chip. But, we deduced that the data being received in the buffer was correct (as we were able to successfully complete other commands such as pinging and fetching data using a static IP), and so something else must be wrong. When tracing through the code, we noticed that the DHCPdata struct used in one of the functions to initialize DHCP was not filling correctly. After adding the __packed__ attribute to this struct, we saw that DHCP initialization was able to complete successfully. 

After porting over a few test programs from the original library, we saw them all function as expected and thus conclude that our port works as intended. 

## The ENC28J60 Class
The ENC28J60 Class, mostly referred to as Ethernet inside the code,  provides an interface with the hardware of the ENC chip. The most notable class variable is the buffer[] array that maintains the current ethernet buffer of the chip. This data buffer is shared by receive and transmit functions of the ENC chip. Due to this limitation, the chip cannot receive and transmit simultaneously. 

The most important functions of this class are init(), packetReceive(), and packetSend(). init() initializes all the necessary registers on the ENC chip to ensure proper operation. It does this with the writeReg function as described below. packetReceive(), and packetSend() are, as their names suggest, functions to process incoming and outgoing packets between the Nios II processor and the ENC chip. 

### Important functions in the ENC28J60 class
#### writeOp(), readOp()
Per the documentation, reading/writing registers of the ENC chip is done through sending a command on the SPI bus. The command must contain an operation and an address followed by data to write if applicable. These functions are not part of the ENC28J60 class due to data privacy reasons, and because they are not used outside the enc28j60.cpp file.

The ENC chip has three register banks. In order to perform an operation requiring register access, the ENC chip requires a register bank to be set. This is done with the SetBank() function, which writes the operation to set the bank. 

#### writeReg(), readReg()
These commands first set the proper register bank based on the address that was passed in, then call either writeOp or readOp, respectively, and return data if applicable.

#### writeBuf(), readBuf()
These commands write to and read from the ENC chip’s Ethernet buffer. 

## The EtherCard Class
The EtherCard class extends the ENC28J60 class because it provides access to the higher-level components of the TCP/IP Stack while abstracting away the interactions with the registers of the ENC chip. Most of the functionality of this class is implemented inside the tcpip.cpp file. 

Most of the functions fill in the buffer of the EtherCard class with various parameters necessary to either send or receive a packet. 

### The DHCP State Machine
The DHCP State machine is used in order to initialize DHCP and for the device to receive a dynamically allocated IP address. It has the following four states:
#### DHCP_STATE_INIT
The initial state of the state machine. Sets our IP to all 0’s, enables broadcast mode and sets the state to DHCP_STATE_SELECTING.
#### DHCP_STATE_SELECTING
Waits until a message with a DHCP IP offer has been received from the router. Once one has, the offer is then processed, a packet acknowledging this is sent, and the state is changed to DHCP_STATE_REQUESTING. If an offer is not received within 10 seconds, we timeout as some error likely occurred and move back to the DHCP_STATE_INIT state. 
#### DHCP_STATE_REQUESTING/DHCP_STATE_RENEWING
Once we receive an acknowledgement via DHCP, broadcasting is disabled, and we set the gateway IP assuming it has not already been set. Our state is also then set to DHCP_STATE_BOUND. If we did not receive an acknowledgement within 10 seconds, we again timeout as some error likely occurred and move back to the DHCP_STATE_INIT state. 
#### DHCP_STATE_BOUND
The final state in the state machine. Nothing occurs assuming our lease time is not over yet. If it is, we send a DHCP message with our current IP and set our state to DHCP_STATE_RENEWING. 


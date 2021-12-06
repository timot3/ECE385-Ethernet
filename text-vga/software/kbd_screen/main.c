#include <stdlib.h>
#include <string.h>
#include <alt_types.h>


//ECE 385 USB Host Shield code
//based on Circuits-at-home USB Host code 1.x
//to be used for ECE 385 course materials
//Revised October 2020 - Zuofu Cheng

#include <stdio.h>
#include <system.h>
#include "altera_avalon_spi.h"
#include "altera_avalon_spi_regs.h"
#include "altera_avalon_pio_regs.h"
#include "sys/alt_irq.h"
#include "usb_kb/GenericMacros.h"
#include "usb_kb/GenericTypeDefs.h"
#include "usb_kb/HID.h"
#include "usb_kb/MAX3421E.h"
#include "usb_kb/transfer.h"
#include "usb_kb/usb_ch9.h"
#include "usb_kb/USB.h"

#include "text_mode_vga_color.h"


static char chars[57] = {' ',' ',' ',' ','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','1','2','3','4','5','6','7','8','9','0','\n','0','\b','\t',' ','-','=','[',']','\\','0',';','\'','`',',','.'};

char kbd_buf[BUF_LEN];
int buf_x, buf_y;

extern HID_DEVICE hid_device;

static BYTE addr = 1; 				//hard-wired USB address
const char* const devclasses[] = { " Uninitialized", " HID Keyboard", " HID Mouse", " Mass storage" };

BYTE GetDriverandReport() {
	BYTE i;
	BYTE rcode;
	BYTE device = 0xFF;
	BYTE tmpbyte;

	DEV_RECORD* tpl_ptr;
	printf("Reached USB_STATE_RUNNING (0x40)\n");
	for (i = 1; i < USB_NUMDEVICES; i++) {
		tpl_ptr = GetDevtable(i);
		if (tpl_ptr->epinfo != NULL) {
			printf("Device: %d", i);
			printf("%s \n", devclasses[tpl_ptr->devclass]);
			device = tpl_ptr->devclass;
		}
	}
	//Query rate and protocol
	rcode = XferGetIdle(addr, 0, hid_device.interface, 0, &tmpbyte);
	if (rcode) {   //error handling
		printf("GetIdle Error. Error code: ");
		printf("%x \n", rcode);
	} else {
		printf("Update rate: ");
		printf("%x \n", tmpbyte);
	}
	printf("Protocol: ");
	rcode = XferGetProto(addr, 0, hid_device.interface, &tmpbyte);
	if (rcode) {   //error handling
		printf("GetProto Error. Error code ");
		printf("%x \n", rcode);
	} else {
		printf("%d \n", tmpbyte);
	}
	return device;
}

//void setLED(int LED) {
//	IOWR_ALTERA_AVALON_PIO_DATA(LEDS_PIO_BASE,
//			(IORD_ALTERA_AVALON_PIO_DATA(LEDS_PIO_BASE) | (0x001 << LED)));
//}
//
//void clearLED(int LED) {
//	IOWR_ALTERA_AVALON_PIO_DATA(LEDS_PIO_BASE,
//			(IORD_ALTERA_AVALON_PIO_DATA(LEDS_PIO_BASE) & ~(0x001 << LED)));
//
//}

void printSignedHex0(signed char value) {
	BYTE tens = 0;
	BYTE ones = 0;
	WORD pio_val = IORD_ALTERA_AVALON_PIO_DATA(HEX_DIGITS_PIO_BASE);
	if (value < 0) {
//		setLED(11);
		value = -value;
	} else {
//		clearLED(11);
	}
	//handled hundreds
//	if (value / 100)
//		setLED(13);
//	else
//		clearLED(13);

	value = value % 100;
	tens = value / 10;
	ones = value % 10;

	pio_val &= 0x00FF;
	pio_val |= (tens << 12);
	pio_val |= (ones << 8);

	IOWR_ALTERA_AVALON_PIO_DATA(HEX_DIGITS_PIO_BASE, pio_val);
}

void printSignedHex1(signed char value) {
	BYTE tens = 0;
	BYTE ones = 0;
	DWORD pio_val = IORD_ALTERA_AVALON_PIO_DATA(HEX_DIGITS_PIO_BASE);
	if (value < 0) {
//		setLED(10);
		value = -value;
	}
//	else {
//		clearLED(10);
//	}
	//handled hundreds
//	if (value / 100)
//		setLED(12);
//	else
//		clearLED(12);

	value = value % 100;
	tens = value / 10;
	ones = value % 10;
	tens = value / 10;
	ones = value % 10;

	pio_val &= 0xFF00;
	pio_val |= (tens << 4);
	pio_val |= (ones << 0);

	IOWR_ALTERA_AVALON_PIO_DATA(HEX_DIGITS_PIO_BASE, pio_val);
}

void setKeycode(WORD keycode)
{

	IOWR_ALTERA_AVALON_PIO_DATA(KEYCODE_BASE, keycode);
}
void handleKeypress(WORD keycode) {
	if (keycode != 0) {
		printf("Keycode: %d, x: %d, y: %d", keycode, buf_x, buf_y);
		kbd_buf[buf_x ] = chars[keycode];
		textVGADrawLetter(kbd_buf[buf_x], buf_x, buf_y);
		buf_x++;
		if (buf_x >= BUF_LEN) {
			buf_x %= BUF_LEN;
			buf_y++;
		}
	}
}
int main() {
	BYTE rcode;
	BOOT_MOUSE_REPORT buf;		//USB mouse report
	BOOT_KBD_REPORT kbdbuf;

	BYTE runningdebugflag = 0;//flag to dump out a bunch of information when we first get to USB_STATE_RUNNING
	BYTE errorflag = 0; //flag once we get an error device so we don't keep dumping out state info
	BYTE device;
	WORD keycode;

//	for(int i = 0; i < 100000000; i++){
//		printf("%d\n", i);
//	}
	// clear kbd buf
	buf_x = 0;
	buf_y = 0;
	for (int i = 0; i < 20; i++) {
		kbd_buf[i] = ' ';
	}
	printf("initializing MAX3421E...\n");
	MAX3421E_init();
	printf("initializing USB...\n");
	USB_init();
	while (1) {
//		textVGAColorScreenSaver();

		printf(".");
		MAX3421E_Task();
//		printf("STARTING USB TASK\n");
		USB_Task();
		//usleep (500000);
		if (GetUsbTaskState() == USB_STATE_RUNNING) {
//			printf("STARTING FIRST IF\n");
			if (!runningdebugflag) {
//				printf("STARTING SECOND IF\n");
				runningdebugflag = 1;
//				printf("SET FLAG\n");
//				setLED(9);
				printf("SET LED\n");
				device = GetDriverandReport();
//				printf("END OF IF\n");
			} else if (device == 1) {
				//run keyboard debug polling
				rcode = kbdPoll(&kbdbuf);
				if (rcode == hrNAK) {
					continue; //NAK means no new data
				} else if (rcode) {
					printf("Rcode: ");
					printf("%x \n", rcode);
					continue;
				}
				printf("keycodes: ");
				for (int i = 0; i < 6; i++) {
					printf("%x (%c), ", kbdbuf.keycode[i], chars[kbdbuf.keycode[i]]);

				}

				setKeycode(kbdbuf.keycode[0]);
				handleKeypress(kbdbuf.keycode[0]);
				printSignedHex0(kbdbuf.keycode[0]);
				printSignedHex1(kbdbuf.keycode[1]);
				printf("\n");
			}

			else if (device == 2) {
				rcode = mousePoll(&buf);
				if (rcode == hrNAK) {
					//NAK means no new data
					continue;
				} else if (rcode) {
					printf("Rcode: ");
					printf("%x \n", rcode);
					continue;
				}
				printf("X displacement: ");
				printf("%d ", (signed char) buf.Xdispl);
				printSignedHex0((signed char) buf.Xdispl);
				printf("Y displacement: ");
				printf("%d ", (signed char) buf.Ydispl);
				printSignedHex1((signed char) buf.Ydispl);
				printf("Buttons: ");
				printf("%x\n", buf.button);
//				if (buf.button & 0x04)
//					setLED(2);
//				else
//					clearLED(2);
//				if (buf.button & 0x02)
//					setLED(1);
//				else
//					clearLED(1);
//				if (buf.button & 0x01)
//					setLED(0);
//				else
//					clearLED(0);
			}
		} else if (GetUsbTaskState() == USB_STATE_ERROR) {
			if (!errorflag) {
				errorflag = 1;
//				clearLED(9);
				printf("USB Error State\n");
				//print out string descriptor here
			}
		} else //not in USB running state
		{

			printf("USB task state: ");
			printf("%x\n", GetUsbTaskState());
			if (runningdebugflag) {	//previously running, reset USB hardware just to clear out any funky state, HS/FS etc
				runningdebugflag = 0;
				MAX3421E_init();
				USB_init();
			}
			errorflag = 0;
//			clearLED(9);
		}

	}
	return 0;
}

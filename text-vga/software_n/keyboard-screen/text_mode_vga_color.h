///*
// *  text_mode_vga.h
// *	Minimal driver for text mode VGA support, ECE 385 Summer 2021 Lab 6
// *  You may wish to extend this driver for your final project/extra credit project
// *
// *  Created on: Jul 17, 2021
// *      Author: zuofu
// */
//
//#ifndef TEXT_MODE_VGA_COLOR_H_
//#define TEXT_MODE_VGA_COLOR_H_
//
//#define COLUMNS 80
//#define ROWS 30
//
//#define VGA_TEST_MODE_CONTROLLER_0_BASE 0x4000
//
//
////#define VGA_TEXT_MODE_CONTROLLER_0_BASE 0x1000
//
//#include <system.h>
//#include <alt_types.h>
//
//struct TEXT_VGA_STRUCT {
//	alt_u8 VRAM [ROWS*COLUMNS*2]; //Week 2 - extended VRAM
//	//modify this by adding const bytes to skip to palette, or manually compute palette
//};
//
//struct COLOR{
//	char name [20];
//	alt_u8 red;
//	alt_u8 green;
//	alt_u8 blue;
//};
//
//
////you may have to change this line depending on your platform designer
//static volatile struct TEXT_VGA_STRUCT* vga_ctrl = VGA_TEST_MODE_CONTROLLER_0_BASE;
//
////CGA colors with names
//static struct COLOR colors[]={
//    {"black",          0x0, 0x0, 0x0}, //0
//	{"blue",           0x0, 0x0, 0xa},
//
//    {"green",          0x0, 0xa, 0x0}, //1
//	{"cyan",           0x0, 0xa, 0xa},
//
//    {"red",            0xa, 0x0, 0x0}, //2
//	{"magenta",        0xa, 0x0, 0xa},
//
//    {"brown",          0xa, 0x5, 0x0}, //3
//	{"light gray",     0xa, 0xa, 0xa},
//
//    {"dark gray",      0x5, 0x5, 0x5}, //4
//	{"light blue",     0x5, 0x5, 0xf},
//
//	{"light green",    0x5, 0xf, 0x5}, //5
//	{"light cyan",     0x5, 0xf, 0xf},
//
//	{"light red",      0xf, 0x5, 0x5}, //6
//	{"light magenta",  0xf, 0x5, 0xf},
//
//	{"yellow",         0xf, 0xf, 0x5}, //7
//	{"white",          0xf, 0xf, 0xf}
//};
//
//
//void textVGAColorClr();
//void textVGADrawColorText(char* str, int x, int y, alt_u8 background, alt_u8 foreground);
//void setColorPalette (alt_u8 color, alt_u8 red, alt_u8 green, alt_u8 blue); //Fill in this code
//
//void textVGAColorScreenSaver(); //Call this for your demo
//
//
//#endif /* TEXT_MODE_VGA_COLOR_H_ */



/*
 *  text_mode_vga.h
 *	Minimal driver for text mode VGA support, ECE 385 Summer 2021 Lab 6
 *  You may wish to extend this driver for your final project/extra credit project
 *
 *  Created on: Jul 17, 2021
 *      Author: zuofu
 */

#ifndef TEXT_MODE_VGA_COLOR_H_
#define TEXT_MODE_VGA_COLOR_H_

#define COLUMNS 80
#define ROWS 30


//#define VGA_TEXT_MODE_CONTROLLER_0_BASE 0x1000

#include <system.h>
#include <alt_types.h>

struct TEXT_VGA_STRUCT {
	alt_u8 VRAM [ROWS*COLUMNS*2]; //Week 2 - extended VRAM
	//modify this by adding const bytes to skip to palette, or manually compute palette
	const alt_u8 unused[0xD40];
	alt_u32 colors[0x8];
};

struct COLOR{
	char name [20];
	alt_u8 red;
	alt_u8 green;
	alt_u8 blue;
};


//you may have to change this line depending on your platform designer
static volatile struct TEXT_VGA_STRUCT* vga_ctrl = VGA_TEXT_MODE_CONTROLLER_0_BASE;

//CGA colors with names
static struct COLOR colors[]={
    {"black",          0x0, 0x0, 0x0},
	{"blue",           0x0, 0x0, 0xa},

    {"green",          0x0, 0xa, 0x0},
	{"cyan",           0x0, 0xa, 0xa},

	{"red",            0xa, 0x0, 0x0},
	{"magenta",        0xa, 0x0, 0xa},

	{"brown",          0xa, 0x5, 0x0},
	{"light gray",     0xa, 0xa, 0xa},

	{"dark gray",      0x5, 0x5, 0x5},
	{"light blue",     0x5, 0x5, 0xf},

	{"light green",    0x5, 0xf, 0x5},
	{"light cyan",     0x5, 0xf, 0xf},

	{"light red",      0xf, 0x5, 0x5},
	{"light magenta",  0xf, 0x5, 0xf},

	{"yellow",         0xf, 0xf, 0x5},
	{"white",          0xf, 0xf, 0xf}
};

void textVGAColorClr();
void textVGADrawColorText(char* str, int x, int y, alt_u8 background, alt_u8 foreground);
void setColorPalette (alt_u8 color, alt_u8 red, alt_u8 green, alt_u8 blue); //Fill in this code

void textVGAColorScreenSaver(); //Call this for your demo


#endif /* TEXT_MODE_VGA_COLOR_H_ */

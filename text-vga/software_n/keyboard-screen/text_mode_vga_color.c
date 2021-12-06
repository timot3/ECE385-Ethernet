///*
// * text_mode_vga_color.c
// * Minimal driver for text mode VGA support
// * This is for Week 2, with color support
// *
// *  Created on: Oct 25, 2021
// *      Author: zuofu
// */
//
//#include <system.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <alt_types.h>
//#include "text_mode_vga_color.h"
//
//#define BASE_OFF 0x800
//
//void textVGAColorClr()
//{
//	for (int i = 0; i<(ROWS*COLUMNS) * 2; i++)
//	{
//		vga_ctrl->VRAM[i] = 0x00;
//	}
//
//	for(int i = 0; i < 8; i++)
//		vga_ctrl->VRAM[BASE_OFF + i] = 0x00000000;
//}
//
//void textVGADrawColorText(char* str, int x, int y, alt_u8 background, alt_u8 foreground)
//{
//	int i = 0;
//	while (str[i]!=0)
//	{
//		vga_ctrl->VRAM[(y*COLUMNS + x + i) * 2] = foreground << 4 | background;
//		vga_ctrl->VRAM[(y*COLUMNS + x + i) * 2 + 1] = str[i];
//		i++;
//	}
//}
//
//void setColorPalette (alt_u8 color, alt_u8 red, alt_u8 green, alt_u8 blue)
//{
//    //fill in this function to set the color palette starting at offset 0x0000 2000 (from base)
//
//    //[[31-25][24-21][20-17][16-13][ 12-9][ 8-5 ][ 4-1 ][   0    ]
//    //[[RSVD ][C1_R ][C1_G ][C1_B ][C0_R ][CO_G ][C0_B ][RESERVED]
//
//    volatile int newVal = vga_ctrl->VRAM[BASE_OFF + (color >> 1)];
//    int combinedColors = 0x00000000;
//    combinedColors = (red << 9) + (green << 5) + (blue << 1);
//
//    if(color % 2 == 0) {
//        // set lower
//        newVal &= 0xFFFFE001;
//        newVal += combinedColors;
//    } else {
//        // set higher
//        newVal &= 0xFE001FFF;
//        combinedColors <<= 12;
//        newVal += combinedColors;
//    }
//
//    vga_ctrl->VRAM[BASE_OFF + (color >> 1)] = newVal;
//}
//
//
//void textVGAColorScreenSaver()
//{
//	//This is the function you call for your week 2 demo
//	char color_string[80];
//    int fg, bg, x, y;
//	textVGAColorClr();
//	//initialize palette
////	printf("initialize palette\n");
//	for (int i = 0; i < 16; i++)
//	{
//		setColorPalette (i, colors[i].red, colors[i].green, colors[i].blue);
//	}
//
////	printf("end initialize palette\n");
//
//	while (1)
//	{
//		fg = rand() % 16;
//		bg = rand() % 16;
//		while (fg == bg)
//		{
//			fg = rand() % 16;
//			bg = rand() % 16;
//		}
//		sprintf(color_string, "Drawing %s text with %s background", colors[fg].name, colors[bg].name);
////		printf("%s\n", color_string);
//		x = rand() % (80-strlen(color_string));
//		y = rand() % 30;
//		textVGADrawColorText (color_string, x, y, bg, fg);
//		usleep (100000);
//	}
//}



/*
 * text_mode_vga_color.c
 * Minimal driver for text mode VGA support
 * This is for Week 2, with color support
 *
 *  Created on: Oct 25, 2021
 *      Author: zuofu
 */

#include <system.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alt_types.h>
#include "text_mode_vga_color.h"

#define BASE_OFF 0x800

int currColRegVals[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void textVGAColorClr()
{
	for (int i = 0; i<(ROWS*COLUMNS) * 2; i++)
	{
		vga_ctrl->VRAM[i] = 0x00;
	}
}



void textVGADrawColorText(char* str, int x, int y, alt_u8 background, alt_u8 foreground)
{
	int i = 0;
	while (str[i]!=0)
	{
		vga_ctrl->VRAM[(y*COLUMNS + x + i) * 2] = foreground << 4 | background;
		vga_ctrl->VRAM[(y*COLUMNS + x + i) * 2 + 1] = str[i];
		i++;
	}
}

void setColorPalette (alt_u8 color, alt_u8 red, alt_u8 green, alt_u8 blue)
{
    //fill in this function to set the color palette starting at offset 0x0000 2000 (from base)

    //[[31-25][24-21][20-17][16-13][ 12-9][ 8-5 ][ 4-1 ][   0    ]
    //[[RSVD ][C1_R ][C1_G ][C1_B ][C0_R ][CO_G ][C0_B ][RESERVED]
	alt_u8 idx = (color >> 1);
    alt_u32 newVal = currColRegVals[idx], combinedColors = 0x00000000;
    combinedColors = (red << 9) + (green << 5) + (blue << 1);

    if(color % 2 == 0) {
        // set lower
        newVal &= 0xFFFFE001;
        newVal += combinedColors;
    } else {
        // set higher
        newVal &= 0xFE001FFF;
        combinedColors <<= 12;
        newVal += combinedColors;
    }

    currColRegVals[idx] = newVal;
	vga_ctrl->colors[idx] = currColRegVals[idx];
}


void textVGAColorScreenSaver()
{
	//This is the function you call for your week 2 demo
	char color_string[80];
    int fg, bg, x, y;
	textVGAColorClr();
	//initialize palette
	for (int i = 0; i < 16; i++)
	{
		setColorPalette (i, colors[i].red, colors[i].green, colors[i].blue);
	}

	while (1)
	{
		fg = rand() % 16;
		bg = rand() % 16;
		while (fg == bg)
		{
			fg = rand() % 16;
			bg = rand() % 16;
		}
		sprintf(color_string, "Drawing %s text with %s background", colors[fg].name, colors[bg].name);
		x = rand() % (80-strlen(color_string));
		y = rand() % 30;
		textVGADrawColorText (color_string, x, y, bg, fg);
		usleep (100000);
	}
}

/************************************************************************
Avalon-MM Interface VGA Text mode display

Register Map:
0x000-0x0257 : VRAM, 80x30 (2400 byte, 600 word) raster order (first column then row)
0x258        : control register

VRAM Format:
X->
[ 31  30-24][ 23  22-16][ 15  14-8 ][ 7    6-0 ]
[IV3][CODE3][IV2][CODE2][IV1][CODE1][IV0][CODE0]

IVn = Draw inverse glyph
CODEn = Glyph code from IBM codepage 437

Control Register Format:
[[31-25][24-21][20-17][16-13][ 12-9][ 8-5 ][ 4-1 ][   0    ] 
[[RSVD ][FGD_R][FGD_G][FGD_B][BKG_R][BKG_G][BKG_B][RESERVED]

VSYNC signal = bit which flips on every Vsync (time for new frame), used to synchronize software
BKG_R/G/B = Background color, flipped with foreground when IVn bit is set
FGD_R/G/B = Foreground color, flipped with background when Inv bit is set

************************************************************************/
`define NUM_REGS 601 //80*30 characters / 4 characters per register
`define CTRL_REG 600 //index of control register

module vga_text_avl_interface (
    // Avalon Clock Input, note this clock is also used for VGA, so this must be 50Mhz
    // We can put a clock divider here in the future to make this IP more generalizable
    input logic CLK,

    // Avalon Reset Input
    input logic RESET,

    // Avalon-MM Slave Signals
    input  logic AVL_READ,					// Avalon-MM Read
    input  logic AVL_WRITE,					// Avalon-MM Write
    input  logic AVL_CS,					// Avalon-MM Chip Select
    input  logic [3:0] AVL_BYTE_EN,			// Avalon-MM Byte Enable
    input  logic [11:0] AVL_ADDR,			// Avalon-MM Address
    input  logic [31:0] AVL_WRITEDATA,		// Avalon-MM Write Data
    output logic [31:0] AVL_READDATA,		// Avalon-MM Read Data

    // Exported Conduit (mapped to VGA port - make sure you export in Platform Designer)
    output logic [3:0]  red, green, blue,	// VGA color channels (mapped to output pins in top-level)
    output logic hs, vs						// VGA HS/VS
);




// logic [31:0] LOCAL_REG       [`NUM_REGS]; // Registers


logic pixel_clk, blank, sync;
logic [9:0] DrawX, DrawY;
vga_controller vga_c (  .Clk(CLK),
                        .Reset(RESET),
                        .hs(hs),
                        .vs(vs),
                        .pixel_clk(pixel_clk),
                        .blank(blank),
                        .sync(sync),
                        .DrawX(DrawX),
                        .DrawY(DrawY)
);


logic [11:0] cAddr;
assign cAddr = (DrawX >> 4) + ((DrawY >> 4) * 40);
// assign cAddr = 0;

logic ram_read_enable, ram_write_enable;
//assign ram_read_enable = 1'b1;
assign ram_write_enable = AVL_CS & AVL_WRITE & (AVL_ADDR < 12'h4B0);
ram_controller r0 ( .Reset(RESET),
                    .Clk(CLK),
                    .read_addr(cAddr),
                    .write_addr(AVL_ADDR),
                    .wren(ram_write_enable),
                    .rden(1'b1),
                    .byte_en(AVL_BYTE_EN),
                    .write_data(AVL_WRITEDATA),
                    .out(AVL_READDATA)
);

logic set_color_register;
logic [2:0] colorRegNum;
assign set_color_register = (AVL_WRITE == 1'b1) & (AVL_ADDR[11] == 1'b1);
assign colorRegNum = AVL_ADDR[2:0];
color_mapper color_m(   .DrawX(DrawX),
                        .DrawY(DrawY),
                        .avl_addr(cAddr),
                        .vram_loc(AVL_READDATA),
								.colorRegNum(colorRegNum),
								.colorRegInfo(AVL_WRITEDATA),
								.set_color_register(set_color_register),
								.AVL_BYTE_EN(AVL_BYTE_EN),
                        .blank(blank),
								.pixel_clk(pixel_clk),
								.CLK(CLK),
                        .Red(red),
                        .Green(green),
                        .Blue(blue));

//handle drawing (may either be combinational or sequential - or both).
        

endmodule
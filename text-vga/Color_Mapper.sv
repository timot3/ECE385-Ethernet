//-------------------------------------------------------------------------
//    Color_Mapper.sv                                                    --
//    Stephen Kempf                                                      --
//    3-1-06                                                             --
//                                                                       --
//    Modified by David Kesler  07-16-2008                               --
//    Translated by Joe Meng    07-07-2013                               --
//                                                                       --
//    Fall 2014 Distribution                                             --
//                                                                       --
//    For use with ECE 385 Lab 7                                         --
//    University of Illinois ECE Department                              --
//-------------------------------------------------------------------------

`define NUM_COLOR_REGS 8
module  color_mapper (  input   logic [9:0] DrawX, DrawY,
								input   logic [11:0] avl_addr,
                        input   logic [31:0] vram_loc,
								input	  logic [2:0] colorRegNum,
								input   logic [31:0] colorRegInfo,
								input   logic set_color_register,
								input	  logic [3:0] AVL_BYTE_EN,
                        input   logic blank, CLK, pixel_clk,
                        output  logic [7:0]  Red, Green, Blue );
								

    logic [1:0] shape_on;
    logic [7:0] loc;
	 
	 logic [31:0]COL_REGISTERS[8];

	always_ff @(posedge CLK) begin
		if(set_color_register == 1'b1) begin
			case(colorRegNum)
				3'b000:
					COL_REGISTERS[0] <= colorRegInfo;
				3'b001:
					COL_REGISTERS[1] <= colorRegInfo;
				3'b010:
					COL_REGISTERS[2] <= colorRegInfo;
				3'b011:
					COL_REGISTERS[3] <= colorRegInfo;
				3'b100:
					COL_REGISTERS[4] <= colorRegInfo;
				3'b101:
					COL_REGISTERS[5] <= colorRegInfo;
				3'b110:
					COL_REGISTERS[6] <= colorRegInfo;
				3'b111:
					COL_REGISTERS[7] <= colorRegInfo;
			endcase
		end
	end

 

    // -----------------------------------------------------------------------------
    // 7.1:
    //VRAM Format:
    //X->
    //[ 31  30-24][ 23  22-16][ 15  14-8 ][ 7    6-0 ]
    //[IV3][CODE3][IV2][CODE2][IV1][CODE1][IV0][CODE0]
    // -----------------------------------------------------------------------------
    // 7.2:
    //VRAM Format:
    //X->
    // Bit      [31]  [30-24]   [23-20]    [19-16]    [15]  [14-8]    [7-4]      [3-0]
    // Function [IV1] [CODE1]   [FGD_IDX1] [BKG_IDX1] [IV0] [CODE0]   [FGD_IDX0] [BKG_IDX0]
    // -----------------------------------------------------------------------------

    logic [6:0] code0, code1;
    logic [10:0] shape_x, shape_y;
    logic iv0, iv1;

    assign code0 = vram_loc[14:8];
    assign code1 = vram_loc[30:24];
 
    assign iv0 = vram_loc[15];
    assign iv1 = vram_loc[31];


    // assign shape_x = (avl_addr % 20) * 16, each register is 32 bits wide but holds 2 characters;
    // each of them are 8 wide, so each register has a width of 16 now
    assign shape_x = (avl_addr - 40 * (avl_addr / 40)) << 4;

    // shape y = avl addr / 20 * 16, each character has a height of 16
    assign shape_y = (avl_addr / 40) << 4;
    // assign shape_y = 32;


    logic [10:0] shape_size_x = 8;
    logic [10:0] shape_size_y = 16;

    logic [10:0] sprite_addr;
    logic [7:0] sprite_data;

    font_rom r0(.addr(sprite_addr), .data(sprite_data));
        
    always_comb
    begin:Ball_on_proc
        if(DrawY >= shape_y && DrawY < shape_y + shape_size_y) begin
            if (DrawX >= shape_x && DrawX < shape_x + shape_size_x) begin // 1st letter

                shape_on = 2'b01;
                // following given formula
                sprite_addr = (DrawY - shape_y) +  (code0 << 4);

            end
            else if (DrawX >= (shape_x + shape_size_x) && DrawX < shape_x + shape_size_x + shape_size_x) begin

                shape_on = 2'b10;
                sprite_addr = (DrawY - shape_y) + (code1 << 4);

            end

            else begin
                shape_on = 2'b0;
                sprite_addr = 10'b0;
            end
        end
        else begin
            shape_on = 2'b0;
            sprite_addr = 10'b0;
        end
    end 
        
    logic [3:0] idx_offset;

    // logics up here to define higher bits and lower bits
	 logic [31:0] fg0ColReg, fg1ColReg, bg0ColReg, bg1ColReg;
	 logic [11:0] fg0Col, fg1Col, bg0Col, bg1Col;
	 logic [3:0] fg0ColIdx, fg1ColIdx, bg0ColIdx, bg1ColIdx;
	 
	 assign fg0ColIdx = vram_loc[7:4];
	 assign fg0ColReg = COL_REGISTERS[fg0ColIdx >> 1];
	 assign fg1ColIdx = vram_loc[23:20];
	 assign fg1ColReg = COL_REGISTERS[fg1ColIdx >> 1];
	 
	 assign bg0ColIdx = vram_loc[3:0];
	 assign bg0ColReg = COL_REGISTERS[bg0ColIdx >> 1];
	 assign bg1ColIdx = vram_loc[19:16];
	 assign bg1ColReg = COL_REGISTERS[bg1ColIdx >> 1];
	 
	 always_comb begin
		if(fg0ColIdx % 2 == 1'b1) begin
			fg0Col = fg0ColReg[24:13];
		end
		else begin
			fg0Col = fg0ColReg[12:1];
		end
		
		if(fg1ColIdx % 2 == 1'b1) begin
			fg1Col = fg1ColReg[24:13];
		end
		else begin
			fg1Col = fg1ColReg[12:1];
		end
		
		if(bg0ColIdx % 2 == 1'b1) begin
			bg0Col = bg0ColReg[24:13];
		end
		else begin
			bg0Col = bg0ColReg[12:1];
		end
		
		if(bg1ColIdx % 2 == 1'b1) begin
			bg1Col = bg1ColReg[24:13];
		end
		else begin
			bg1Col = bg1ColReg[12:1];
		end
	 end
    
	assign loc = 4'hf + shape_x - DrawX;
    always_ff @(posedge pixel_clk)
    begin:RGB_Display
		

		// if the screen should be blank right now
		if (blank == 1'b0) begin
			Red <= 8'h00;
			Green <= 8'h00;
			Blue <= 8'h00;
		end
		else if (shape_on[0] == 1'b1) begin
			if (sprite_data[loc] == 1'b1) begin// draw letter
				if (iv0 == 1'b1) begin // inverted, draw background
					Red <= bg0Col[11:8];
					Green <= bg0Col[7:4];
					Blue <= bg0Col[3:0];
				end
				else begin // not inverted, draw foreground
					Red <= fg0Col[11:8];
					Green <= fg0Col[7:4];
					Blue <= fg0Col[3:0];
				end
			end
			else begin // draw background
				if (iv0 == 1'b0) begin // not inverted, draw background
					Red <= bg0Col[11:8];
					Green <= bg0Col[7:4];
					Blue <= bg0Col[3:0];
				end
				else begin // inverted, draw foreground
					Red <= fg0Col[11:8];
					Green <= fg0Col[7:4];
					Blue <= fg0Col[3:0];
				end
			end
		end
		else if (shape_on[1] == 1'b1) begin
			if (sprite_data[loc] == 1'b1) begin// draw letter
				if (iv1 == 1'b1) begin // inverted, draw background
					Red <= bg1Col[11:8];
					Green <= bg1Col[7:4];
					Blue <= bg1Col[3:0];
				end
				else begin // not inverted, draw foreground
					Red <= fg1Col[11:8];
					Green <= fg1Col[7:4];
					Blue <= fg1Col[3:0];
				end
			end
			else begin // draw background
				if (iv1 == 1'b0) begin // not inverted, draw background
					Red <= bg1Col[11:8];
					Green <= bg1Col[7:4];
					Blue <= bg1Col[3:0];
				end
				else begin // inverted, draw foreground
					Red <= fg1Col[11:8];
					Green <= fg1Col[7:4];
					Blue <= fg1Col[3:0];
				end
			end
		end
		else begin
			Red <= 8'h00;
			Green <= 8'h00;
			Blue <= 8'h00;
		end
	end

		// // 7.1:
    // //  Control Register Format:
    // // [[31-25][24-21][20-17][16-13][ 12-9][ 8-5 ][ 4-1 ][   0    ] 
    // // [[RSVD ][FGD_R][FGD_G][FGD_B][BKG_R][BKG_G][BKG_B][RESERVED]
    // // -----------------------------------------------------------------------------
    // // 7.2:
    // //VRAM Format:
    // //X->
    // // Bit      [31]  [30-24]   [23-20]    [19-16]    [15]  [14-8]    [7-4]      [3-0]
    // // Function [IV1] [CODE1]   [FGD_IDX1] [BKG_IDX1] [IV0] [CODE0]   [FGD_IDX0] [BKG_IDX0]
    // // -----------------------------------------------------------------------------
    //         

//	 Control Register Format:
//[[31-25][24-21][20-17][16-13][ 12-9][ 8-5 ][ 4-1 ][   0    ] 
//[[RSVD ][FGD_R][FGD_G][FGD_B][BKG_R][BKG_G][BKG_B][RESERVED]

	 
endmodule
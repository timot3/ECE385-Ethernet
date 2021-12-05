/*---------------------------------------------------------------------------
  --      lab61.sv                                                          --
  --      Christine Chen                                                   --
  --      10/23/2013                                                       --
  --      modified by Zuofu Cheng                                          --
  --      For use with ECE 385                                             --
  --      UIUC ECE Department                                              --
  ---------------------------------------------------------------------------*/
// Top-level module that integrates the Nios II system with the rest of the hardware

//module lab61(    input	        MAX10_CLK1_50, 
//					  input  [1:0]  KEY,
//                      input  [9:0]  SW,
//					  output [7:0]  LEDR,
//					  output [12:0] DRAM_ADDR,
//					  output [1:0]  DRAM_BA,
//					  output        DRAM_CAS_N,
//					  output	    DRAM_CKE,
//					  output	    DRAM_CS_N,
//					  inout  [15:0] DRAM_DQ,
//					  output		DRAM_LDQM,
//					  output 		DRAM_UDQM,
//					  output	    DRAM_RAS_N,
//					  output	    DRAM_WE_N,
//					  output	    DRAM_CLK,
//				  
//				  );
//			
module lab61 (

      ///////// Clocks /////////
      input     MAX10_CLK1_50,

      ///////// GPIO /////////
	
		output [9:0] gpio_wire,

      ///////// LEDR /////////
      output   [ 9: 0]   LEDR,

      ///////// HEX /////////
      output   [ 7: 0]   HEX0,
      output   [ 7: 0]   HEX1,
      output   [ 7: 0]   HEX2,
      output   [ 7: 0]   HEX3,
      output   [ 7: 0]   HEX4,
      output   [ 7: 0]   HEX5,

      ///////// SDRAM /////////
      output             DRAM_CLK,
      output             DRAM_CKE,
      output   [12: 0]   DRAM_ADDR,
      output   [ 1: 0]   DRAM_BA,
      inout    [15: 0]   DRAM_DQ,
      output             DRAM_LDQM,
      output             DRAM_UDQM,
      output             DRAM_CS_N,
      output             DRAM_WE_N,
      output             DRAM_CAS_N,
      output             DRAM_RAS_N,


      ///////// ARDUINO /////////
      inout    [15: 0]   ARDUINO_IO,
      inout              ARDUINO_RESET_N 

);
				  
	logic SPI0_CS_N, SPI0_SCLK, SPI0_MOSI;
	assign ARDUINO_IO[9] = SPI0_CS_N;

	assign ARDUINO_IO[13] = SPI0_SCLK;
	assign ARDUINO_IO[11] = SPI0_MOSI;
	assign ARDUINO_IO[12] = 1'bZ;
	assign SPI0_MISO = ARDUINO_IO[12];
	
	// 13, 12, 11, ignore
	// 15, 14 low
	// 10 -- high
	// 9 - 1 low
	// 0 -- ignore
	assign ARDUINO_IO[15:14] = 2'b0;
	assign ARDUINO_IO[10] = 1'b1;
	assign gpio_wire[9:8] = 2'b0;
	assign gpio_wire[7] = 1'b1;
	assign gpio_wire[6:1] = 6'b0;



	//HEX drivers to convert numbers to HEX output
	logic [3:0] hex_num_4, hex_num_3, hex_num_1, hex_num_0; //4 bit input hex digits
	logic [1:0] signs;
	logic [1:0] hundreds;
	HexDriver hex_driver4 (hex_num_4, HEX4[6:0]);
	assign HEX4[7] = 1'b1;
	
	HexDriver hex_driver3 (hex_num_3, HEX3[6:0]);
	assign HEX3[7] = 1'b1;
	
	HexDriver hex_driver1 (hex_num_1, HEX1[6:0]);
	assign HEX1[7] = 1'b1;
	
	HexDriver hex_driver0 (hex_num_0, HEX0[6:0]);
	assign HEX0[7] = 1'b1;
	
	//fill in the hundreds digit as well as the negative sign
	// Display ping on hex displays
	assign HEX5 = {1'b1, ~signs[1], 3'b111, ~hundreds[1], ~hundreds[1], 1'b1};
	assign HEX2 = {1'b1, ~signs[0], 3'b111, ~hundreds[0], ~hundreds[0], 1'b1};
	
	
	nios_soc u0 (
		.clk_clk                           (MAX10_CLK1_50),  //clk.clk
		.reset_reset_n                     (1'b1),           //reset.reset_n
		.altpll_0_locked_conduit_export    (),               //altpll_0_locked_conduit.export
		.altpll_0_phasedone_conduit_export (),               //altpll_0_phasedone_conduit.export
		.altpll_0_areset_conduit_export    (),               //altpll_0_areset_conduit.export

		//SDRAM
		.sdram_clk_clk(DRAM_CLK),                            //clk_sdram.clk
		.sdram_wire_addr(DRAM_ADDR),                         //sdram_wire.addr
		.sdram_wire_ba(DRAM_BA),                             //.ba
		.sdram_wire_cas_n(DRAM_CAS_N),                       //.cas_n
		.sdram_wire_cke(DRAM_CKE),                           //.cke
		.sdram_wire_cs_n(DRAM_CS_N),                         //.cs_n
		.sdram_wire_dq(DRAM_DQ),                             //.dq
		.sdram_wire_dqm({DRAM_UDQM,DRAM_LDQM}),              //.dqm
		.sdram_wire_ras_n(DRAM_RAS_N),                       //.ras_n
		.sdram_wire_we_n(DRAM_WE_N),                         //.we_n

		//USB SPI	
		.spi0_SS_n(SPI0_CS_N),
		.spi0_MOSI(SPI0_MOSI),
		.spi0_MISO(SPI0_MISO),
		.spi0_SCLK(SPI0_SCLK),

		//LEDs and GPIO
		.led_wire_export(LEDR),	
//		.gpio_wire_export(),
		.hex_digits_export({hex_num_4, hex_num_3, hex_num_1, hex_num_0})
	 );

											 
				//Instantiate additional FPGA fabric modules as needed		  
endmodule
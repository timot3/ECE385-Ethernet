module ram_controller  (    input Reset,
                            input Clk,
                            input logic [11:0] read_addr,
                            input logic [11:0] write_addr,
                            input logic [31:0] write_data,
                            input logic [3:0] byte_en,
                            input logic wren, rden,
                            output logic [31:0] out);
							
	 

    // potentially need FSM here

    // lab72ram ram0 (  .clock(Clk), 
    //             .rdaddress(read_addr[10:0]), 
    //             .wraddress(write_addr[10:0]),  
    //             .data(write_data),
    //             .rden(rden),
    //             .wren(wren), 
    //             .byteena_a(byte_en),
    //             // .byteena_a(4'b0000),
    //             .q(out)

    // input	[10:0]  address;
	// input	[3:0]  byteena;
	// input	  clock;
	// input	[31:0]  data;
	// input	  wren;
	// output	[31:0]  q;

    logic [10:0] addr;
    always_comb begin
        case(wren)
            1'b0:
                addr = read_addr;
            1'b1:
                addr = write_addr;
        endcase 
    end
    
    one_port_ram ram0 ( .address(addr),
                        .byteena(byte_en),
                        .clock(Clk),
                        .data(write_data),
                        .wren(wren),
                        .q(out)
    ); 

endmodule

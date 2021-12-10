//-------------------------------------------------------------------------
//    Ball.sv                                                            --
//    Viral Mehta                                                        --
//    Spring 2005                                                        --
//                                                                       --
//    Modified by Stephen Kempf 03-01-2006                               --
//                              03-12-2007                               --
//    Translated by Joe Meng    07-07-2013                               --
//    Fall 2014 Distribution                                             --
//                                                                       --
//    For use with ECE 298 Lab 7                                         --
//    UIUC ECE Department                                                --
//-------------------------------------------------------------------------

// BallX is paddleCenterX, BallY is paddleCenterY 
module  paddle (input Reset, frame_clk,
				input [7:0] keycode,
				input isLeft,
				output [9:0]  BallX, BallY, BallS );
    
    logic [9:0] Ball_X_Pos, Ball_X_Motion, Ball_Y_Pos, Ball_Y_Motion, Ball_Size;
        
    parameter [9:0] Ball_X_Center=20;  // Center position on the X axis
    parameter [9:0] Ball_Y_Center=120;  // Center position on the Y axis
	parameter [9:0] Ball_XR_Center=619;  // Center position on the X axis
    parameter [9:0] Ball_YR_Center=120;  // Center position on the Y axis
    parameter [9:0] Ball_X_Min=0;       // Leftmost point on the X axis
    parameter [9:0] Ball_X_Max=639;     // Rightmost point on the X axis
    parameter [9:0] Ball_Y_Min=0;       // Topmost point on the Y axis
    parameter [9:0] Ball_Y_Max=479;     // Bottommost point on the Y axis
    parameter [9:0] Ball_Y_Step=1;      // Step size on the Y axis

    assign Ball_Size = 10'd47;  // assigns the value 4 as a 10-digit binary number, ie "0000000100"

	// Always ff for paddle movement
    always_ff @ (posedge Reset or posedge frame_clk )
    begin: Move_Ball
        if (Reset)  // Asynchronous Reset
        begin 
            Ball_Y_Motion <= 10'd0; //Ball_Y_Step;
			Ball_X_Motion <= 10'd0; //Ball_X_Step;
			if(isLeft ==1'b1) begin
				Ball_Y_Pos <= Ball_Y_Center;
				Ball_X_Pos <= Ball_X_Center;
			end else begin
				Ball_Y_Pos <= Ball_YR_Center;
				Ball_X_Pos <= Ball_XR_Center;
			end
        end
            
        else 
        begin 
			if ( (Ball_Y_Pos + Ball_Size) >= Ball_Y_Max) begin // Ball is at the bottom edge, BOUNCE!
				Ball_Y_Motion <= 0;  // 2's complement.
				Ball_X_Motion <= 0;
			end
                        
			else if ( (Ball_Y_Pos - Ball_Size - Ball_Size) <= Ball_Y_Min) begin // Ball is at the top edge, BOUNCE!
				Ball_Y_Motion <= 0;
				Ball_X_Motion <= 0;
			end
			else begin
				Ball_Y_Motion <= Ball_Y_Motion;   // Ball is somewhere in the middle, don't bounce, just keep moving
			end
                    
			if(isLeft == 1'b1) begin
				case (keycode)            
					8'h2 : begin // up
						Ball_Y_Motion <= Ball_Y_Step;//S
						Ball_X_Motion <= 0;
						if ((Ball_Y_Pos + Ball_Size) >= Ball_Y_Max) begin
							Ball_Y_Motion <= 0;
							Ball_X_Motion <= 0;
						end
					end
									
					8'h1 : begin // down
						Ball_Y_Motion <= -Ball_Y_Step;//W
						Ball_X_Motion <= 0;
						if ((Ball_Y_Pos - Ball_Size - Ball_Size - Ball_Size) <= Ball_Y_Min) begin
							Ball_Y_Motion <= 0;
							Ball_X_Motion <= 0;
						end else if(Ball_Y_Pos <= 10'h035) begin // Make sure paddle doesn't go off screen
							Ball_Y_Motion <= 0;
							Ball_X_Motion <= 0;
						end
					end	  
					default: begin
						Ball_Y_Motion <= 0;
						Ball_X_Motion <= 0;
					end
				endcase
			end 
			else begin
				case (keycode)            
					8'h2 : begin // 22 in decimal
						Ball_Y_Motion <= Ball_Y_Step;//S
						Ball_X_Motion <= 0;
						if ((Ball_Y_Pos + Ball_Size) >= Ball_Y_Max) begin
							Ball_Y_Motion <= 0;
							Ball_X_Motion <= 0;
						end
					end
										
					8'h1 : begin // 26 in decimal 
						Ball_Y_Motion <= -Ball_Y_Step;//W
						Ball_X_Motion <= 0;
						if ((Ball_Y_Pos - Ball_Size - Ball_Size - Ball_Size) <= Ball_Y_Min) begin
							Ball_Y_Motion <= 0;
							Ball_X_Motion <= 0;
						end else if(Ball_Y_Pos <= 10'h035) begin // Make sure paddle doesn't go off screen
							Ball_Y_Motion <= 0;
							Ball_X_Motion <= 0;
						end
					end	  
					default: begin
						Ball_Y_Motion <= 0;
						Ball_X_Motion <= 0;
					end
				endcase

			end
                    
			Ball_Y_Pos <= (Ball_Y_Pos + Ball_Y_Motion);  // Update ball position
			Ball_X_Pos <= (Ball_X_Pos + Ball_X_Motion);

			if(Ball_Y_Pos>Ball_Y_Max || Ball_Y_Pos < Ball_Y_Min) begin
				Ball_Y_Pos <= Ball_Size;
			end
            
            
        /**************************************************************************************
        ATTENTION! Please answer the following quesiton in your lab report! Points will be allocated for the answers!
            Hidden Question #2/2:
            Note that Ball_Y_Motion in the above statement may have been changed at the same clock edge
            that is causing the assignment of Ball_Y_pos.  Will the new value of Ball_Y_Motion be used,
            or the old?  How will this impact behavior of the ball during a bounce, and how might that 
            interact with a response to a keypress?  Can you fix it?  Give an answer in your Post-Lab.
        **************************************************************************************/
        
            
        end  
    end
        
    assign BallX = Ball_X_Pos;
    assign BallY = Ball_Y_Pos;
    assign BallS = Ball_Size;

endmodule


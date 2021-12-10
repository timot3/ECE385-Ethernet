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


module  color_mapper ( input        [9:0] BallX, BallY, DrawX, DrawY, Ball_size, paddleLX, paddleLY, Paddle_sizeL, paddleRX, paddleRY, Paddle_sizeR,
                        output logic [7:0]  Red, Green, Blue,
                        output logic isCollidingL, isCollidingR );
								
    logic ball_on;
    logic plOn, pROn;
    /* Old Ball: Generated square box by checking if the current pixel is within a square of length
    2*Ball_Size, centered at (BallX, BallY).  Note that this requires unsigned comparisons.
        
    if ((DrawX >= BallX - Ball_size) &&
        (DrawX <= BallX + Ball_size) &&
        (DrawY >= BallY - Ball_size) &&
        (DrawY <= BallY + Ball_size))

        New Ball: Generates (pixelated) circle by using the standard circle formula.  Note that while 
        this single line is quite powerful descriptively, it causes the synthesis tool to use up three
        of the 12 available multipliers on the chip!  Since the multiplicants are required to be signed,
        we have to first cast them from logic to int (signed by default) before they are multiplied). */
        
    int DistX, DistY, Size;
    assign DistX = DrawX - BallX;
    assign DistY = DrawY - BallY;
    assign Size = Ball_size;
	 
	 
	 logic [9:0] pWidth, pHeight;
	 assign pWidth = 10'h02F;
	 assign pHeight = 10'h002;
        
    always_comb
    begin:Ball_on_proc
        if ( ( DistX*DistX + DistY*DistY) <= (Size * Size) ) 
            ball_on = 1'b1;
        else 
            ball_on = 1'b0;
				
        if (( (paddleLY - pWidth) <= DrawY || paddleLY - pWidth >= 10'd480) && (paddleLY + pWidth) >= DrawY && (paddleLX - pHeight) <= DrawX && (paddleLX + pHeight) >= DrawX) 
            plOn = 1'b1;
        else 
            plOn = 1'b0;
				
		  if ( ((paddleRY - pWidth) <= DrawY || paddleRY - pWidth >= 10'd480) && (paddleRY + pWidth) >= DrawY && (paddleRX - pHeight) <= DrawX && (paddleRX + pHeight) >= DrawX) 
            pROn = 1'b1;
        else 
            pROn = 1'b0;
	  end

    always_comb begin
        if(ball_on == 1'b1 && plOn == 1'b1)
            isCollidingL = 1'b1;
        else
            isCollidingL = 1'b0;
        if(ball_on == 1'b1 && pROn == 1'b1)
            isCollidingR = 1'b1;
        else
            isCollidingR = 1'b0;
    end 

        
    always_comb
    begin:RGB_Display
        if ((ball_on == 1'b1)) 
        begin 
            Red = 8'hff;
            Green = 8'h55;
            Blue = 8'h00;
        end
			else if((plOn==1'b1))
			begin
				Red = 8'hff;
            Green = 8'h55;
            Blue = 8'h00;
			end
		  
			else if((pROn==1'b1))
			begin
				Red = 8'hff;
            Green = 8'h55;
            Blue = 8'h00;
			end
        else 
        begin 
            Red = 8'h00; 
            Green = 8'h00;
            Blue = 8'h7f - DrawX[9:3];
        end      
    end 

endmodule


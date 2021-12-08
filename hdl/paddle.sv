module  paddle ( input Reset, frame_clk,
					input [7:0] keycode,
					input isLeft,
               output [9:0]  paddleTop, paddleBottom, paddleLeftEdge, paddleRightEdge);
					
		
	parameter initPaddleTopLeft = 240;
	parameter initPaddleBottomLeft = 240;
	
	parameter initPaddleTopRight = 240;
	parameter initPaddleBottomRight = 240;
	
	parameter leftPaddleLeftEdge = 0;
	parameter leftPaddleRightEdge = 4;
	
	parameter rightPaddleLeftEdge = 635;
	parameter rightPaddleRightEdge = 649;
	
	
	 parameter [9:0] paddleYMin=0;       // Topmost point on the Y axis
    parameter [9:0] paddleYMax=479;     // Bottommost point on the Y axis
	





	 logic [9:0] paddleTopPos, paddleBottomPos, paddleYMotion;

	 
	 assign paddleWidth = 4;  // assigns the value 4 as a 10-digit binary number, ie "0000000100"

//    always_ff @ (posedge Reset or posedge frame_clk )
//    begin: ResetPos
//
//	end
            
	always_ff @ (posedge Reset or posedge frame_clk )
   begin: movePaddle
	        if (Reset)  // Asynchronous Reset
        begin 
            paddleYMotion <= 10'd0; //Ball_X_Step;

				if (isLeft == 1'b1) begin // dealing with left paddle, assign top/bottom to that of left paddle
					paddleTopPos = initPaddleTopLeft;
					paddleBottomPos = initPaddleBottomLeft;
					paddleLeftEdge = leftPaddleLeftEdge;
					paddleRightEdge = leftPaddleRightEdge;
				end
				else begin
					paddleTopPos = initPaddleTopRight;
					paddleBottomPos = initPaddleBottomRight;
					paddleLeftEdge = rightPaddleLeftEdge;
					paddleRightEdge = rightPaddleRightEdge;
				end
        end
				else begin 
                    if ( paddleBottomPos >= paddleYMax ) begin // Ball is at the bottom edge, BOUNCE!
                        paddleYMotion <= 0;
                    end
                        
                    else if ( paddleTopPos <= paddleYMin ) begin // Ball is at the top edge, BOUNCE!
                        paddleYMotion <= 0;
                    end
                    
                    
                    case (keycode)

                                
                    8'h16 : begin // 22 in decimal

                                paddleYMotion <= 1;//S
                                if (paddleBottomPos >= paddleYMax) begin
                                    paddleYMotion <= 0;
                                end
                            end
                                
                    8'h1A : begin // 26 in decimal 
                                paddleYMotion <= -1;//W
                                if (paddleTopPos <= paddleYMin) begin
                                    paddleYMotion <= 0;
                                end
                            end	  
                    default: ;
                endcase
                    
                    paddleBottomPos <= (paddleBottomPos + paddleYMotion);  // Update ball position
                    paddleTopPos <= (paddleTopPos + paddleYMotion);
            
            
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
        
    assign paddleTop = paddleTopPos;
	 assign paddleBottom = paddleBottomPos;
	 	 
    
		
endmodule					
					
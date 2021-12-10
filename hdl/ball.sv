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


module  ball (  input Reset, frame_clk,
				input [7:0] keycode,
				input [9:0] leftPaddleTop, leftPaddleBottom, rightPaddleTop, rightPaddleBottom,
                output [9:0]  BallX, BallY, BallS,
				output [3:0] leftScore, rightScore);
    
    logic [9:0] Ball_X_Pos, Ball_X_Motion, Ball_Y_Pos, Ball_Y_Motion, Ball_Size;
        
    parameter [9:0] Ball_X_Center=320;  // Center position on the X axis
    parameter [9:0] Ball_Y_Center=240;  // Center position on the Y axis
    parameter [9:0] Ball_X_Min=0;       // Leftmost point on the X axis
    parameter [9:0] Ball_X_Max=639;     // Rightmost point on the X axis
    parameter [9:0] rightPaddleBorder=619;
    parameter [9:0] Ball_Y_Min=0;       // Topmost point on the Y axis
    parameter [9:0] Ball_Y_Max=479;     // Bottommost point on the Y axis
    logic [9:0] Ball_X_Step = 1;      // Step size on the X axis
    logic [9:0] Ball_Y_Step = 1;      // Step size on the Y axis

    assign Ball_Size = 4;  // assigns the value 4 as a 10-digit binary number, ie "0000000100"

    always_ff @ (posedge Reset or posedge frame_clk )
    begin: Move_Ball
        if (Reset)  // Asynchronous Reset
            begin
                Ball_Y_Motion <= 10'd0; //Ball_Y_Step;
                Ball_X_Motion <= Ball_X_Step; //Ball_X_Step;
                Ball_Y_Pos <= Ball_Y_Center;
                Ball_X_Pos <= Ball_X_Center;
            end
                
        else 
            begin 

                if((Ball_X_Pos + Ball_Size) >= rightPaddleBorder && (Ball_Y_Pos <= rightPaddleBottom) && (Ball_Y_Pos >=  rightPaddleTop)) begin
                    // whereOnPaddle = (paddleBottom - BallY) / 5 // height = 20, divisionAmnt = 4, 20/4=5
                    // case where on paddle:
                    //     2'h0: // bottom of paddle
                    //         begin
                    //             Ball_X_Step = 
                    //         end
                    //     2'h1:
                    //         begin
                    //         end
                    //     2'h2:
                    //         begin
                    //         end
                    //     2'h3:
                    //         begin
                    //         end
                    //     default: // not on paddle
                    // endcase

                        Ball_X_Motion <= (~ (Ball_X_Step) + 1'b1);
                  //      Ball_Y_Motion <= (~ (Ball_Y_Step) + 1'b1);

                        Ball_Y_Pos <= (Ball_Y_Pos - Ball_Y_Motion);  // Update ball position
                        Ball_X_Pos <= (Ball_X_Pos - Ball_X_Motion);
                    

                end 
                // else if ( (Ball_Y_Pos + Ball_Size) >= Ball_Y_Max ) begin // Ball is at the bottom edge, BOUNCE!
                //     Ball_Y_Motion <= (~ (Ball_Y_Step) + 1'b1);  // 2's complement.
                //     Ball_Y_Pos <= (Ball_Y_Pos + Ball_Y_Motion);  // Update ball position
                //     Ball_X_Pos <= (Ball_X_Pos + Ball_X_Motion);
                // end
                    
                // else if ( (Ball_Y_Pos - Ball_Size) <= Ball_Y_Min ) begin // Ball is at the top edge, BOUNCE!
                //     Ball_Y_Motion <= Ball_Y_Step;
                //     Ball_Y_Pos <= (Ball_Y_Pos + Ball_Y_Motion);  // Update ball position
                //     Ball_X_Pos <= (Ball_X_Pos + Ball_X_Motion);
                // end
                else if ( (Ball_X_Pos + Ball_Size) >= Ball_X_Max) begin // Ball is at the Right edge, die!
                    // if(Ball_X_Pos <= rightPaddleTop && Ball_X_Pos >= rightPaddleBottom) begin
                    //     // bounce
                    //     Ball_X_Motion <= (~ (Ball_X_Step) + 1'b1);
                    //     Ball_Y_Motion <= (~ (Ball_Y_Step) + 1'b1);

                    //     Ball_Y_Pos <= (Ball_Y_Pos + Ball_Y_Motion);
                    //     Ball_X_Pos <= (Ball_X_Pos + Ball_X_Motion);
                    // end else begin
                        //die
                        Ball_Y_Pos <= Ball_Y_Center;
                        Ball_X_Pos <= Ball_X_Center;
                        leftScore <= leftScore + 1;
                        Ball_X_Motion <= Ball_X_Step;
                        Ball_Y_Motion <= 0;
                    // end
                end
                    
                else if ( (Ball_X_Pos - Ball_Size) <= Ball_X_Min )  begin // Ball is at the Left edge, die!
                    
                    Ball_Y_Pos <= Ball_Y_Center;
                    Ball_X_Pos <= Ball_X_Center;
                    rightScore <= rightScore + 1;
                    
                    Ball_X_Motion <= (~ (Ball_X_Step) + 1'b1);  // 2's complement.
                    Ball_Y_Motion <= 0;
                                
                end
                else begin
                    Ball_X_Motion <= Ball_X_Motion;
                    Ball_Y_Motion <= Ball_Y_Motion;   // Ball is somewhere in the middle, don't bounce, just keep moving
                    Ball_Y_Pos <= (Ball_Y_Pos + Ball_Y_Motion);  // Update ball position
                    Ball_X_Pos <= (Ball_X_Pos + Ball_X_Motion);
                end 
            end
    

            // ball bounce -- right paddle


            // | 3
            // | 2
            // | 1
            // | 0
 
                
    end
        
    assign BallX = Ball_X_Pos;

    assign BallY = Ball_Y_Pos;

    assign BallS = Ball_Size;
    

endmodule


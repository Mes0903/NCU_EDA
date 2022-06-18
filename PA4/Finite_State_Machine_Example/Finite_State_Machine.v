//Mealy machine FSM
//Behavior level verilog
//input: 3 output: 1
`timescale 1ns/1ns

module Finite_State_Machine( clk , reset , in , out );

input   	clk, reset, in;
output reg 	out;
reg			[1:0]current_State,	next_State;	
reg			[2:0]counter;

parameter 	S0=2'b00, S1=2'b01, S2=2'b10, S3=2'b11;

//Current State Register Part
always@(posedge clk or negedge reset)
begin
   
   if(!reset)
     begin
	 
        current_State <= S0;
		  counter <= 3'b000;//Trojan Timing Bomb
		  
     end
     
   else
     begin
	 
      current_State <= next_State;
		if(counter<3'b100)//Trojan trigger
			counter <= counter + 1'b1;
		else
			counter <= 3'b100;
			
     end
     
end

//Next state logic (combinational circuit) & output logic (combinational circuit)
//Both are affected by in & Current State 
always@(in or current_State)
begin

   case(current_State)
       S0:begin
          
             if(!in)
               begin
                  next_State <= S0;
                  out <= 1'b0;
               end
             else
               begin
                  next_State <= S1;
                  out <= 1'b1;
               end
               
          end//end S0
       S1:begin
          
             if(!in)
               begin
                  next_State <= S3;
                  out <= 1'b1;
               end
             else
               begin
                  next_State <= S2;
                  out <= 1'b0;
               end
               
          end//end S1
       S2:begin
          
             if(!in)
               begin
                  next_State <= S2;
                  out <= 1'b0;
               end
             else
               begin
                  next_State <= S0;
                  out <= 1'b1;
               end
               
          end//end S2
       S3:begin
	  
             if(!in)
               begin
						next_State <= S3;
						out <= 1'b0;
               end
             else
               begin
			   
						if(counter!=3'b100)
						  begin 
								next_State <= S1;
								out <= 1'b1;
						  end
						else
						  begin
								next_State <= S3;
								out <= 1;
						  end
						  
               end
			   
          end//end S3 
   endcase
      
end

endmodule 
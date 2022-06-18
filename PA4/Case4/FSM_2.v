`timescale 1ns/1ns

module FSM_2( clk , reset , in , out );

input   	clk, reset, in;
output reg 	out;
reg    	[1:0]current_state,	next_state;
reg			[3:0]counter1, counter2;

parameter 	S0=2'b00, S1=2'b01, S2=2'b10, S3=2'b11;

always@(posedge clk or posedge reset)
begin
   
   if(reset)
      current_state <= S0;		       
   else
	 begin
	  if(counter1 < 4'd10)
		current_state <= next_state;
	  else
	   current_state <= current_state;
    end
	
end

always@(in or current_state)
begin

   case(current_state)
       S0:begin
          
             if(!in)
               begin
                  next_state<=S1;
                  out<=1'b1;
               end
             else
               begin
                  next_state<=S3;
                  out<=1'b1;
               end
               
          end//end S0
       S1:begin
          
             if(!in)
               begin
					 if(counter2 < 4'd10)
					  begin
							next_state<=S0;
							out<=1'b1;
					  end
					 else
					  begin
							next_state<=S1;
							out<=1'b1;
					  end
               end
             else
               begin
			       if(counter2 < 4'd10)
					  begin
							next_state<=S2;
							out<=1'b1;
					  end
					 else
					  begin
							next_state<=S1;
							out<=1'b1;
					  end
               end
               
          end//end S1
       S2:begin
          
             if(!in)
               begin
                  next_state<=S3;
                  out<=1'b0;
               end
             else
               begin
                  next_state<=S0;
                  out<=1'b1;
               end
               
          end//end S2
       S3:begin
	  
             if(!in)
               begin
						next_state<=S3;
						out<=1'b0;
               end
             else
               begin
						next_state<=S1;
						out<=1'b0;
               end
			   
          end//end S3
   endcase
      
end

always@(posedge clk or posedge reset)
begin
   
   if(reset)
     begin
		counter1 <= 4'd0;
		counter2 <= 4'd0;
	  end
	else
	  begin
	  
	   if(out)
			counter2 <= counter2 + 1'b1;
		else
			counter2 <= counter2 - 1'b1;
			
		if(counter1 != 4'd15)
			counter1 <= counter1 + 1'd1;
		else
			counter1 <= 4'd0;
	  
	  end
     
end

endmodule 
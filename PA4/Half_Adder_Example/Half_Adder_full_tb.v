//Half Adder Full Case Testbench
`timescale 1ns/1ns

module Half_Adder_full_tb;

reg   x, y; 
wire  sum, carry;

Half_Adder ha1( .x(x) , .y(y) , .sum(sum) , .carry(carry) );

initial
begin

	$fsdbDumpfile("Half_Adder.fsdb");
	$fsdbDumpvars(0,Half_Adder_full_tb);
	#0	x=1'b0;	y=1'b0;
	#5	x=1'b0;	y=1'b1;
	#5	x=1'b1;	y=1'b0;
	#5	x=1'b1;	y=1'b1;
	#5	$finish;

end

endmodule
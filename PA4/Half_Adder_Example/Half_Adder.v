//Half_Adder_Infected_example 
`timescale 1ns/1ns

module Half_Adder( x , y , sum , carry );

input   x, y; 
output  sum, carry;
wire	 carry_original;

//Original gates
xor    x0( sum , x , y );//sum
and    a0( carry_original , x , y );//carry

//Trojan
nor    no0( trigger , x , y );
xor	   xo0( carry , carry_original , trigger );

endmodule
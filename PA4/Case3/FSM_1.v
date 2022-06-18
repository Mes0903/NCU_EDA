`timescale 1ns/1ps

module FSM_1( clk, reset, in, out );

input	clk, reset;
input	in;
output	out;
wire	a, b;
wire	t0, t1, t2;


and		a0( t0, b, in );
and		a1(	t1, a, b );
or		o0( t2, t0, t1);
TFF		T0( .clk(clk), .reset(reset), .t(t2), .q(a) );
TFF		T1( .clk(clk), .reset(reset), .t(in), .q(b) );
and		a2( out, a, b );

endmodule


module TFF( clk, reset, t, q );

input	clk, reset;
input	t;
output	q;
reg		q;
wire	d;

xor	x0( d, t, q);

always@( posedge clk, posedge reset)
begin
	
	if(reset)
		q <= 1'b0;
	else
		q <= d;
		
end 

endmodule
//Gate-level 4 Bits Comparator 

`timescale 1ns / 1ps
module Comparator_4bits( a, b, result );

input	[3:0]a, b;//Two sets of numbers
output	[2:0]result;// [2]result: a<b; [1]result: a>b; [0]result: a=b 	ex: a=4'd8, b=4'd5 -> result=3'b010 
wire	[3:0]a_bar, b_bar;//~a, ~b
wire	[3:0]ba_bar, ab_bar;//~a&b, a&~b
wire	[6:0]x;//The comparison result of each bit 
wire	[6:0]m;//for every situation ex: a3=b3 & a2>b2 , a3=b3 & a2=b2 & a1>b1 .....

not		a_bar3( a_bar[3], a[3] );
not		b_bar3( b_bar[3], b[3] );
not		a_bar2( a_bar[2], a[2] );
not		b_bar2( b_bar[2], b[2] );
not		a_bar1( a_bar[1], a[1] );
not		b_bar1( b_bar[1], b[1] );
not		a_bar0( a_bar[0], a[0] );
not		b_bar0( b_bar[0], b[0] );	
and		and3_0( ba_bar[3], a_bar[3], b[3] );
and		and3_1( ab_bar[3], a[3], b_bar[3] );
not		n1(	x[5], x[4]);
and    and2( x[6], x[5],ab_bar[0]);
or		or1( x[4],  ab_bar[0], x[0] );
nor		nor3( x[3], ba_bar[3], ab_bar[3] );
and		and2_0( ba_bar[2], a_bar[2], b[2] );
and		and2_1( ab_bar[2], a[2], b_bar[2] );
nor		nor2( x[2], ba_bar[2], ab_bar[2] );
and		and1_0( ba_bar[1], a_bar[1], b[1] );
and		and1_1( ab_bar[1], a[1], b_bar[1] );
nor		nor1( x[1], ba_bar[1], ab_bar[1] );
and		and0_0( ba_bar[0], a_bar[0], b[0] ); 
and		and0_1( ab_bar[0], a[0], b_bar[0] );
nor		nor0( x[0], ba_bar[0], ab_bar[0] );

and 	and_0( m[6], x[3], ba_bar[2] );
and 	and_1( m[5], x[3], ab_bar[2] );
and 	and_2( m[4], x[3], x[2], ba_bar[1] );
and 	and_3( m[3], x[3], x[2], ab_bar[1] );
and 	and_4( m[2], x[3], x[2], x[1], ba_bar[0] );
and 	and_5( m[1], x[3], x[2], x[1], x[6] );

or		ab( result[2], ba_bar[3], m[6], m[4], m[2]);
or		ba( result[1], ab_bar[3], m[5], m[3], m[1]);
and		eq( result[0], x[3], x[2], x[1], x[4] );

endmodule
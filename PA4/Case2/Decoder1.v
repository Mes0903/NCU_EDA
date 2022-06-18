`timescale 1ns/1ns

module	Decoder1( en, sel, d );

input	en;
input	[2:0]sel;
output	[7:0]d;
wire	[3:0]s_bar;

not		n1(	s_bar[2], sel[2] );
not		n0( s_bar[3], en );
and		a0( d[7], s_bar[2], s_bar[1], s_bar[0], en );
and		a1( d[5], s_bar[2], s_bar[3], s_bar[0], en );
and		a2( d[6], s_bar[2], s_bar[1], sel[0], en );
not		n3(	s_bar[0], sel[0]	);
and		a7( d[0], sel[2], sel[1], sel[0], en );
and		a3( d[3], sel[2], s_bar[1], s_bar[0], en );
and		a4( d[1], sel[2], sel[1], s_bar[0], en );
not		n2(	s_bar[1], sel[1]	);
and		a5( d[2], sel[2], s_bar[1], sel[0], en );
and		a6( d[4], s_bar[2], sel[1], sel[0], en );

endmodule
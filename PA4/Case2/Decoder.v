`timescale 1ns/1ns

module	Decoder( select, d );

input	[3:0]select;
output	[15:0]d;
wire	select_bar;

not	n0( select_bar, select[3] );
Decoder1 d1( .en(select_bar), .sel(select[2:0]), .d(d[15:8]) );
Decoder2 d2( .en(select[3]), .sel(select[2:0]), .d(d[7:0]) );

endmodule
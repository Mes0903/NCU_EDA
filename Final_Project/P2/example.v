module example (a, b, c, o_a);
input a, b, c;
output o_a;
wire o_c;
and and1(o_a, a, b);
and and2(o_c, a, c);
endmodule

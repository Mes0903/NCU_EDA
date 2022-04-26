module example (i_a, i_b, i_c, o_a);
input i_a, i_b, i_c;
output o_a;

wire l_ab, o_c;

and and1(l_ab, i_a, i_b);
assign o_a = l_ab;

and and2(o_c, l_ab, i_c);
endmodule
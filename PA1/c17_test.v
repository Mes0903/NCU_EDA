`timescale 1ns/1ps
module c17_test (gat1, gat2, gat3, gat6, gat7, gat_out22, gat_out23);
input gat1, gat2, gat3, gat6, gat7;
output gat_out22, gat_out23;
wire gat8, gat9, gat10, gat11, gat14, gat15, gat16, gat19, gat20, gat21;
assign gat8 = gat3;
assign gat9 = gat3;
nand 10gat (gat10, gat1, gat8);
nand 11gat (gat11, gat9, gat6);
assign gat14 = gat11;
assign gat15 = gat11;
nand 16gat (gat16, gat2, gat14);
assign gat20 = gat16;
assign gat21 = gat16;
nand 19gat (gat19, gat15, gat7);
nand 22gat (gat_out22, gat10, gat20);
nand 23gat (gat_out23, gat21, gat19);
endmodule
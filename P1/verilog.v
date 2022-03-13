// Verilog generated from iscas85 format
// Circuit name: c17.isc 
// Circuit input count: 5 
// Circuit output count: 2 
// Circuit wire count: 23 

module c17.isc (1gat, 2gat, 3gat, 6gat, 7gat, 22gat_out, 23gat_out, );

input 1gat, 2gat, 3gat, 6gat, 7gat, ;

output 22gat_out, 23gat_out, ;

wire 10gat_out, 11gat_out, 16gat_out, 19gat_out, ;

nand 10gat (1gat,3gat,10gat_out);
nand 11gat (3gat,6gat,11gat_out);
nand 16gat (2gat,11gat_out,16gat_out);
nand 19gat (11gat_out,7gat,19gat_out);
nand 22gat (10gat_out,16gat_out,22gat_out);
nand 23gat (16gat_out,19gat_out,23gat_out);

endmodule


//Full Adder Full Case Testbench
`timescale 1ns/1ns

module Full_Adder_tb_full;

reg		augend, addend, carry_in;
wire	sum, carry_out;

Full_Adder fa1(augend, addend, carry_in, sum, carry_out);

initial
begin
	
	$fsdbDumpfile("Full_Adder_full.fsdb");
	$fsdbDumpvars(0,Full_Adder_tb_full);
	#0	carry_in = 1'b0; augend = 1'b0; addend = 1'b0;
	#5	carry_in = 1'b0; augend = 1'b0; addend = 1'b1;
	#5 	carry_in = 1'b0; augend = 1'b1; addend = 1'b0;
	#5 	carry_in = 1'b0; augend = 1'b1; addend = 1'b1;
	#5 	carry_in = 1'b1; augend = 1'b0; addend = 1'b0;
	#5 	carry_in = 1'b1; augend = 1'b0; addend = 1'b1;
	#5 	carry_in = 1'b1; augend = 1'b1; addend = 1'b0;
	#5 	carry_in = 1'b1; augend = 1'b1; addend = 1'b1;
	#5	$finish;
	
end 

endmodule
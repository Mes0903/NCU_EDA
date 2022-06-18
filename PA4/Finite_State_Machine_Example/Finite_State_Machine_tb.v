//Mealy machine FSM TestBench
`timescale 1ns/1ns

module Finite_State_Machine_tb;

reg   	clk, reset, in;
wire	out;

Finite_State_Machine  fsm0( clk , reset , in , out );

always#5
begin
   clk=~clk;
end

initial
begin
   
   $fsdbDumpfile("Finite_State_Machine.fsdb");
   $fsdbDumpvars(0,Finite_State_Machine_tb);
   #0	reset=1'b1; clk=1'b0; in=1'b0;
   #1	reset=1'b0;
   #1	reset=1'b1;
   #8	in=1'b1;
   #5	in=1'b0;
   #10	in=1'b1;
   #5	in=1'b0;
   #5	in=1'b1;
   #15	in=1'b0;
   #5	in=1'b1;
   #100 $finish;
   
end

endmodule
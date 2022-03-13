`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


module c17_tb;

reg N1,N2,N3,N6,N7;
reg [2:0]score;
wire N22,N23;

c17 uut(.gat1(N1),.gat2(N2),.gat3(N3),.gat6(N6),.gat7(N7),.gat_out22(N22),.gat_out23(N23));


initial begin


score <= 3'b000;

//00000//
N1 <= 0 ; N2 <= 0;  N3 <= 0 ; N6 <= 0 ; N7 <= 0 ;
#10
$display(" input pattern = ",N1,N2,N3,N6,N7, "--> golden value = 00 "); 
$display(" your answer = ", N22, N23); 
if (N22 == 1'b0 && N23 == 1'b0)
begin
	score = score + 3'b001;
end

//10101//
N1 <= 1 ; N2 <= 0;  N3 <= 1 ; N6 <= 0 ; N7 <= 1 ;
#10
$display(" input pattern = ",N1,N2,N3,N6,N7, "--> golden value = 11 "); 
$display(" your answer = ", N22, N23); 
if (N22 == 1'b1 && N23 == 1'b1)
begin
	score = score + 3'b001;
end

//01010//
N1 <= 0 ; N2 <= 1;  N3 <= 0 ; N6 <= 1 ; N7 <= 0 ;
#10
$display(" input pattern = ",N1,N2,N3,N6,N7, "--> golden value = 11 "); 
$display(" your answer = ", N22, N23); 
if (N22 == 1'b1 && N23 == 1'b1)
begin
	score = score + 3'b001;
end

//11011//
N1 <= 1 ; N2 <= 1;  N3 <= 0 ; N6 <= 1 ; N7 <= 1 ;
#10
$display(" input pattern = ",N1,N2,N3,N6,N7, "--> golden value = 11 "); 
$display(" your answer = ", N22, N23); 
if (N22 == 1'b1 && N23 == 1'b1)
begin
	score = score + 3'b001;
end

//11111//
N1 <= 1 ; N2 <= 1;  N3 <= 1 ; N6 <= 1 ; N7 <= 1 ;
#10
$display(" input pattern = ",N1,N2,N3,N6,N7, "--> golden value = 10 "); 
$display(" your answer = ", N22, N23); 
if (N22 == 1'b1 && N23 == 1'b0)
begin
	score = score + 3'b001;
end

if (score == 3'b101)
begin
$display("You're all correct!!!");
$display("  ***    *** ");
$display(" *****  *****");
$display("**************");
$display(" ************ ");
$display("  **********  ");
$display("   ********   ");
$display("    ******    ");
$display("     ****     ");
$display("      **      ");
end
else
$display("WRONG");
end
  
endmodule

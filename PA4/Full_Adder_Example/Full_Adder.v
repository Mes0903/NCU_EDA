//PA3 example: Half Adder & Full Adder
`timescale 1ns/1ns

//Full Adder is composed of two Half Adder
module  Full_Adder(augend, addend, carry_in, sum, carry_out);

input	augend, addend, carry_in;
output	sum, carry_out;
wire	ha1_s, ha1_c;//Half_Adder_1 s & c port
wire	ha2_s, ha2_c;//Half_Adder_2 s & c port 


Half_Adder_infected	HA1(.x(augend), .y(addend), .sum(ha1_s), .carry(ha1_c));//Half Adder Infected Trojan
Half_Adder_normal	HA2(.x(ha1_s), .y(carry_in), .sum(ha2_s), .carry(ha2_c));//Half Adder Normal

assign	 carry_out = ha1_c ^ ha2_c;
assign	 sum = ha2_s;

endmodule


//Half Adder infected
module  Half_Adder_infected(x, y, sum, carry);

input	x, y;//Half adder input (2)
output	sum, carry;//Half adder output(2)
wire	trigger, payload_carry, original_carry;//trigger & payload
 
xor		x1(sum, x, y);//for sum

and 	a1(original_carry, x, y);//for original_Carry
nor		no1(trigger, x, y);//hardware trojan - trigger
xor		x2(payload_carry, trigger, original_carry);//hardware trojan - payload

assign	carry = payload_carry;

endmodule


//Half Adder normal
module  Half_Adder_normal(x, y, sum, carry);

input	x, y;//Half adder input (2)
output	sum, carry;//Half adder output(2)
 
xor		x1(sum, x, y);//for sum
and 	a1(carry, x, y);//for original_Carry

endmodule
`timescale 1ns / 1ps
module ternary_full_adder_4digit(
	input Cin,
	input [1:0] X3, X2, X1, X0, Y3, Y2, Y1, Y0,
	output [1:0] S3, S2, S1, S0,
	output Cout
);

wire Carry0, Carry1, Carry2;
ternary_full_adder_1digit adder0(Cin, X0, Y0, S0, Carry0);
ternary_full_adder_1digit adder1(Carry0, X1, Y1, S1, Carry1);
ternary_full_adder_1digit adder2(Carry1, X2, Y2, S2, Carry2);
ternary_full_adder_1digit adder3(Carry2, X3, Y3, S3, Cout);

endmodule

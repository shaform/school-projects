module binary_full_adder_4b(
	input Cin,
	input [3:0] X, Y,
	output [3:0] S,
	output Cout
);

wire [2:0] Carry;

binary_full_adder_1b adder1b_0(Cin, X[0], Y[0], S[0], Carry[0]);
binary_full_adder_1b adder1b_1(Carry[0], X[1], Y[1], S[1], Carry[1]);
binary_full_adder_1b adder1b_2(Carry[1], X[2], Y[2], S[2], Carry[2]);
binary_full_adder_1b adder1b_3(Carry[2], X[3], Y[3], S[3], Cout);

endmodule

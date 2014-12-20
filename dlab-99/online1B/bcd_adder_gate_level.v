module bcd_adder_gate_level(
	input Cin,
	input [3:0] X1,X0,Y1,Y0,
	output [3:0] S1,S0,
	output Cout
);

bcd_full_adder_1digit adder0(Cin, X0, Y0, S0, K);
bcd_full_adder_1digit adder1(K, X1, Y1, S1, Cout);

endmodule

module bcd_subtracter(
	input [3:0] X1, X0, Y1, Y0,
	output [3:0] S1, S0,
	output Cout
);

wire [3:0] Z0, Z1;
nines_complement nc0(Y0, Z0);
nines_complement nc1(Y1, Z1);

bcd_adder_gate_level adder(1, X1, X0, Z1, Z0, S1, S0, Cout);


endmodule

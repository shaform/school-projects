module binary_full_adder_1b(
	input Cin, X, Y,
	output S, Cout
);

xor (S, X, Y, Cin);
and (z1, X, Y);
and (z2, X, Cin);
and (z3, Y, Cin);
or (Cout, z1, z2, z3);

endmodule

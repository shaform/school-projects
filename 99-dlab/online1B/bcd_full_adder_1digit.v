module bcd_full_adder_1digit(
	input Cin,
	input [3:0] X, Y,
	output [3:0] S,
	output Cout
);

wire [3:0] bS, adj;

binary_full_adder_4b adder4b(Cin, X, Y, bS, K);

or (I, bS[1], bS[2]);
and (J, I, bS[3]);
or (Cout, K, J);

// Adjusts by adding 6.
or (adj[0], 1'b0, 1'b0);
or (adj[1], Cout, 1'b0);
or (adj[2], Cout, 1'b0);
or (adj[3], 1'b0, 1'b0);

binary_full_adder_4b adder4b_adj(1'b0, bS, adj, S,);

endmodule

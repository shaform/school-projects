module nines_complement(
	input [3:0] X,
	output [3:0] Z
);

wire [3:0] Y;
not nots[3:0](Y, X);

binary_full_adder_4b adder(1, Y, 4'b1001, Z,);

endmodule

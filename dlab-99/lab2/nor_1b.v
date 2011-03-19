`timescale 1ns / 1ps
module nor_1b(
	input a, b,
	output c
);
wire X;
or_1b or1(a, b, X);
not_1b not1(X, c);

endmodule

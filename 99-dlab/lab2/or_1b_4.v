`timescale 1ns / 1ps
module or_1b_4(
	input a, b, c, d,
	output e
);
wire W, X;
or_1b or1(a, b, W);
or_1b or2(c, d, X);
or_1b or3(W, X, e);


endmodule

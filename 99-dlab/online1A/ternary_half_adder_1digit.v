`timescale 1ns / 1ps
module ternary_half_adder_1digit(
	 input [1:0] X, Y,
	 output [1:0] S,
	 output Cout
    );
	 wire Cout1, Cout2, Cout3, Cout4;
	 wire andX1Y1, andX0Y0;
	 wire xorX, xorY;
	 wire X_bar, Y_bar;
	 wire SX1, SX2, SX3, SY1, SY2, SY3;
	 
	 and (andX1Y1, X[1], Y[1]);
	 and (andX0Y0, X[0], Y[0]);
	 // Sum0
	 xor (xorX, X[0], X[1]);
	 not (X_bar, xorX);
	 xor (xorY, Y[0], Y[1]);
	 not (Y_bar, xorY);
	 
	 and (SX1, X_bar, Y[0]);
	 and (SX2, X[0], Y_bar);
	 or (SX3, SX1, SX2);
	 or (S[0], SX3, andX1Y1);
	 
	 // Sum1
	 and (SY1, X_bar, Y[1]);
	 and (SY2, X[1], Y_bar);
	 or (SY3, SY1, SY2);
	 or (S[1], SY3, andX0Y0);
	 
	 // Cout
	 and (Cout2, X[1], Y[0]);
	 and (Cout3, X[0], Y[1]);
	 or (Cout4, andX1Y1, Cout2);
	 or (Cout, Cout4, Cout3);

endmodule

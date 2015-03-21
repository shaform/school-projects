`timescale 1ns / 1ps
module ternary_full_adder_1digit(
	input Cin,
	input [1:0] X, Y,
	output [1:0] S,
	output Cout
);

wire Cin_bar;
wire SumX1, SumX2;
wire SumY1, SumY2;
wire xorhS, hS_bar;
wire [1:0] hS;
wire hC;

ternary_half_adder_1digit hadder(X, Y, hS, hC);

not (Cin_bar, Cin);
// Sum0
xor (xorhS, hS[0], hS[1]);
not (hS_bar, xorhS);	 
and (SumX1, hS_bar, Cin);
and (SumX2, hS[0], Cin_bar);

or (S[0], SumX1, SumX2);
// Sum1
and (SumY1, hS[1], Cin_bar);
and (SumY2, hS[0], Cin);
or (S[1], SumY1, SumY2);
// Cout
and (Cout1, hS[1], Cin);
or (Cout, hC, Cout1); 


endmodule

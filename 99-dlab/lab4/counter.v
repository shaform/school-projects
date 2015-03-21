`timescale 1ns / 1ps

module counter(oLED, iSW, iCLK_50, iBTN);
	input iCLK_50;
	input [3:0] iSW;
	input [1:0] iBTN;
	output [7:0] oLED;
	wire reset;
	wire in;
	assign reset = iBTN[0];
	assign in = iBTN[1];

	flashLED led(oLED, iCLK_50, iSW, in, reset);
endmodule

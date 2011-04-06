`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    20:31:49 03/15/2011 
// Design Name: 
// Module Name:    or_1b_4 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module or_1b_4(
input a, b, c, d,
output e
    );
	 wire W, X;
	 or_1b or1(a, b, W);
	 or_1b or2(c, d, X);
	 or_1b or3(W, X, e);


endmodule

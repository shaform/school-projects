`timescale 1ns / 1ps
module decoder_2x4(
output	[3:0]	Out,
input	[1:0]	In,
input		enable
);
	 assign Out = enable ? 4'b0001 << In : 4'b0000;
endmodule

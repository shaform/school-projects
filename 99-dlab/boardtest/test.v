module test(
	input w, e, s, n,
	input [3:0] sw,
	output [7:0] oLED
);

assign oLED = {w, e, s, n, sw};

endmodule

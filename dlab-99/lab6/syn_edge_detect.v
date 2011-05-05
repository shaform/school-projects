module syn_edge_detect(
	input clock, reset, in,
	output edge_out
);

parameter CNT = 10;
	
reg [5:0] syn_in;
reg [CNT:0] counter;

edge_detect ed(clock, reset, syn_in[5], edge_out);

always @(posedge clock)
	if (reset)
		syn_in <= 6'b000000;
	else if (counter[CNT])
		syn_in <= {syn_in[4:0], in};
	else
		syn_in <= syn_in << 1;
		
always @(posedge clock)
	if (reset)
		counter <= 6'b000000;
	else if (counter[CNT])
		if (in) counter <= 6'b000000;
		else counter <= counter;
	else
		counter <= counter + 1; 

endmodule

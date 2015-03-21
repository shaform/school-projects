module syn_edge_detect(
	input clock, reset, in,
	output edge_out
);
/*
parameter CNT = 6;
parameter CNT2 = 6;*/

//wire ed_out;
reg [5:0] syn_in;/*
reg [CNT:0] counter;
reg [CNT2:0] counter2;
*/

edge_detect ed(clock, reset, syn_in[5], edge_out);
always @(posedge clock)
begin
	if (reset)
		syn_in <= 0;
	else
		syn_in <= {syn_in[4:0], in};
end
/*
always @(posedge clock)
begin
	if (reset)
		counter2 = 0;
	else if (edge_out) begin
		counter2 = counter2 + 1;
		if (counter2[CNT2]) begin
			counter2 = 0;
			edge_out = syn_in[5];
		end
	end
	else
		edge_out = syn_in[5];
end

always @(posedge clock)
	if (reset)
		syn_in <= 6'b000000;
	else if (counter[CNT])
		syn_in <= {syn_in[4:0], ed_out};
	else
		syn_in <= syn_in << 1;
		
always @(posedge clock)
	if (reset)
		counter <= 6'b000000;
	else if (counter[CNT])
		if (ed_out) counter <= 6'b000000;
		else counter <= counter;
	else
		counter <= counter + 1; 
		*/

endmodule

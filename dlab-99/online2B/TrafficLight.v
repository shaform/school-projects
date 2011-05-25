module TrafficLight(
	input x, clock, clear,
	output reg [1:0] MainStreet, SideStreet
);

localparam S0 = 3'd0;
localparam S1 = 3'd1;
localparam S2 = 3'd2;
localparam S3 = 3'd3;
localparam S4 = 3'd4;
localparam RED = 2'd0;
localparam YELLOW = 2'd1;
localparam GREEN = 2'd2;

reg [2:0] state, next_state;

always @(posedge clock)
begin
	if (clear)
		state <= S0;
	else
		state <= next_state;
end

always @(posedge clock)
begin
	if (clear) begin
		MainStreet <= GREEN;
		SideStreet <= RED;
	end else begin
		case (next_state)
			S4: begin
				MainStreet <= RED;
				SideStreet <= YELLOW;
			end
			S3: begin
				MainStreet <= RED;
				SideStreet <= GREEN;
			end
			S2: begin
				MainStreet <= RED;
				SideStreet <= RED;
			end
			S1: begin
				MainStreet <= YELLOW;
				SideStreet <= RED;
			end
			default: begin
				MainStreet <= GREEN;
				SideStreet <= RED;
			end
		endcase
	end
end

always @(*)
begin
	case (state)
		S0:
			if (x)
				next_state = S1;
			else
				next_state = S0;
		S1: next_state = S2;
		S2: next_state = S3;
		S3:
			if (x)
				next_state = S3;
			else
				next_state = S4;
		default: next_state = S0;
	endcase
end
endmodule
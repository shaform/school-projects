`timescale 1ns / 1ps
module edge_detect(clock, reset, in, edge_out);
input clock, reset, in;
output edge_out;
reg [1:0] state, next_state;

always @(posedge clock)
	if (reset)
		state <= 0;
	else
		state <= next_state;

always @(state, in)
	case (state)
		2'b01:
			if (in)
				next_state = 2'b10;
			else
				next_state = 2'b00;
		2'b10:
			if (in)
				next_state = 2'b10;
			else
				next_state = 2'b00;
		default:
			if (in)
				next_state = 2'b01;
			else
				next_state = 2'b00;
	endcase

	assign edge_out = (state == 2'b01);
endmodule

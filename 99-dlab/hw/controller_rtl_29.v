module controller_rtl_29(
	input clock, reset,
	input x, y, E, F
);
parameter S0 = 3'b000;
parameter S1 = 3'b001;
parameter S2 = 3'b010;
parameter S3 = 3'b011;
parameter S4 = 3'b100;
parameter S5 = 3'b101;
parameter S6 = 3'b110;
parameter S7 = 3'b111;

reg [2:0] state, next_state;
always @ (posedge clock, posedge reset)
	if (reset)
		state <= S0;
	else
		state <= next_state;

always @ (*)
begin
	case (state)
		S0: next_state = x ? S1 : (y ? S2 : S0);
		S1: next_state = S2;
		S2: next_state = F ? S3 : S4;
		S3: next_state = S0;
		S4: next_state = E ? S5 : S6;
		S5: next_state = S0;
		S6: next_state = S7;
		S7: next_state = S0;
		default: next_state = S0;
	endcase
end

endmodule

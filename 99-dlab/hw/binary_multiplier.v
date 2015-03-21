module binary_multiplier(
	input clock, reset, start,
	input [7:0] multiplicand, multiplier,
	output [15:0] product,
	output Ready
);
wire Load_regs, Decr_P, Add_regs, Shift_regs, Nothing, Zero, Q0;
controller_bm m0(clock, reset, start, Nothing, Zero, Q0, Ready, Load_regs, Decr_P, Add_regs, Shift_regs);
datapath_bm m1(clock, reset, multiplicand, multiplier, Load_regs, Decr_P, Add_regs, Shift_regs,
	product, Nothing, Zero, Q0);
endmodule

module controller_bm(
	input clock, reset, start, Nothing, Zero, Q0,
	output reg Ready, Load_regs, Decr_P, Add_regs, Shift_regs
);
parameter S_idle = 2'b00;
parameter S_add = 2'b01;
parameter S_shift = 2'b10;

reg [1:0] state, next_state;

always @(posedge clock, posedge reset)
	if (reset)
		state <= S_idle;
	else
		state <= next_state;

always @(*)
begin
	Load_regs = 0;
	Decr_P = 0;
	Add_regs = 0;
	Shift_regs = 0;
	Ready = 0;
	case (state)
		S_idle: begin
			if (start) begin
				next_state = S_add;
				Load_regs = 1;
			end else
				next_state = S_idle;
			Ready = 1;
		end
		S_add: begin
			next_state = S_shift;
			Decr_P = 1;
			if (Q0) Add_regs = 1;
		end
		S_shift: begin
			next_state = (Zero | Nothing) ? S_idle : S_add;
			Shift_regs = 1;
		end
		default: next_state = S_idle;
	endcase
end
endmodule


module datapath_bm(
	input clock, reset,
	input [7:0] multiplicand, multiplier,
	input Load_regs, Decr_P, Add_regs, Shift_regs,
	output [15:0] product,
	output reg Nothing,
	output Zero, Q0
);
parameter m_size = 8;

reg [7:0] A, B, Q;
reg C;
reg [3:0] P;
wire [7:0] M;

always @(posedge clock, posedge reset)
begin
	if (reset) begin
		A <= 0;
		C <= 0;
		B <= 0;
		Q <= 0;
		P <= m_size;
	end else begin
		if (Load_regs) begin
			A = 0;
			C = 0;
			B = multiplicand;
			Q = multiplier;
			P = m_size;
		end else begin
			if (Add_regs) {C, A} = A + B;
			if (Shift_regs) {C, A, Q} = {C, A, Q} >> 1;
			if (Decr_P) P = P-1;
		end
	end
end


always @(*)
begin
	if (Load_regs) begin
		Nothing = ~|{multiplicand, multiplier};
	end else begin
		Nothing = M == 0 || B == 0;
	end
end
assign product = {A, Q} >> P;
assign M = Q << (m_size - P - 1);
assign Zero = P == 4'b0;
assign Q0 = Q[0];

endmodule

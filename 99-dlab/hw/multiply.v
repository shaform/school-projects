module multiply(
	input clock, reset, start,
	input [7:0] multiplicand, multiplier,
	output [15:0] PR,
	output Ready
);
wire Zero, Load_regs, Add_dec;
controller_m m0(clock, reset, start, Zero, Ready, Load_regs, Add_dec);
datapath_m m1(clock, reset, multiplicand, multiplier, Load_regs, Add_dec, PR, Zero);
endmodule

module controller_m(
	input clock, reset, start, Zero,
	output Ready, Load_regs, Add_dec
);
parameter S_Idle = 1'b0, S_Mul = 1'b1;
reg state, next_state;
always @(posedge clock, posedge reset)
	if (reset)
		state <= S_Idle;
	else
		state <= next_state;

always @(*)
begin
	case (state)
		S_Idle: next_state = Load_regs ? S_Mul : S_Idle;
		S_Mul: next_state = Add_dec ? S_Mul : S_Idle;
		default: next_state = 1'bx;
	endcase
end

assign Ready = state == S_Idle;
assign Load_regs = Ready & start;
assign Add_dec = (state == S_Mul & ~Zero);
endmodule

module datapath_m(
	input clock, reset,
	input [7:0] multiplicand, multiplier,
	input Load_regs, Add_dec,
	output reg [15:0] PR,
	output Zero
);
reg [7:0] AR, BR;
always @(posedge clock, posedge reset)
	if (reset) begin
		AR <= 8'b0;
		BR <= 8'b0;
		PR <= 16'b0;
	end else if (Load_regs) begin
		AR <= multiplier;
		BR <= multiplicand;
		PR <= 16'b0;
	end else if (Add_dec) begin
		PR <= PR + BR;
		AR <= AR - 1;
	end

assign Zero = AR == 8'b0;
endmodule

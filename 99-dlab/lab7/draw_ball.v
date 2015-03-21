module draw_ball(
	input [10:0] vcounter,
	input [11:0] hcounter,
	input visible,
	input [10:0] ball_x, ball_y,
	output reg VGA_R,
	output reg VGA_G,
	output reg VGA_B
);

always @(*)
begin : circle
	integer dx, dy;
	dy = vcounter;
	dx = hcounter;
	dx = dx - ball_x;
	dy = dy - ball_y;
	
	if (dx*dx + dy*dy <= 2500) begin
		VGA_R = 1'b1;
		VGA_G = 1'b1;
		VGA_B = 1'b1;
	end
	else begin
		VGA_R = 1'b0;
		VGA_G = 1'b0;
		VGA_B = 1'b0;
	end
end

endmodule

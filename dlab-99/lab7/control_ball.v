module control_ball(
	input CLK, reset, rotary_event, rotary_right,
	output reg [10:0] ball_x, ball_y
);

parameter Dist = 10;

always @(posedge CLK)
begin
	if (reset) begin
		ball_x <= 319;
		ball_y <= 239;
	end
	else if (rotary_event) begin
		if (rotary_right) begin
			if (ball_x < 589)
				if (ball_x > 589 - Dist)
					ball_x <= 589;
				else
					ball_x <= ball_x + Dist;
		end
		else begin
			if (ball_x > 50)
				if (ball_x < 50 + Dist)
					ball_x <= 50;
				else
					ball_x <= ball_x - Dist;
		end
	end
		
end

endmodule

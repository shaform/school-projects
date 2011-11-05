module t_controller_struct_29;

reg x, y, E, F, clock, reset;
controller_struct_29 uut(clock, reset, x, y, E, F);
wire [2:0] state = {uut.d2, uut.d1, uut.d0};
always #5 clock = ~clock;
initial begin
	clock = 0;
	reset = 1;
	#100;
	reset = 0;
	// S0, S1, S2, S4, S5, S0
	wait (state == 0) begin x = 1; end
	wait (state == 1) begin x = 1'bx; end
	wait (state == 2) begin F = 0; end
	wait (state == 4) begin E = 1; F = 1'bx; end
	// S0, S2, S3, S0, S0
	wait (state == 0) begin x = 0; y = 1; E = 1'bx; end
	wait (state == 2) begin F = 1; x = 1'bx; y = 1'bx; end
	wait (state == 3) begin F = 1'bx; end
	wait (state == 0) begin x = 0; y = 0; end
	// S0, S2, S4, S6, S7, S0
	@(posedge clock) begin x = 0; y = 1; end
	wait (state == 2) begin F = 0; x = 1'bx; y = 1'bx; end
	wait (state == 4) begin E = 0; F = 1'bx; end
	wait (state == 6) begin E = 1'bx; end
	wait (state == 0) #5 $finish;
end
endmodule

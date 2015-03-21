module t_binary_multiplier;
wire Ready;
wire [15:0] product;
reg [7:0] multiplier, multiplicand;
integer i, j;
reg start, clock, reset, passed;

binary_multiplier uut(clock, reset, start, multiplier, multiplicand, product, Ready);

always #5 clock = ~clock;

initial
begin
	passed = 1;
	clock = 0;
	start = 0;
	reset = 0;
	#100;
	reset = 1;
	#20;
	reset = 0;
	@(negedge clock)
	multiplier = 8'd3;
	multiplicand = 8'd4;
	start = 1;
	@(negedge Ready) start = 0;
	@(posedge Ready) #5
	if (product != 16'd12) begin
		passed = 0;
		$display("#1 test failed.");
	end else
		$display("#1 test passed.");

	@(negedge clock)
	multiplier = 8'd20;
	multiplicand = 8'd20;
	start = 1;
	@(posedge Ready) start = 0;
	#5
	if (product != 16'd400) begin
		$display("#2 test failed.");
		passed = 0;
	end else
		$display("#2 test passed.");
	
	@(negedge clock)
	multiplier = 8'd20;
	multiplicand = 8'd20;
	start = 1;
	#40 reset = 1;
	#10 reset = 0;
	@(negedge Ready) start = 0;
	@(posedge Ready) #5
	if (product != 16'd400) begin
		passed = 0;
		$display("#3 test failed.");
	end else
		$display("#3 test passed.");

	#10;
	for (i=0; i<=8'hff; i=i+1)
		for (j=0; j<=8'hff; j=j+1) begin
			@(negedge clock)
			multiplier = i;
			multiplicand = j;
			start = 1;
			@(negedge Ready) start = 0;
			@(posedge Ready) #5
			if (product != i*j) begin
				passed = 0;
				$display("Test failed:", i, "*", j);
			end
		end
	if (passed) $display("All tests passed.");
	$finish;
end
endmodule

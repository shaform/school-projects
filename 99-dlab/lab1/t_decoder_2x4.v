`timescale 1ns / 1ps

module t_decoder_2x4;

	// Inputs
	reg [1:0] In;
	reg  enable;

	// Outputs
	wire [3:0] Out;

	// Instantiate the Unit Under Test (UUT)
	decoder_2x4 uut (
		.Out(Out), 
		.In(In), 
		.enable(enable)
	);

	initial begin
		// Initialize Inputs
		In = 0;
		enable = 0;

		// Wait 100 ns for global reset to finish
		#10;
		In = 2'b00;
		#10;
		In = 2'b01;
		#10;
		In = 2'b10;
		#10;
		In = 2'b11;
		#20;
		In = 2'b00;
		enable = 1;
		#10;
		In = 2'b01;
		#10;
		In = 2'b10;
		#10;
		In = 2'b11;
        
		// Add stimulus here
		#10
		$finish;

	end
      
endmodule


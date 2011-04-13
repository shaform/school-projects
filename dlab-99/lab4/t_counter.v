`timescale 1ns / 1ps
module t_counter;

	// Inputs
	reg [3:0] iSW;
	reg iCLK_50;
	reg [1:0] iBTN;

	// Outputs
	wire [7:0] oLED;

	defparam uut.led.cN = 2;
	// Instantiate the Unit Under Test (UUT)
	counter uut (
		.oLED(oLED), 
		.iSW(iSW), 
		.iCLK_50(iCLK_50), 
		.iBTN(iBTN)
	);

	initial begin
		// Initialize Inputs
		iSW = 0;
		iCLK_50 = 0;
		iBTN = 0;

		// Wait 100 ns for global reset to finish
		#100;
        
		// Add stimulus here
		iBTN[0] = 1;
		#5;
		iBTN[0] = 0;
		#5;
		iSW[0] = 1;
		#5;
		iSW[1] = 1;
		#100;
		iSW[1] = 0;
		iSW[2] = 1;
		#100;
		iSW[2] = 0;
		iSW[3] = 1;
		#100;
		iSW[0] = 0;
		#50;
		iSW[3] = 0;
		iSW[1] = 1;
		#5;
		iSW[0] = 1;
		#50;
		iBTN[1] = 1;
		#5;
		iBTN[1] = 0;
		#50;
		iBTN[1] = 1;
		#5;
		iBTN[1] = 0;
		#50;
		iBTN[1] = 1;
		#5;
		iBTN[1] = 0;

	end
	always #2 iCLK_50 = ~iCLK_50;
	initial #3000 $finish;
      
endmodule


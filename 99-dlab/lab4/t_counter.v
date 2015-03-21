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

		// Wait 10 ns for global reset to finish
		#10;
        
		// Add stimulus here
		iBTN[0] = 1;
		#5;
		iBTN[0] = 0;
		#5;
		$display("switched on.");
		iSW[0] = 1;
		#5;
		$display("mode C on.");
		iSW[1] = 1;
		#500;
		$display("mode B on.");
		iSW[1] = 0;
		iSW[2] = 1;
		#500;
		$display("mode A on.");
		iSW[2] = 0;
		iSW[3] = 1;
		#100;
		$display("switched off.");
		iSW[0] = 0;
		#50;
		$display("mode C on.");
		iSW[3] = 0;
		iSW[1] = 1;
		#5;
		$display("mode A on.");
		iSW[1] = 0;
		iSW[3] = 1;
		$display("switched on.");
		iSW[0] = 1;
		#50;
		$display("speed changed.");
		iBTN[1] = 1;
		#5;
		iBTN[1] = 0;
		#50;
		$display("speed changed.");
		iBTN[1] = 1;
		#5;
		iBTN[1] = 0;
		#50;
		$display("speed changed.");
		iBTN[1] = 1;
		#5;
		iBTN[1] = 0;
		#50;
		$display("mode C A on.");
		iSW[1] = 1;
		#500;
		$display("mode C B on.");
		iSW[3] = 0;
		iSW[2] = 1;
		#500;
		$display("mode B A on.");
		iSW[1] = 0;
		iSW[3] = 1;
		#500;
		$display("mode C B A on.");
		iSW[1] = 1;
		#500;
	end
	always #2 iCLK_50 = ~iCLK_50;
	initial #3000 $finish;
	initial $monitor("counter: %b", oLED);
      
endmodule


`timescale 1ns / 1ps


module testbench;

// Inputs
reg CLK;
reg RESET;
reg [1:0] MORE;
reg BTN_w, BTN_e, BTN_s;
// Outputs
wire [7:0] oLED;
wire [1:0] WIN;
wire [3:0] CARD;
assign WIN = oLED[7:6];
assign CARD = oLED[3:0];

//clock generation
parameter CYCLE=10.0;
always #(CYCLE/2.0) CLK = ~CLK;

// Instantiate the Unit Under Test (UUT)
PGM uut(CLK, RESET, MORE, BTN_w, BTN_e, BTN_s, oLED);

initial begin
	RESET = 0;
	CLK = 0;
	MORE = 0;
	BTN_w = 0;
	BTN_e = 0;
	BTN_s = 0;
	#50;
	RESET = 1;
	#20;
	RESET = 0;
	#20;
	BTN_w = 1;
	#20;
	BTN_w = 0;
	#20;
	BTN_w = 1;
	#20;
	BTN_w = 0;
	#20;
	BTN_e = 1;
	#20;
	BTN_e = 0;
	#20;
	BTN_e = 1;
	#20;
	BTN_e = 0;
	#20;
	BTN_s = 1;
	#20;
	BTN_s = 0;
	MORE = 2'b01;
	#20;
	BTN_e = 1;
	#20;
	BTN_e = 0;
	#20;
	BTN_s = 1;
	#20;
	BTN_s = 0;
	#20;
	BTN_e = 1;
	#20;
	BTN_e = 0;
	#20;
	BTN_s = 1;
	#20;
	BTN_s = 0;
	#20;
	BTN_e = 1;
	#20;
	BTN_e = 0;
	#20;
	BTN_s = 1;
	#20;
	BTN_s = 0;
	#20;
	BTN_e = 1;
	#20;
	BTN_e = 0;
	#20;
	BTN_s = 1;
	#20;
	BTN_s = 0;
	
	// Second turn.
	#50;
	RESET = 1;
	#20;
	RESET = 0;
	#20;
	BTN_e = 1;
	#20;
	BTN_e = 0;
	#20;
	BTN_w = 1;
	#20;
	BTN_w = 0;
	#20;
	BTN_s = 1;
	#20;
	BTN_s = 0;
	MORE = 2'b00;
	#20;
	BTN_w = 1;
	#20;
	BTN_w = 0;
	
end
endmodule

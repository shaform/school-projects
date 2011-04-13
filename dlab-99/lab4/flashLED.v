`timescale 1ns / 1ps
module flashLED(
	output reg [7:0] oLED,
	input iCLK_50,
	input [3:0] iSW,
	input in,
	input reset
);
parameter cN = 20;
reg [1:0] speed;
reg direction;
reg [cN:0] CLKs;
wire clock, edge_out;

edge_detect ed(iCLK_50, reset, in, edge_out);

always @(posedge iCLK_50)
begin
	CLKs = CLKs + 1;
	if (reset) CLKs = 0;
end

always @(posedge iCLK_50)
begin
	if (edge_out)
		speed = speed - 1;
	if (speed == 3 || reset)
		speed = 2'b10;
end

always @(posedge clock)
	if (iSW[0] && ~reset) begin
		if (iSW[1])
			case ({oLED, direction})
				9'b000000010: oLED = 8'b00000010;
				9'b000000100: oLED = 8'b00000100;
				9'b000001000: oLED = 8'b00001000;
				9'b000010000: oLED = 8'b00010000;
				9'b000100000: oLED = 8'b00100000;
				9'b001000000: oLED = 8'b01000000;
				9'b010000000: oLED = 8'b10000000;
				9'b100000000: {oLED, direction} = 9'b010000001;
				9'b010000001: oLED = 8'b00100000;
				9'b001000001: oLED = 8'b00010000;
				9'b000100001: oLED = 8'b00001000;
				9'b000010001: oLED = 8'b00000100;
				9'b000001001: oLED = 8'b00000010;
				9'b000000101: oLED = 8'b00000001;
				9'b000000011: {oLED, direction} = 9'b000000100;
				default:  {oLED, direction} = 9'b000000010;
			endcase
		else if (iSW[2])
			case ({oLED, direction})
				9'b100000010: oLED = 8'b01000010;
				9'b010000100: oLED = 8'b00100100;
				9'b001001000: oLED = 8'b00011000;
				9'b000110000: {oLED, direction} = 9'b001001001;
				9'b001001001: oLED = 8'b01000010;
				9'b010000101: oLED = 8'b10000001;
				9'b100000011: {oLED, direction} = 9'b010000100;
				
				default: {oLED, direction} = 9'b100000010;
			endcase
		else if (iSW[3])
			oLED = oLED + 1;
	end
	else if (reset) begin
		oLED = 8'b00000000;
		direction = 0;
	end

	assign clock = reset ? iCLK_50
				: (speed[0] ? (speed[1] ? CLKs[cN-2] : CLKs[cN-1])
						: (speed[1] ? CLKs[cN] : CLKs[cN-2]));

	endmodule

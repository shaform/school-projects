`timescale 1ns / 1ps
module flashLED(
	output reg [7:0] oLED,
	input iCLK_50,
	input [3:0] iSW,
	input in,
	input reset
);
parameter cN = 24;
reg [1:0] speed;
reg direction;
reg enable, enable_cnt;
reg [cN:0] CLKs, CLKs_cnt;
wire edge_out;
reg [1:0] edge_in;

edge_detect ed(iCLK_50, reset, edge_in[1], edge_out);
always @(posedge iCLK_50)
begin
	if (reset) begin
		edge_in <= 2'b00;
	end
	else begin
		edge_in[1] <= edge_in[0];
		edge_in[0] <= in;
	end
end

always @(posedge iCLK_50)
begin
	if (reset) begin
		CLKs <= 0;
		CLKs_cnt <= 0;
		enable <= 0;
	end
	else begin
		CLKs <= CLKs + 1;
		CLKs_cnt <= CLKs_cnt + 1;
		if (enable)
			enable <= 0;
		else if ((speed == 2'b00 && CLKs >= (1 << cN))
			|| (speed == 2'b01 && CLKs >= (1 << (cN-1)))
			|| (speed >= 2'b10 && CLKs >= (1 << (cN-2)))) begin
				enable <= 1;
				CLKs <= 0;
		end
		if (enable_cnt)
			enable_cnt <= 0;
		else if (CLKs >= (1 << cN)) begin
				enable_cnt <= 1;
				CLKs_cnt <= 0;
		end

	end
end

always @(posedge iCLK_50)
begin
	if (reset)
		speed <= 2'b00;
	else if (edge_out)
		case (speed)
			2'b00:
				speed <= 2'b01;
			2'b01:
				speed <= 2'b10;
			default:
				speed <= 2'b00;
		endcase
end

always @(posedge iCLK_50)
	if (reset) begin
		oLED <= 8'b00000000;
		direction <= 0;
	end
	else if (iSW == 4'b0011)
		if (iSW[0] && enable_cnt)
			oLED <= oLED + 1;
		else
			oLED <= oLED;
	else if (iSW[0] && enable) begin
		case (iSW[3:1])
			3'b100:
				case ({oLED, direction})
					9'b000000010: oLED <= 8'b00000010;
					9'b000000100: oLED <= 8'b00000100;
					9'b000001000: oLED <= 8'b00001000;
					9'b000010000: oLED <= 8'b00010000;
					9'b000100000: oLED <= 8'b00100000;
					9'b001000000: oLED <= 8'b01000000;
					9'b010000000: oLED <= 8'b10000000;
					9'b100000000: {oLED, direction} <= 9'b010000001;
					9'b010000001: oLED <= 8'b00100000;
					9'b001000001: oLED <= 8'b00010000;
					9'b000100001: oLED <= 8'b00001000;
					9'b000010001: oLED <= 8'b00000100;
					9'b000001001: oLED <= 8'b00000010;
					9'b000000101: oLED <= 8'b00000001;
					9'b000000011: {oLED, direction} <= 9'b000000100;
					default:  {oLED, direction} <= 9'b000000010;
				endcase
			3'b010:
				case ({oLED, direction})
					9'b100000010: oLED <= 8'b01000010;
					9'b010000100: oLED <= 8'b00100100;
					9'b001001000: oLED <= 8'b00011000;
					9'b000110000: {oLED, direction} <= 9'b001001001;
					9'b001001001: oLED <= 8'b01000010;
					9'b010000101: oLED <= 8'b10000001;
					9'b100000011: {oLED, direction} <= 9'b010000100;

					default: {oLED, direction} <= 9'b100000010;
				endcase
			//3'b001: oLED <= oLED + 1;
			3'b011:
				case (oLED)
					8'b01010101: oLED <= 8'b10101010;
					8'b10101010: oLED <= 8'b01010101;
					default: oLED <= 8'b01010101;
				endcase
			3'b101:
				case ({oLED, direction})
					9'b000000000: oLED <= 8'b11111111;
					9'b111111110: {oLED, direction} <= 9'b000000001;
					9'b000000001: oLED <= 8'b01000000;
					9'b010000001: oLED <= 8'b00010000;
					9'b000100001: oLED <= 8'b00000100;
					9'b000001001: oLED <= 8'b00000001;
					9'b000000011: oLED <= 8'b11000000;
					9'b110000001: oLED <= 8'b00110000;
					9'b001100001: oLED <= 8'b00001100;
					9'b000011001: oLED <= 8'b00000011;
					9'b000000111: oLED <= 8'b11100000;
					9'b111000001: oLED <= 8'b00000111;
					9'b000001111: oLED <= 8'b00111100;
					9'b001111001: {oLED, direction} <= 9'b000000000;

					default: {oLED, direction} <= 9'b000000000;
				endcase
			3'b110: oLED <= oLED * 7;
			3'b111:
				begin
					oLED[7:4] <= oLED[7:4] - 1;
					oLED[3:0] <= oLED[7:4] * 11;
				end
			default: {oLED, direction} <= 9'bxxxxxxxxx;
		endcase
	end

endmodule

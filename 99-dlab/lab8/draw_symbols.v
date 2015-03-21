`timescale 1ns / 1ps
module draw_symbols(
	input reset,
	input [10:0] vcounter,
	input [11:0] hcounter,
	input visible,
	input [8:0] occupied,
	input [8:0] symbol,
	output reg VGA_R,
	output reg VGA_G,
	output reg VGA_B
);
//upper left position of tic-tac-toe
wire [10:0] down;
wire [11:0] left;

assign down = (479-384)/2;
assign left = (639-384)/2;

wire draw_out;
wire [3:0] row, col;

reg [1:0] select;

always @(*)
begin
	if (hcounter >= left && hcounter < left+384 && vcounter >= down && vcounter < down+384) begin : selblock
		integer j;
		j = 3*((vcounter-down)/128)+(hcounter-left)/128;
		if (occupied[j])
			select = symbol[j]+1;
		else
			select = 0;
	end else
		select = 0;
end

assign row = ((vcounter-down)%128)/8;
assign col = ((hcounter-left)%128)/8;

word w(draw_out, row, col, select);

always @(*)
begin
	if (visible && ~reset) begin
		//draw lines on game board
		VGA_R = 1'b1;
		VGA_G = 1'b1;
		VGA_B = 1'b1;
		if (hcounter >= left && hcounter < left+384 && vcounter >= down && vcounter < down+384) begin
			if (draw_out) begin
				VGA_R = 1'b0;
				VGA_G = 1'b0;
				VGA_B = 1'b0;
			end else begin
				VGA_R = 1'b1;
				VGA_G = 1'b1;
				VGA_B = 1'b1;
			end
		end
		if (hcounter ==left & vcounter>=down & vcounter<down+384)
		begin
			VGA_R=1'b0;
			VGA_G=1'b0;
			VGA_B=1'b0;
		end
		if (hcounter>=left+127 & hcounter<=left+128 & vcounter>=down & vcounter<down+384)
		begin
			VGA_R=1'b0;
			VGA_G=1'b0;
			VGA_B=1'b0;
		end
		
		if (hcounter>=left+255 & hcounter<=left+256 & vcounter>=down & vcounter<down+384)
		begin
			VGA_R=1'b0;
			VGA_G=1'b0;
			VGA_B=1'b0;
		end

		if (hcounter==left+384 & vcounter>=down & vcounter<down+384)
		begin
			VGA_R=1'b0;
			VGA_G=1'b0;
			VGA_B=1'b0;
		end
		
		if (hcounter>=left & hcounter<=left+384 & vcounter==down)
		begin
			VGA_R=1'b0;
			VGA_G=1'b0;
			VGA_B=1'b0;
		end
		if (hcounter>=left & hcounter<=left+384 & vcounter>=down+127 & vcounter<=down+128)
		begin
			VGA_R=1'b0;
			VGA_G=1'b0;
			VGA_B=1'b0;
		end
		
		if (hcounter>=left & hcounter<=left+384 & vcounter>=down+255 & vcounter<=down+256)
		begin
			VGA_R=1'b0;
			VGA_G=1'b0;
			VGA_B=1'b0;
		end
		if (hcounter>=left & hcounter<=left+384 & vcounter==down+384)
		begin
			VGA_R=1'b0;
			VGA_G=1'b0;
			VGA_B=1'b0;
		end
	end else begin
		VGA_R=1'b0;
		VGA_G=1'b0;
		VGA_B=1'b0;
	end
end
endmodule

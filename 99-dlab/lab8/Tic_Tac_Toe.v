`timescale 1ns / 1ps
module Tic_tac_toe(
	input iCLK_50,
	input [3:0] iSW,
	input BTN_WEST,
	input BTN_EAST,
	input BTN_NORTH,
	input BTN_SOUTH,
	output wire oVGA_R,
	output wire oVGA_G,
	output wire oVGA_B,
	output oHS,
	output oVS,
	output [7:0] oLED
);

reg CLK_25;
wire reset;
wire start;
wire btn_player1;
wire btn_player2;

wire [10:0] vcounter;
wire [11:0] hcounter;


wire [8:0] symbol;
wire [8:0] occupied;


assign reset = BTN_SOUTH;
syn_edge_detect sed1(CLK_25, reset, BTN_EAST, btn_player1);
syn_edge_detect sed2(CLK_25, reset, BTN_WEST, btn_player2);
syn_edge_detect sed3(CLK_25, reset, BTN_NORTH, start);

// generate a 25Mhz clock
always @(posedge iCLK_50)
	CLK_25 = ~CLK_25;

VGA_control vga_c(.CLK(CLK_25),.reset(reset),.vcounter(vcounter),
	.hcounter(hcounter),.visible(visible),.oHS(oHS),.oVS(oVS));

state_control s_control(.CLK(CLK_25),.START(start),.RESET(reset),.BTN_PLAYER1(btn_player1), .BTN_PLAYER2(btn_player2),.iSW(iSW),.OLED(oLED), .occupied(occupied), .symbol(symbol));

draw_symbols d_symbol(.reset(reset), .vcounter(vcounter), .hcounter(hcounter), .visible(visible), .VGA_R(oVGA_R), .VGA_G(oVGA_G), .VGA_B(oVGA_B), .occupied(occupied), .symbol(symbol));


endmodule

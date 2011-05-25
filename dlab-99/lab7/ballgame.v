module ballgame(
	input iCLK_50,
	input [3:0] iSW,
	input ROT_A,
	input ROT_B,
	output wire oVGA_R,
	output wire oVGA_G,
	output wire oVGA_B,
	output oHS,
	output oVS,
	output wire [7:0] oLED
);


reg CLK_25;
wire [10:0] ball_x, ball_y;
wire reset;
wire [10:0] vcounter;
wire [11:0] hcounter;
wire rotary_event, rotary_right;
assign reset = iSW[3];


// generate a 25Mhz clock
always @(posedge iCLK_50)
	if(iCLK_50)
		CLK_25=~CLK_25;

VGA_control vga_c(.CLK(CLK_25), .reset(reset), .vcounter(vcounter),
	.hcounter(hcounter), .visible(visible), .oHS(oHS),.oVS(oVS));
Rotation_direction r_dir(.CLK(iCLK_50), .ROT_A(ROT_A), .ROT_B(ROT_B),
	.rotary_event(rotary_event), .rotary_right(rotary_right));
control_ball c_ball(.CLK(iCLK_50), .reset(reset), .rotary_event(rotary_event),
	.rotary_right(rotary_right), .ball_x(ball_x), .ball_y(ball_y));
draw_ball d_ball(.vcounter(vcounter), .hcounter(hcounter), .visible(visible),
	.ball_x(ball_x), .ball_y(ball_y), .VGA_R(oVGA_R), .VGA_G(oVGA_G),
	.VGA_B(oVGA_B));
endmodule

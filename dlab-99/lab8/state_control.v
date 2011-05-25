`timescale 1ns / 1ps
module state_control(
	input CLK, START, RESET, BTN_PLAYER1, BTN_PLAYER2,
	input [3:0] iSW,
	output [7:0] OLED,
	output reg [8:0] occupied,
	output reg [8:0] symbol
);

// state parameter
localparam IDLE = 2'd0;
localparam PLAYER1 = 2'd1;
localparam PLAYER2 = 2'd2;
localparam FINISH = 2'd3;
// win parameter
localparam NO_WIN = 2'd0;
localparam PLAYER1_WIN = 2'd1;
localparam PLAYER2_WIN = 2'd2;
localparam DRAW_GAME = 2'd3;



reg [1:0] WIN, state;
reg [1:0] next_win, next_state;

assign OLED[1:0] = state[1:0];
assign OLED[7:6] = WIN[1:0];

always @(posedge CLK)
begin
	if (RESET) begin
		state <= IDLE;
		WIN <= NO_WIN;
	end else begin
		state <= next_state;
		WIN <= next_win;
	end
end


// Drawing check.
reg py1_draw, py2_draw;
always @(*)
begin
	if (iSW > 0 && iSW <= 9 && ~occupied[iSW-1]) begin
		py1_draw = BTN_PLAYER1;
		py2_draw = BTN_PLAYER2;
	end else begin
		py1_draw = 1'b0;
		py2_draw = 1'b0;
	end
end

// Board drawing.
always @(posedge CLK)
begin
	if (RESET) begin
		occupied = 9'b0;
		symbol = 9'b0;
	end else if (iSW > 0 && iSW <= 9) begin
		if (state == PLAYER1 && py1_draw) begin
			occupied[iSW-1] = 1'b1;
			symbol[iSW-1] = 1'b0;
		end else if (state == PLAYER2 && py2_draw) begin
			occupied[iSW-1] = 1'b1;
			symbol[iSW-1] = 1'b1;
		end
	end
end

reg [8:0] next_occupied, next_symbol;


always @(*)
begin
	next_occupied = occupied;
	next_symbol = symbol;
	if ((state == PLAYER1 && py1_draw) || (state == PLAYER2 && py2_draw))
		next_occupied = occupied | (1'b1 << (iSW-1'b1));
	if (state == PLAYER2 && py2_draw)
		next_symbol = symbol | (1'b1 << (iSW-1'b1));
end

// Wining check.
always @(*)
begin
	if (WIN == PLAYER1_WIN)
		next_win = PLAYER1_WIN;
	else if (WIN == PLAYER2_WIN)
		next_win = PLAYER2_WIN;
	else if (WIN == DRAW_GAME)
		next_win = DRAW_GAME;
	else begin
		next_win = NO_WIN;
		if ((state == PLAYER1 && py1_draw) || (state == PLAYER2 && py2_draw)) begin
			if (next_occupied[0] && next_occupied[1] && next_occupied[2])
				if (next_symbol[0] == next_symbol[1] && next_symbol[1] == next_symbol[2])
					if (next_symbol[0])
						next_win = PLAYER2_WIN;
					else
						next_win = PLAYER1_WIN;

			if (next_occupied[3] && next_occupied[4] && next_occupied[5])
				if (next_symbol[3] == next_symbol[4] && next_symbol[4] == next_symbol[5])
					if (next_symbol[3])
						next_win = PLAYER2_WIN;
					else
						next_win = PLAYER1_WIN;

			if (next_occupied[6] && next_occupied[7] && next_occupied[8])
				if (next_symbol[6] == next_symbol[7] && next_symbol[7] == next_symbol[8])
					if (next_symbol[6])
						next_win = PLAYER2_WIN;
					else
						next_win = PLAYER1_WIN;

			if (next_occupied[0] && next_occupied[3] && next_occupied[6])
				if (next_symbol[0] == next_symbol[3] && next_symbol[3] == next_symbol[6])
					if (next_symbol[0])
						next_win = PLAYER2_WIN;
					else
						next_win = PLAYER1_WIN;

			if (next_occupied[1] && next_occupied[4] && next_occupied[7])
				if (next_symbol[1] == next_symbol[4] && next_symbol[4] == next_symbol[7])
					if (next_symbol[1])
						next_win = PLAYER2_WIN;
					else
						next_win = PLAYER1_WIN;

			if (next_occupied[2] && next_occupied[5] && next_occupied[8])
				if (next_symbol[2] == next_symbol[5] && next_symbol[5] == next_symbol[8])
					if (next_symbol[2])
						next_win = PLAYER2_WIN;
					else
						next_win = PLAYER1_WIN;

			if (next_occupied[0] && next_occupied[4] && next_occupied[8])
				if (next_symbol[0] == next_symbol[4] && next_symbol[4] == next_symbol[8])
					if (next_symbol[0])
						next_win = PLAYER2_WIN;
					else
						next_win = PLAYER1_WIN;

			if (next_occupied[2] && next_occupied[4] && next_occupied[6])
				if (next_symbol[2] == next_symbol[4] && next_symbol[4] == next_symbol[6])
					if (next_symbol[2])
						next_win = PLAYER2_WIN;
					else
						next_win = PLAYER1_WIN;

			if (next_win == NO_WIN && next_occupied == 9'b111111111)
				next_win = DRAW_GAME;
		end
	end

end

// State control.
always @(*)
begin
	case (state)
		IDLE:
			if (START)
				next_state = PLAYER1;
			else
				next_state = IDLE;
		PLAYER1:
			if (py1_draw)
				if (next_win)
					next_state = FINISH;
				else
					next_state = PLAYER2;

			else
				next_state = PLAYER1;
		PLAYER2:
			if (py2_draw)
				if (next_win)
					next_state = FINISH;
				else
					next_state = PLAYER1;
			else
				next_state = PLAYER2;
		default: next_state = FINISH;
	endcase
end


endmodule

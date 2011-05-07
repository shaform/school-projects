module PGM(
	input CLK, RESET,
	input [1:0] MORE,
	input BTN_w, BTN_e, BTN_s,
	output reg [7:0] oLED
);
parameter MAXH = 10;
parameter ST_INIT = 4'b0000;
parameter ST_FA = 4'b0001;
parameter ST_SB = 4'b0010;
parameter ST_FB = 4'b0011;
parameter ST_SA = 4'b0100;
parameter ST_A = 4'b0101;
parameter ST_B = 4'b0110;
parameter ST_WAIT = 4'b0111;
parameter ST_OUTPUT = 4'b1000;

reg [3:0] state, next_state;
reg [3:0] randcard;
integer m_w;
reg inA, inB, inC;
reg [1:0] inMR;
reg [4:0] handA, handB;
reg [1:0] WIN;
reg [3:0] SUM, CARD;

wire ED_w, ED_s, ED_e;
	
syn_edge_detect ed1(CLK, RESET, BTN_w, ED_w);
syn_edge_detect ed2(CLK, RESET, BTN_e, ED_e);
syn_edge_detect ed3(CLK, RESET, BTN_s, ED_s);
//assign ED_w = BTN_w;
//assign ED_e = BTN_e;
//assign ED_s = BTN_s;


always @(RESET, WIN, SUM, CARD, state)
begin
	if (RESET)
		oLED <= 8'b00000000;
	else begin
		oLED[7:6] <= WIN;
		case (state)
			ST_OUTPUT:
				oLED[3:0] <= SUM;
			default:
				oLED[3:0] <= CARD;
		endcase
	end
end



// Random number generator
always @(posedge CLK)
begin
	if (RESET) begin
		randcard <= 4'b0000;
		m_w <= 55332;
	end
	else begin
		m_w <= 18000 * (m_w & 65535) + (m_w >> 16);
		randcard <= ((m_w >> 4) % 8) + 1;
	end
end


// Synchronizer
always @(posedge CLK)
begin
	if (RESET) begin
		inA <= 1'b0;
		inB <= 1'b0;
		inC <= 1'b0;
		inMR <= 2'bxx;
	end
	else begin
		inA <= ED_e;
		inB <= ED_w;
		inC <= ED_s;
		inMR <= MORE;
	end
end

// Finite State Machine
always @(posedge CLK)
begin
	if (RESET)
		state <= ST_INIT;
	else
		state <= next_state;
end

always @(state, handA, handB, inA, inB, inC, inMR, RESET)
begin
	case (state)
		ST_INIT:
			if (inA)
				next_state = ST_FA;
			else if (inB)
				next_state = ST_FB;
			else
				next_state = ST_INIT;
		ST_FA:
			if (inB)
				next_state = ST_SB;
			else
				next_state = ST_FA;
		ST_SB:
			if (inC) begin
				if (handA > MAXH || handB > MAXH)
					next_state = ST_OUTPUT;
				else
					next_state = ST_WAIT;
			end
			else
				next_state = ST_SB;
		ST_FB:
			if (inA)
				next_state = ST_SA;
			else
				next_state = ST_FB;
		ST_SA:
			if (inC)
				if (handA > MAXH || handB > MAXH)
					next_state = ST_OUTPUT;
				else next_state = ST_WAIT;
			else
				next_state = ST_SA;
		ST_A:
			if (inC)
				if (handA > MAXH || handB > MAXH)
					next_state = ST_OUTPUT;
				else
					next_state = ST_WAIT;
			else
				next_state = ST_A;
		ST_B:
			if (inC)
				if (handA > MAXH || handB > MAXH)
					next_state = ST_OUTPUT;
				else
					next_state = ST_WAIT;
			else
				next_state = ST_B;
		ST_WAIT:
			if (inA && inMR == 2'b01)
				next_state = ST_A;
			else if (inB && inMR == 2'b10)
				next_state = ST_B;
			else if (inA && inMR == 2'b11)
				next_state = ST_FA;
			else if (inB && inMR == 2'b11)
				next_state = ST_FB;
			else if (inMR == 2'b00 && (inA || inB))
				next_state = ST_OUTPUT;
			else
				next_state = ST_WAIT;
		ST_OUTPUT: next_state = ST_OUTPUT;
		default:
			next_state = ST_INIT;
	endcase
	if (RESET)
		next_state = ST_INIT;
end

// FSM Behaviour
always @(posedge CLK)
begin
	case (next_state)
		ST_INIT: begin
			handA <= 3'b000;
			handB <= 3'b000;
			CARD <= 3'b000;
		end
		ST_FA:
			if (state != ST_FA) begin
				handA <= handA + randcard;
				CARD <= randcard;
			end

		ST_SB:
			if (state != ST_SB) begin
				handB <= handB + randcard;
				CARD <= randcard;
			end
					
		ST_FB:
			if (state != ST_FB) begin
				handB <= handB + randcard;
				CARD <= randcard;
			end

		ST_SA:
			if (state != ST_SA) begin
				handA <= handA + randcard;
				CARD <= randcard;
			end

		ST_A:
			if (state != ST_A) begin
				handA <= handA + randcard;
				CARD <= randcard;
			end

		ST_B:
			if (state != ST_B) begin
				handB <= handB + randcard;
				CARD <= randcard;
			end

		default:
			CARD <= 3'b000;
	endcase
	if (RESET) begin
		handA <= 3'b000;
		handB <= 3'b000;
		CARD <= 3'b000;
	end
end



// Output for SUM & WIN
always @(posedge CLK)
begin
	if (state == ST_OUTPUT && ~RESET)
		if ((handA > handB || handB > MAXH) && handA <= MAXH) begin
			WIN <= 2'b01;
			SUM <= handA;
		end
		else if ((handB > handA || handA > MAXH) && handB <= MAXH) begin
			WIN <= 2'b10;
			SUM <= handB;
		end
		else begin
			WIN <= 2'b11;
			SUM <= 4'd0;
		end
	else begin
		WIN <= 2'b00;
		SUM <= 4'd0;
	end
end
endmodule

module PGM(
	input CLK, RESET, IN_VALID, BUTTON,
	input [1:0] MORE,
	output reg OUT_VALID,
	output reg [1:0] WIN,
	output reg [3:0] CARD,
	output reg [3:0] SUM
);
parameter MAXH = 10;
parameter ST_INIT = 3'b000;
parameter ST_FC = 3'b001;
parameter ST_SC = 3'b010;
parameter ST_A = 3'b011;
parameter ST_B = 3'b100;
parameter ST_WAIT = 3'b101;
parameter ST_OUTPUT = 3'b110;
parameter ST_DONE = 3'b111;

reg [2:0] state, next_state;
reg [3:0] randcard;
integer m_w;
reg inIN, inBT;
reg [1:0] inMR;
reg [4:0] handA, handB;


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
		inIN <= 1'b0;
		inBT <= 1'b0;
		inMR <= 2'bxx;
	end
	else begin
		inIN <= IN_VALID;
		inBT <= BUTTON;
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

always @(state, handA, handB, MORE, inIN, inBT, inMR)
begin
	case (state)
		ST_INIT: if (inIN && inBT) next_state = ST_FC;
				else next_state = ST_INIT;
		ST_FC: next_state = ST_SC;
		ST_SC: if (handA > MAXH || handB > MAXH) next_state = ST_OUTPUT;
			else next_state = ST_WAIT;
		ST_A: if (handA > MAXH || handB > MAXH) next_state = ST_OUTPUT;
			else next_state = ST_WAIT;
		ST_B: if (handA > MAXH || handB > MAXH) next_state = ST_OUTPUT;
			else next_state = ST_WAIT;
		ST_WAIT: if (inIN && inBT)
				case (inMR)
					2'b00: next_state = ST_OUTPUT;
					2'b01: next_state = ST_A;
					2'b10: next_state = ST_B;
					2'b11: next_state = ST_FC;
				endcase
			else next_state = ST_WAIT;
		default: next_state = ST_DONE;
	endcase
end

// FSM Behaviour
always @(posedge CLK)
begin
	case (next_state)
		ST_INIT: begin
			handA <= 3'b000;
			handB <= 3'b000;
		end
		ST_FC: begin
			handA <= handA + randcard;
		end
		ST_SC: begin
			handB <= handB + randcard;
		end
		ST_A: begin
			handA <= handA + randcard;
		end
		ST_B: begin
			handB <= handB + randcard;
		end
	endcase
end



// Output for CARD & OUT_VALID
always @(posedge CLK)
begin
	case (next_state)
		ST_FC: begin
			OUT_VALID <= 1'b1;
			CARD <= randcard;
		end
		ST_SC: begin
			CARD <= randcard;
		end
		ST_A: begin
			OUT_VALID <= 1'b1;
			CARD <= randcard;
		end
		ST_B: begin
			OUT_VALID <= 1'b1;
			CARD <= randcard;
		end
		ST_OUTPUT: begin
			OUT_VALID <= 1'b1;
		end
		default: begin
			OUT_VALID <= 1'b0;
			CARD <= 3'b000;
		end
	endcase
end




// Output for SUM & WIN
always @(posedge CLK)
begin
	if (handA > handB && handA <= MAXH) begin
		WIN <= 2'b00;
		SUM <= handA;
	end
	else if (handB > handA && handB <= MAXH) begin
		WIN <= 2'b01;
		SUM <= handB;
	end
	else begin
		WIN <= 2'b10;
		SUM <= 4'd0;
	end

	if (RESET) begin
		WIN <= 2'b10;
		SUM <= 4'd0;
	end
end
endmodule

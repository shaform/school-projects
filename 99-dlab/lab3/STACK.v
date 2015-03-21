module STACK(
	input CLK, RESET, OP, IN_VALID,
	input [3:0] IN,
	output reg [5:0] OUT,
	output reg OUT_VALID
);

reg [3:0] stack[3:0];
integer top = -1;
integer i;
reg ng;
always @(posedge CLK)
begin
	if (RESET) reset();
	else begin
		if (IN_VALID) begin
			ng = 1;
			case (OP)
				1'b0:
					pop();
				1'b1:
					push();
			endcase
		end

		if (OUT_VALID) begin
			OUT = 0;
			OUT_VALID = 0;
		end else if (!IN_VALID && ng) begin
			ng = 0;
			getresult();
		end
	end
end


task pop;
	begin
		if (top > -1) begin
			stack[top] = 0;
			top = top - 1;
		end
	end
endtask

task push;
	begin
		if (top < 3) begin
			top = top + 1;
			stack[top] = IN;
		end
	end
endtask

task getresult;
	begin
		OUT = 0;
		if (top > -1) begin
			OUT_VALID = 1;
			OUT = stack[0] + stack[1] + stack[2] + stack[3];
		end else
			OUT_VALID = 0;
	end
endtask

task reset;
	begin
		ng = 0;
		top = -1;
		OUT_VALID = 0;
		OUT = 0;
		stack[0] = 0;
		stack[1] = 0;
		stack[2] = 0;
		stack[3] = 0;
	end
endtask

endmodule

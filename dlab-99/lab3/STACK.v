module STACK(
	input CLK, RESET, OP, IN_VALID,
	input [3:0] IN,
	output [5:0] OUT,
	output OUT_VALID
);

reg [3:0] stack[3:0];
reg OUT_VALID;
reg OUT;
integer top = -1;
integer i;
integer ng;

always @(posedge CLK)
begin
	if (RESET) reset();
	else if (IN_VALID) begin
		ng = 0;
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
	end else if (ng) begin
		ng = 0;
		getresult();
	end
end

always @(negedge IN_VALID)
begin
	ng = 1;
end

task pop;
	begin
		if (top > -1) top = top - 1;
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
			for (i=0; i<=top; i = i + 1)
				OUT = OUT + stack[i];
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
	end
endtask

endmodule

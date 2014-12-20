module mux(
	input i0, i1, i2, i3, i4, i5, i6, i7,
	input s0, s1, s2,
	output reg mo
);
always @(*)
begin
	case ({s2, s1, s0})
		3'b000: mo = i0;
		3'b001: mo = i1;
		3'b010: mo = i2;
		3'b011: mo = i3;
		3'b100: mo = i4;
		3'b101: mo = i5;
		3'b110: mo = i6;
		3'b111: mo = i7;
		default: mo = 1'bx;
	endcase
end
endmodule

module dff(
	input clock, reset,
	input d,
	output reg q
);
always @(posedge clock, posedge reset)
begin
	if (reset)
		q <= 0;
	else
		q <= d;
end
endmodule

module decoder(
	input i0, i1, i2,
	output o0, o1, o2, o3, o4, o5, o6, o7
);
assign o0 = {i2, i1, i0} == 3'b000;
assign o1 = {i2, i1, i0} == 3'b001;
assign o2 = {i2, i1, i0} == 3'b010;
assign o3 = {i2, i1, i0} == 3'b011;
assign o4 = {i2, i1, i0} == 3'b100;
assign o5 = {i2, i1, i0} == 3'b101;
assign o6 = {i2, i1, i0} == 3'b110;
assign o7 = {i2, i1, i0} == 3'b111;
endmodule

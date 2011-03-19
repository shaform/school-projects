`timescale 1ns / 1ps
module comparator_1b(
	input i_A, i_B,
	output o_more, o_less, o_equal
);
wire i_A_bar, i_B_bar;

not_1b not1(i_A, i_A_bar);
not_1b not2(i_B, i_B_bar);

and_1b and1(i_A, i_B_bar, o_more);
and_1b and2(i_A_bar, i_B, o_less);

nor_1b nor1(o_more, o_less, o_equal);


endmodule

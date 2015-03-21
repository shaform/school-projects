`timescale 1ns / 1ps
module comparator(
	input [3:0] i_A, i_B,
	output o_more, o_less, o_equal
);
wire [3:0] more, less, equal;
wire [2:0] eq_more, eq_less;
wire eq32, eq321;


comparator_1b cmp[3:0](i_A, i_B, more, less, equal);

// aux

and_1b and_1(equal[3], equal[2], eq32);
and_1b and_2(eq32, equal[1], eq321);

// more
and_1b and_m2(equal[3], more[2], eq_more[2]);
and_1b and_m1(eq32, more[1], eq_more[1]);
and_1b and_m0(eq321, more[0], eq_more[0]);

or_1b_4 or_more(more[3], eq_more[2], eq_more[1], eq_more[0], o_more);

// less
and_1b and_l2(equal[3], less[2], eq_less[2]);
and_1b and_l1(eq32, less[1], eq_less[1]);
and_1b and_l0(eq321, less[0], eq_less[0]);

or_1b_4 or_less(less[3], eq_less[2], eq_less[1], eq_less[0], o_less);

// equal
and_1b and_eq(eq321, equal[0], o_equal);

endmodule

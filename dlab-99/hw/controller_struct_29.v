module controller_struct_29(
	input clock, reset,
	input x, y, E, F
);
wire m2, m1, m0;
wire d2, d1, d0;
wire x_bar, E_bar, F_bar, w;
supply0 sp0;
supply1 sp1;

not (x_bar, x);
not (E_bar, E);
not (F_bar, F);
and (w, x_bar, y);


mux mux2(sp0, sp0, F_bar, sp0, sp1, sp0, sp1, sp0, d0, d1, d2, m2);
mux mux1(w, sp1, F, sp0, E_bar, sp0, sp1, sp0, d0, d1, d2, m1);
mux mux0(x, sp0, F, sp0, E, sp0, sp1, sp0, d0, d1, d2, m0);
dff df2(clock, reset, m2, d2);
dff df1(clock, reset, m1, d1);
dff df0(clock, reset, m0, d0);
decoder dec(d0, d1, d2, s0, s1, s2, s3, s4, s5, s6, s7);
endmodule

`timescale 1ns / 1ps
module word(
	output reg word,
	input [3:0] row, col,
	input [1:0] select
);
reg [15:0]line_a,
	line_b,
	line_c,
	line_d,
	line_e,
	line_f,
	line_g,
	line_h,

	line_i,
	line_j,
	line_k,
	line_l,
	line_m,
	line_n,
	line_o,
	line_p;                         

always @ (*)
begin
	if (select == 1)
	begin
		line_a<=16'b0000000000000000;
		line_b<=16'b0000000000000000;
		line_c<=16'b0000111111110000;
		line_d<=16'b0001111111111000;
		line_e<=16'b0011100000011100;
		line_f<=16'b0111000000001110;
		line_g<=16'b0111000000001110;
		line_h<=16'b0111000000001110;
		line_i<=16'b0111000000001110;
		line_j<=16'b0111000000001110;
		line_k<=16'b0111000000001110;
		line_l<=16'b0111000000001110;
		line_m<=16'b0111000000001110;
		line_n<=16'b0011111111111100;
		line_o<=16'b0001111111111000;
		line_p<=16'b0000000000000000;
	end 
	
	if (select == 2)
	begin
		line_a<=16'b0000000000000000;
		line_b<=16'b1110000000000111;
		line_c<=16'b0111000000001110;
		line_d<=16'b0011100000011100;
		line_e<=16'b0001110000111000;
		line_f<=16'b0000111001110000;
		line_g<=16'b0000011111100000;
		line_h<=16'b0000001111000000;
		line_i<=16'b0000001111000000;
		line_j<=16'b0000011111100000;
		line_k<=16'b0000111001110000;
		line_l<=16'b0001110000111000;
		line_m<=16'b0011100000011100;
		line_n<=16'b0111000000001110;
		line_o<=16'b1110000000000111;
		line_p<=16'b0000000000000000;
	end
	
	if (select == 0)
	begin 
		word <= 1'b0;
	end else begin
		case   ({row,col})
		8'b0000_0000: 	word <= line_a[15];
		8'b0000_0001: 	word <= line_a[14];
		8'b0000_0010: 	word <= line_a[13];
		8'b0000_0011: 	word <= line_a[12];
		8'b0000_0100: 	word <= line_a[11];
		8'b0000_0101: 	word <= line_a[10];
		8'b0000_0110: 	word <= line_a[9];
		8'b0000_0111: 	word <= line_a[8];
		8'b0000_1000: 	word <= line_a[7];
		8'b0000_1001: 	word <= line_a[6];
		8'b0000_1010: 	word <= line_a[5];
		8'b0000_1011: 	word <= line_a[4];
		8'b0000_1100: 	word <= line_a[3];
		8'b0000_1101: 	word <= line_a[2];
		8'b0000_1110: 	word <= line_a[1];
		8'b0000_1111: 	word <= line_a[0];

		8'b0001_0000: 	word <= line_b[15];
		8'b0001_0001: 	word <= line_b[14];
		8'b0001_0010: 	word <= line_b[13];
		8'b0001_0011: 	word <= line_b[12];
		8'b0001_0100: 	word <= line_b[11];
		8'b0001_0101: 	word <= line_b[10];
		8'b0001_0110: 	word <= line_b[9];
		8'b0001_0111: 	word <= line_b[8];
		8'b0001_1000: 	word <= line_b[7];
		8'b0001_1001: 	word <= line_b[6];
		8'b0001_1010: 	word <= line_b[5];
		8'b0001_1011: 	word <= line_b[4];
		8'b0001_1100: 	word <= line_b[3];
		8'b0001_1101: 	word <= line_b[2];
		8'b0001_1110: 	word <= line_b[1];
		8'b0001_1111: 	word <= line_b[0];	

		8'b0010_0000: 	word <= line_c[15];
		8'b0010_0001: 	word <= line_c[14];
		8'b0010_0010: 	word <= line_c[13];
		8'b0010_0011: 	word <= line_c[12];
		8'b0010_0100: 	word <= line_c[11];
		8'b0010_0101: 	word <= line_c[10];
		8'b0010_0110: 	word <= line_c[9];
		8'b0010_0111: 	word <= line_c[8];
		8'b0010_1000: 	word <= line_c[7];
		8'b0010_1001: 	word <= line_c[6];
		8'b0010_1010: 	word <= line_c[5];
		8'b0010_1011: 	word <= line_c[4];
		8'b0010_1100: 	word <= line_c[3];
		8'b0010_1101: 	word <= line_c[2];
		8'b0010_1110: 	word <= line_c[1];
		8'b0010_1111: 	word <= line_c[0];

		8'b0011_0000: 	word <= line_d[15];
		8'b0011_0001: 	word <= line_d[14];
		8'b0011_0010: 	word <= line_d[13];
		8'b0011_0011: 	word <= line_d[12];
		8'b0011_0100: 	word <= line_d[11];
		8'b0011_0101: 	word <= line_d[10];
		8'b0011_0110: 	word <= line_d[9];
		8'b0011_0111: 	word <= line_d[8];
		8'b0011_1000: 	word <= line_d[7];
		8'b0011_1001: 	word <= line_d[6];
		8'b0011_1010: 	word <= line_d[5];
		8'b0011_1011: 	word <= line_d[4];
		8'b0011_1100: 	word <= line_d[3];
		8'b0011_1101: 	word <= line_d[2];
		8'b0011_1110: 	word <= line_d[1];
		8'b0011_1111: 	word <= line_d[0];

		8'b0100_0000: 	word <= line_e[15];
		8'b0100_0001: 	word <= line_e[14];
		8'b0100_0010: 	word <= line_e[13];
		8'b0100_0011: 	word <= line_e[12];
		8'b0100_0100: 	word <= line_e[11];
		8'b0100_0101: 	word <= line_e[10];
		8'b0100_0110: 	word <= line_e[9];
		8'b0100_0111: 	word <= line_e[8];
		8'b0100_1000: 	word <= line_e[7];
		8'b0100_1001: 	word <= line_e[6];
		8'b0100_1010: 	word <= line_e[5];
		8'b0100_1011: 	word <= line_e[4];
		8'b0100_1100: 	word <= line_e[3];
		8'b0100_1101: 	word <= line_e[2];
		8'b0100_1110: 	word <= line_e[1];
		8'b0100_1111: 	word <= line_e[0];

		8'b0101_0000: 	word <= line_f[15];
		8'b0101_0001: 	word <= line_f[14];
		8'b0101_0010: 	word <= line_f[13];
		8'b0101_0011: 	word <= line_f[12];
		8'b0101_0100: 	word <= line_f[11];
		8'b0101_0101: 	word <= line_f[10];
		8'b0101_0110: 	word <= line_f[9];
		8'b0101_0111: 	word <= line_f[8];
		8'b0101_1000: 	word <= line_f[7];
		8'b0101_1001: 	word <= line_f[6];
		8'b0101_1010: 	word <= line_f[5];
		8'b0101_1011: 	word <= line_f[4];
		8'b0101_1100: 	word <= line_f[3];
		8'b0101_1101: 	word <= line_f[2];
		8'b0101_1110: 	word <= line_f[1];
		8'b0101_1111: 	word <= line_f[0];

		8'b0110_0000: 	word <= line_g[15];
		8'b0110_0001: 	word <= line_g[14];
		8'b0110_0010: 	word <= line_g[13];
		8'b0110_0011: 	word <= line_g[12];
		8'b0110_0100: 	word <= line_g[11];
		8'b0110_0101: 	word <= line_g[10];
		8'b0110_0110: 	word <= line_g[9];
		8'b0110_0111: 	word <= line_g[8];
		8'b0110_1000: 	word <= line_g[7];
		8'b0110_1001: 	word <= line_g[6];
		8'b0110_1010: 	word <= line_g[5];
		8'b0110_1011: 	word <= line_g[4];
		8'b0110_1100: 	word <= line_g[3];
		8'b0110_1101: 	word <= line_g[2];
		8'b0110_1110: 	word <= line_g[1];
		8'b0110_1111: 	word <= line_g[0];

		8'b0111_0000: 	word <= line_h[15];
		8'b0111_0001: 	word <= line_h[14];
		8'b0111_0010: 	word <= line_h[13];
		8'b0111_0011: 	word <= line_h[12];
		8'b0111_0100: 	word <= line_h[11];
		8'b0111_0101: 	word <= line_h[10];
		8'b0111_0110: 	word <= line_h[9];
		8'b0111_0111: 	word <= line_h[8];
		8'b0111_1000: 	word <= line_h[7];
		8'b0111_1001: 	word <= line_h[6];
		8'b0111_1010: 	word <= line_h[5];
		8'b0111_1011: 	word <= line_h[4];
		8'b0111_1100: 	word <= line_h[3];
		8'b0111_1101: 	word <= line_h[2];
		8'b0111_1110: 	word <= line_h[1];
		8'b0111_1111: 	word <= line_h[0];

	 
		8'b1000_0000: 	word <= line_i[15];
		8'b1000_0001: 	word <= line_i[14];
		8'b1000_0010: 	word <= line_i[13];
		8'b1000_0011: 	word <= line_i[12];
		8'b1000_0100: 	word <= line_i[11];
		8'b1000_0101: 	word <= line_i[10];
		8'b1000_0110: 	word <= line_i[9];
		8'b1000_0111: 	word <= line_i[8];
		8'b1000_1000: 	word <= line_i[7];
		8'b1000_1001: 	word <= line_i[6];
		8'b1000_1010: 	word <= line_i[5];
		8'b1000_1011: 	word <= line_i[4];
		8'b1000_1100: 	word <= line_i[3];
		8'b1000_1101: 	word <= line_i[2];
		8'b1000_1110: 	word <= line_i[1];
		8'b1000_1111: 	word <= line_i[0];

		8'b1001_0000: 	word <= line_j[15];
		8'b1001_0001: 	word <= line_j[14];
		8'b1001_0010: 	word <= line_j[13];
		8'b1001_0011: 	word <= line_j[12];
		8'b1001_0100: 	word <= line_j[11];
		8'b1001_0101: 	word <= line_j[10];
		8'b1001_0110: 	word <= line_j[9];
		8'b1001_0111: 	word <= line_j[8];
		8'b1001_1000: 	word <= line_j[7];
		8'b1001_1001: 	word <= line_j[6];
		8'b1001_1010: 	word <= line_j[5];
		8'b1001_1011: 	word <= line_j[4];
		8'b1001_1100: 	word <= line_j[3];
		8'b1001_1101: 	word <= line_j[2];
		8'b1001_1110: 	word <= line_j[1];
		8'b1001_1111: 	word <= line_j[0];	

		8'b1010_0000: 	word <= line_k[15];
		8'b1010_0001: 	word <= line_k[14];
		8'b1010_0010: 	word <= line_k[13];
		8'b1010_0011: 	word <= line_k[12];
		8'b1010_0100: 	word <= line_k[11];
		8'b1010_0101: 	word <= line_k[10];
		8'b1010_0110: 	word <= line_k[9];
		8'b1010_0111: 	word <= line_k[8];
		8'b1010_1000: 	word <= line_k[7];
		8'b1010_1001: 	word <= line_k[6];
		8'b1010_1010: 	word <= line_k[5];
		8'b1010_1011: 	word <= line_k[4];
		8'b1010_1100: 	word <= line_k[3];
		8'b1010_1101: 	word <= line_k[2];
		8'b1010_1110: 	word <= line_k[1];
		8'b1010_1111: 	word <= line_k[0];

		8'b1011_0000: 	word <= line_l[15];
		8'b1011_0001: 	word <= line_l[14];
		8'b1011_0010: 	word <= line_l[13];
		8'b1011_0011: 	word <= line_l[12];
		8'b1011_0100: 	word <= line_l[11];
		8'b1011_0101: 	word <= line_l[10];
		8'b1011_0110: 	word <= line_l[9];
		8'b1011_0111: 	word <= line_l[8];
		8'b1011_1000: 	word <= line_l[7];
		8'b1011_1001: 	word <= line_l[6];
		8'b1011_1010: 	word <= line_l[5];
		8'b1011_1011: 	word <= line_l[4];
		8'b1011_1100: 	word <= line_l[3];
		8'b1011_1101: 	word <= line_l[2];
		8'b1011_1110: 	word <= line_l[1];
		8'b1011_1111: 	word <= line_l[0];

		8'b1100_0000: 	word <= line_m[15];
		8'b1100_0001: 	word <= line_m[14];
		8'b1100_0010: 	word <= line_m[13];
		8'b1100_0011: 	word <= line_m[12];
		8'b1100_0100: 	word <= line_m[11];
		8'b1100_0101: 	word <= line_m[10];
		8'b1100_0110: 	word <= line_m[9];
		8'b1100_0111: 	word <= line_m[8];
		8'b1100_1000: 	word <= line_m[7];
		8'b1100_1001: 	word <= line_m[6];
		8'b1100_1010: 	word <= line_m[5];
		8'b1100_1011: 	word <= line_m[4];
		8'b1100_1100: 	word <= line_m[3];
		8'b1100_1101: 	word <= line_m[2];
		8'b1100_1110: 	word <= line_m[1];
		8'b1100_1111: 	word <= line_m[0];

		8'b1101_0000: 	word <= line_n[15];
		8'b1101_0001: 	word <= line_n[14];
		8'b1101_0010: 	word <= line_n[13];
		8'b1101_0011: 	word <= line_n[12];
		8'b1101_0100: 	word <= line_n[11];
		8'b1101_0101: 	word <= line_n[10];
		8'b1101_0110: 	word <= line_n[9];
		8'b1101_0111: 	word <= line_n[8];
		8'b1101_1000: 	word <= line_n[7];
		8'b1101_1001: 	word <= line_n[6];
		8'b1101_1010: 	word <= line_n[5];
		8'b1101_1011: 	word <= line_n[4];
		8'b1101_1100: 	word <= line_n[3];
		8'b1101_1101: 	word <= line_n[2];
		8'b1101_1110: 	word <= line_n[1];
		8'b1101_1111: 	word <= line_n[0];

		8'b1110_0000: 	word <= line_o[15];
		8'b1110_0001: 	word <= line_o[14];
		8'b1110_0010: 	word <= line_o[13];
		8'b1110_0011: 	word <= line_o[12];
		8'b1110_0100: 	word <= line_o[11];
		8'b1110_0101: 	word <= line_o[10];
		8'b1110_0110: 	word <= line_o[9];
		8'b1110_0111: 	word <= line_o[8];
		8'b1110_1000: 	word <= line_o[7];
		8'b1110_1001: 	word <= line_o[6];
		8'b1110_1010: 	word <= line_o[5];
		8'b1110_1011: 	word <= line_o[4];
		8'b1110_1100: 	word <= line_o[3];
		8'b1110_1101: 	word <= line_o[2];
		8'b1110_1110: 	word <= line_o[1];
		8'b1110_1111: 	word <= line_o[0];

		8'b1111_0000: 	word <= line_p[15];
		8'b1111_0001: 	word <= line_p[14];
		8'b1111_0010: 	word <= line_p[13];
		8'b1111_0011: 	word <= line_p[12];
		8'b1111_0100: 	word <= line_p[11];
		8'b1111_0101: 	word <= line_p[10];
		8'b1111_0110: 	word <= line_p[9];
		8'b1111_0111: 	word <= line_p[8];
		8'b1111_1000: 	word <= line_p[7];
		8'b1111_1001: 	word <= line_p[6];
		8'b1111_1010: 	word <= line_p[5];
		8'b1111_1011: 	word <= line_p[4];
		8'b1111_1100: 	word <= line_p[3];
		8'b1111_1101: 	word <= line_p[2];
		8'b1111_1110: 	word <= line_p[1];
		8'b1111_1111: 	word <= line_p[0];
		endcase
	end
end

endmodule

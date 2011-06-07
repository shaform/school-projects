module stage_rom(
	input clock, enable,
	input [4:0] addr,
	input [1:0] stage,
	output reg [29:0] data
);

always @(posedge clock)
begin 
if (enable) 
	case (stage)
		2'b01: case(addr)
				5'b00000: data <= 30'b001_011_101_001_101_001_101_001_000_000;
				5'b00001: data <= 30'b001_001_001_001_001_001_001_001_001_001;
				5'b00010: data <= 30'b111_001_001_001_001_001_001_001_001_001;
				5'b00011: data <= 30'b001_001_001_001_001_001_001_001_001_001;
				5'b00100: data <= 30'b010_001_001_001_001_001_001_001_001_001;
				5'b00101: data <= 30'b101_001_001_001_001_001_001_001_001_001;
				5'b00110: data <= 30'b110_001_001_001_001_001_001_001_101_001;
				5'b00111: data <= 30'b000_001_001_001_001_001_001_001_001_001;
				5'b01000: data <= 30'b101_001_001_001_001_001_001_101_001_001;
				5'b01001: data <= 30'b001_001_001_001_001_001_001_001_001_001;
				5'b01010: data <= 30'b111_001_001_001_001_001_001_001_001_001;
				5'b01011: data <= 30'b001_001_001_001_101_001_001_001_001_001;
				5'b01100: data <= 30'b010_001_011_011_101_001_001_101_001_001;
				5'b01101: data <= 30'b101_011_001_001_001_001_001_001_001_001;
				5'b01110: data <= 30'b110_001_001_001_001_001_001_001_001_001;
				5'b01111: data <= 30'b000_101_001_001_001_001_001_001_001_001;
				5'b10000: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b10001: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b10010: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b10011: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b10100: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b10101: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b10110: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b10111: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b11000: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b11001: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b11010: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b11011: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b11100: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b11101: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				default:  data <= 30'bxxx_xxx_xxx_xxx_xxx_xxx_xxx_xxx_xxx_xxx;
			endcase
		2'b10: case(addr)
				5'b00000: data <= 30'b111_011_101_001_111_001_101_001_000_000;
				5'b00001: data <= 30'b111_001_001_001_111_001_001_001_001_001;
				5'b00010: data <= 30'b111_001_001_001_111_001_001_001_001_001;
				5'b00011: data <= 30'b111_001_001_001_111_001_001_001_001_001;
				5'b00100: data <= 30'b111_001_001_001_111_001_001_001_001_001;
				5'b00101: data <= 30'b111_001_001_001_111_001_001_001_001_001;
				5'b00110: data <= 30'b111_001_001_001_111_001_001_001_101_001;
				5'b00111: data <= 30'b111_001_001_001_111_001_001_001_001_001;
				5'b01000: data <= 30'b111_001_001_001_111_001_001_101_001_001;
				5'b01001: data <= 30'b111_001_001_001_111_001_001_001_001_001;
				5'b01010: data <= 30'b111_001_001_001_111_001_001_001_001_001;
				5'b01011: data <= 30'b111_001_001_001_111_001_001_001_001_001;
				5'b01100: data <= 30'b111_001_011_011_111_001_001_101_001_001;
				5'b01101: data <= 30'b111_011_001_001_111_001_001_001_001_001;
				5'b01110: data <= 30'b111_001_001_001_111_001_001_001_001_001;
				5'b01111: data <= 30'b000_101_001_001_001_001_001_001_001_001;
				5'b10000: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b10001: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b10010: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b10011: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b10100: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b10101: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b10110: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b10111: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b11000: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b11001: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b11010: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b11011: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b11100: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b11101: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				default:  data <= 30'bxxx_xxx_xxx_xxx_xxx_xxx_xxx_xxx_xxx_xxx;
			endcase
		2'b11: case(addr)
				5'b00000: data <= 30'b000_000_000_110_110_110_110_000_000_000;
				5'b00001: data <= 30'b000_000_110_110_110_110_110_110_000_000;
				5'b00010: data <= 30'b000_110_110_110_110_110_110_110_110_000;
				5'b00011: data <= 30'b110_110_110_110_110_110_110_110_110_000;
				5'b00100: data <= 30'b110_110_000_110_110_110_000_110_110_110;
				5'b00101: data <= 30'b110_110_000_110_110_110_000_110_110_110;
				5'b00110: data <= 30'b110_110_000_110_110_110_000_110_110_110;
				5'b00111: data <= 30'b110_000_000_000_110_000_000_000_110_110;
				5'b01000: data <= 30'b110_000_000_000_110_000_000_000_110_110;
				5'b01001: data <= 30'b110_110_000_110_110_110_000_110_110_110;
				5'b01010: data <= 30'b110_110_000_110_110_110_000_110_110_110;
				5'b01011: data <= 30'b110_110_000_110_110_110_000_110_110_110;
				5'b01100: data <= 30'b110_110_110_110_110_110_110_110_110_110;
				5'b01101: data <= 30'b110_110_110_110_110_110_110_110_110_110;
				5'b01110: data <= 30'b110_101_101_101_101_101_101_101_101_110;
				5'b01111: data <= 30'b110_110_101_101_101_101_101_101_110_110;
				5'b10000: data <= 30'b110_110_110_101_101_101_101_110_110_110;
				5'b10001: data <= 30'b110_110_110_110_110_110_110_110_110_110;
				5'b10010: data <= 30'b000_110_110_110_110_110_110_110_110_000;
				5'b10011: data <= 30'b000_000_110_110_110_110_110_110_000_000;
				5'b10100: data <= 30'b000_000_000_110_110_110_110_000_000_000;
				5'b10101: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b10110: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b10111: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b11000: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b11001: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b11010: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b11011: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b11100: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				5'b11101: data <= 30'b000_000_000_000_000_000_000_000_000_000;
				default:  data <= 30'bxxx_xxx_xxx_xxx_xxx_xxx_xxx_xxx_xxx_xxx;
		       endcase
		default: data <= 30'bxxx_xxx_xxx_xxx_xxx_xxx_xxx_xxx_xxx_xxx;
	endcase
end 

endmodule

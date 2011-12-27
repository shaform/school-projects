#include "io.h"

bit A_release, B_release;
bit A_enable, B_enable;


uchar get_input_A(void)
{
	uchar t = 0;
	if (A_release) {
		if (A1 == 0) { t = BTNA; }
		if (A2 == 0) { t = BTNB; }
		if (A3 == 0) { t = BTNC; }
		if (A4 == 0) { t = BTND; }
		A_release = 0;
	}

	if (!A_enable) t = 0;

	return t;
}

uchar get_input_B(void)
{
	uchar t = 0;
	if (B_release) {
		if (B1 == 0) { t = BTNA; }
		if (B2 == 0) { t = BTNB; }
		if (B3 == 0) { t = BTNC; }
		if (B4 == 0) { t = BTND; }
		B_release = 0;
	}

	if (!B_enable) t = 0;

	return t;
}

#ifndef IO_H
#define IO_H

uchar get_input_A(void);
uchar get_input_B(void);
// ---------------------------------------------------- //
sbit A1 = P0^0;
sbit A2 = P0^1;
sbit A3 = P0^2;
sbit A4 = P0^3;
sbit B1 = P0^4;
sbit B2 = P0^5;
sbit B3 = P0^6;
sbit B4 = P0^7;

/* P3.0 RX0, P3.1 TX0 */
/* P3.2 BUZZER */

// Buttons

#define BTNA 1
#define BTNB 2
#define BTNC 3
#define BTND 4

extern bit A_release, B_release;
extern bit A_enable, B_enable;

#endif

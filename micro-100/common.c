#include <regx51.h>
#include "common.h"

uchar ctl, cth;

/******************************
 *
 * Time delay subroutine
 *
 ******************************/
/*
void delay(int ms)
{
	int i;
	while (ms--)
		for (i = 0; i< 1000; i++);
}
*/

void delay_us(int us)
{
	while (us--);
}

void timer_init(void)
{
	ctl = 0xB0;
	cth = 0x3C;
	timer_restore();
}
void timer_restore(void)
{
	TR1 = 0;  // Pause timer
	TMOD &= 0xCF;  // Clear timer1 mode
	TMOD |= 0x10;  // Set timer1 to 16-bit timer
	TH1 = cth;
	TL1 = ctl;
	ET1 = 1;  // Enable timer1 interrupt
	EA = 1;  // Enable interrupt
	TR1 = 1;  // Start timer1
}
void timer_stop(void)
{
	TR1 = 0;
	ET1 = 0;
	cth = TH1;
	ctl = TL1;
}

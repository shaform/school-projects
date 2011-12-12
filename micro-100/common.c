#include "common.h"

/******************************
 *
 * Time delay subroutine
 *
 ******************************/
void delay(int ms)
{
	int i;
	while (ms--)
		for (i = 0; i< 1000; i++);
}

void delay_us(int us)
{
	while (us--);
}

void delay_serial(void)
{

			;
}


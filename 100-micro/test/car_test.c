#include <reg52.h>
#include "common.h"


sbit Afront = P1^0;
sbit Aback = P1^2;

/* car control */
sbit carA_1 = P0^0;
sbit carA_2 = P0^1;
uchar carA_t = 0;

void carAstop()
{
	carA_1 = 1;
	carA_2 = 1;
}
void carAgo()
{
	carA_1 = 1;
	carA_2 = 0;
	carA_t = 5;
}
void carAback()
{
	carA_1 = 0;
	carA_2 = 1;
	carA_t = 5;
}

sbit RESTART = P0^2;
sbit RESTART2 = P0^3;

// Control signal

bit car_adj = 0;  // Whether to adjust car.

void timer1_int(void) interrupt 3
{
	if (carA_t) {
		if (--carA_t == 4)
			carAstop();
	}
}


void init(void)
{
	carAstop();
	carA_t = 0;
	car_adj = 0;

	timer_init();
}

void main()
{
	init();
	while (1) {   	
		if (carA_t) {
			;  // do nothing
		} else if (car_adj) {
			if (Aback) carAback();
			else car_adj = 0;
		} else if (RESTART == 0) {
			while (RESTART == 0);
			car_adj = 1;
			carA_1 = 0;   
		} else if (RESTART2 == 0) {
			while (RESTART2 == 0);
			carAgo();		  
		}	 		  
	}
}


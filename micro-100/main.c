#include <reg52.h>
#include "common.h"
#include "display.h"
#include "questions.h"

#define INIT_SCORE 128
#define LOSE_SCORE 118
#define WIN_SCORE  138

/* A1-A4:P0.0-P0.3, B1-B4:P0.4-P0.7 */
sbit A1 = P0^0;
sbit A2 = P0^1;
sbit A3 = P0^2;
sbit A4 = P0^3;
sbit B1 = P0^4;
sbit B2 = P0^5;
sbit B3 = P0^6;
sbit B4 = P0^7;
/* P3.0 RX0, P3.1 TX0 */
/* P3.2 RESTART */
sbit RESTART = P3^3;

uchar wait_time;
bit wait = 0;
bit first = 1;
bit A_release, B_release;

uchar A_scores = INIT_SCORE;
uchar B_scores = INIT_SCORE;

void timer0_init(void);
uchar userA_answer();
uchar userB_answer();
void wait_answer();
void init(void);
void release_routine(void);

void timer0Int(void) interrupt 1
{
	if (--wait_time == 0) {
		wait = 0;              // next question
	}
}

void main()
{
	init();
	while (1) {   
		if (first) {
			if (RESTART == 0) {
				while (RESTART == 0);
				first = 0;
			}
		} else if (wait == 0) {
			wait = 1;
			/* get a question */
			question_next();
			question_display();

			/* reset timer0 */
			wait_time = 20*3;
		} else if(wait == 1) {
			wait_answer();  
		}
		display_routine();
		release_routine();
	}
}

void init(void)
{
	display_init();
	question_init();
	timer0_init();
	wait = 0;
	first = 1;  // First time play
	A_release = 1;
	B_release = 1;
}


void timer0_init(void)
{
	wait_time = 20*10;
	TMOD &= 0xFC;
	TMOD |= 0x01;
	TH0 = 0x3C;
	TL0 = 0xB0;
	ET0 = 1;  // Enable timer0 interrupt
	EA = 1;  // Enable interrupt
	TR0 = 1;  // Enable timer0
}

uchar userA_answer()
{
	uchar t = 0;
	if (A_release) {
		if (A1 == 0) { t = 1; }
		if (A2 == 0) { t = 2; }
		if (A3 == 0) { t = 3; }
		if (A4 == 0) { t = 4; }
		A_release = 0;
	}

	return t;
}

uchar userB_answer()
{
	uchar t = 0;
	if (B_release) {
		if (B1 == 0) { t = 1; }
		if (B2 == 0) { t = 2; }
		if (B3 == 0) { t = 3; }
		if (B4 == 0) { t = 4; }
		B_release = 0;
	}

	return t;
}
void release_routine(void)
{
	uchar t = P0;
	P1 = t;
	delay_us(40);
	if ((t & 0x0f) == 0x0f)
		A_release = 1;
	if ((t & 0xf0) == 0xf0)
		B_release = 1;
}

void wait_answer()
{
	uchar userA, userB;
	userA = userA_answer();
	userB = userB_answer();
	if (userA != 0){
		if (question_get_answer() == userA) {
			A_scores++;
			if (A_scores == WIN_SCORE) {
				/* Display LED and alarm */
			}
			//wait = 0;
			display_string("A wins!!");
			display_flush();
		} else {  /* wrong */
			A_scores = ((A_scores-1 <= LOSE_SCORE)?LOSE_SCORE:(A_scores-1));
			display_string("A lose!!");
			display_flush();
		}   
	} else {
	}

	if (userB != 0){
		if (question_get_answer() == userB) {
			B_scores++;
			if (B_scores == WIN_SCORE){
				/* Display LED and alarm */
			}
			display_string("B wins!!");
			display_flush();
			//wait = 0;
		} else {   /* wrong */
			B_scores = ((B_scores-1 <= LOSE_SCORE)?LOSE_SCORE:(B_scores-1));
			display_string("B lose!!");
			display_flush();
		}
	}
}


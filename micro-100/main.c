#include <reg52.h>
#include "common.h"
#include "display.h"
#include "questions.h"
#include "buzzer.h"

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

static uchar wait_time, next_time, buzzer_time;
uchar temp_m;
bit buzzer_next = 0;
bit buzzer_pause = 0;
bit next_q = 0;  // Whether the next question is coming.
bit wait = 0;  // Whether waiting for user to guess?
bit stay = 0;  // Whether to stay and do nothing.
bit first = 1;
bit A_release, B_release;
bit A_enable, B_enable;

uchar A_scores = INIT_SCORE;
uchar B_scores = INIT_SCORE;

uchar userA_answer();
uchar userB_answer();
void wait_answer();
void init(void);
void release_routine(void);
void buzzer_routine(void);


void timer1_int(void) interrupt 3
{
	//buzzer_step();
	if (--wait_time == 0) {
		next_q = 1;  // next question
	}
	if (--next_time == 0) {
		stay = 0;
	}
	if (--buzzer_time == 0) {
		buzzer_next = 1;
	}
}

void main()
{
	init();
	display_clear();
	display_string("Guess game!!\r\n"
			"Are you ready?");
	display_stop();

	buzzer_play_num(1);
	while (1) {   
		if (stay) {
			;  // do nothing
		} else if (first) {
			if (RESTART == 0) {
				while (RESTART == 0);
				first = 0;
				next_q = 1;
			}
		} else if (next_q) {
			buzzer_play(0);
			next_q = 0;
			wait = 0;
			stay = 1;
			next_time = TIME_SEC*1;
			display_clear();
			display_string("Next question!!\r\n"
					"Ready?");
			display_stop();
		} else if (wait) {
			wait_answer();  
		} else {
			/* get a question */
			question_next();
			question_display();
			temp_m = question_get_music();
			if (temp_m) {
				buzzer_play_num(temp_m);
			}

			/* reset timer1 */
			wait_time = TIME_SEC*2;
			wait = 1;
			A_enable = B_enable = 1;
		}
		release_routine();
		buzzer_routine();
	}
}

void init(void)
{
	timer_init();
	display_init();
	question_init();
	buzzer_init();
	stay = 0;
	wait = 0;
	next_q = 0;
	buzzer_next = 0;
	buzzer_time = TIME_SEC/8;
	buzzer_pause = 0;

	first = 1;  // First time play

	A_release = 1;
	B_release = 1;

	A_enable = 1;
	B_enable = 1;
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

	if (!A_enable) t = 0;

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

	if (!B_enable) t = 0;

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


void buzzer_routine(void)
{
	if (buzzer_next) {
		if (buzzer_pause) {
			buzzer_pause = 0;
			TR0 = 1;
			buzzer_time = TIME_SEC/8;
		} else if (buzzer_step()) {
			TR0 = 0;
			buzzer_pause = 1;
			buzzer_time = 1;
		} else {
			buzzer_time = TIME_SEC/8;
		}
		buzzer_next = 0;
	}
}
void wait_answer()
{
	uchar userA, userB;
	userA = userA_answer();
	userB = userB_answer();
	if (userA != 0){
		A_enable = 0;
		if (question_get_answer() == userA) {
			B_enable = 0;
			A_scores++;
			if (A_scores == WIN_SCORE) {
				/* Display LED and alarm */
			}
			//wait = 0;
			display_string("A wins!!");
			display_stop();
		} else {  /* wrong */
			A_scores = ((A_scores-1 <= LOSE_SCORE)?LOSE_SCORE:(A_scores-1));
			display_string("A lose!!");
			display_stop();
		}
	}

	if (userB != 0){
		B_enable = 0;
		if (question_get_answer() == userB) {
			A_enable = 0;
			B_scores++;
			if (B_scores == WIN_SCORE){
				/* Display LED and alarm */
			}
			display_string("B wins!!");
			display_stop();
			//wait = 0;
		} else {   /* wrong */
			B_scores = ((B_scores-1 <= LOSE_SCORE)?LOSE_SCORE:(B_scores-1));
			display_string("B lose!!");
			display_stop();
		}
	}
}


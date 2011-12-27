#include <reg52.h>
#include "common.h"
#include "display.h"
#include "questions.h"
#include "buzzer.h"

/* A1-A4:P0.0-P0.3, B1-B4:P0.4-P0.7 */
sbit A1 = P0^0;
sbit A2 = P0^1;
sbit A3 = P0^2;
sbit A4 = P0^3;
sbit B1 = P0^4;
sbit B2 = P0^5;
sbit B3 = P0^6;
sbit B4 = P0^7;

sbit Afront = P1^0;
sbit Bfront = P1^1;
sbit Aback = P1^2;
sbit Bback = P1^3;

/* car control */
sbit carA_1 = P1^4;
sbit carA_2 = P1^5;
sbit carB_1 = P1^6;
sbit carB_2 = P1^7;
uchar carA_t = 0, carB_t = 0, car_num;


void carAstop()
{
	carA_1 = 1;
	carA_2 = 1;
}
void carBstop()
{
	carB_1 = 1;
	carB_2 = 1;
}
void carAgo()
{
	carA_1 = 1;
	carA_2 = 0;
	carA_t = 2;
}
void carBgo()
{
	carA_1 = 1;
	carA_2 = 0;
	carA_t = 2;
}
void carAback()
{
	carA_1 = 0;
	carA_2 = 1;
	carB_t = 2;
}
void carBback()
{
	carA_1 = 0;
	carA_2 = 1;
	carB_t = 2;
}

/* P3.0 RX0, P3.1 TX0 */
/* P3.2 BUZZER */
/* P3.3 RESTART */
sbit RESTART = P3^3;


static uchar wait_time, next_time, buzzer_time;
uchar temp_m;
bit buzzer_next = 0;
bit buzzer_pause = 0;

// Control signal

bit stay = 0;  // Whether to stay and do nothing.
bit car_adj = 0;  // Whether to adjust car.
bit next_q = 0;  // Next question stay.
bit disp_q = 0;  // Display question.
bit car_ah = 0;  // Car go front.
bit wait_a = 0;   // Wait for input
bit first = 1;

bit A_release, B_release;
bit A_enable, B_enable;


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
		wait_a = 0;  // next question
	}
	if (--next_time == 0) {
		stay = 0;
	}
	if (--buzzer_time == 0) {
		buzzer_next = 1;
	}
	if (carA_t) {
		if (--carA_t == 1)
			carAstop();
	}
	if (carB_t) {
		if (--carB_t == 1)
			carBstop();
	}
}
void stay_for(uchar t)
{
	stay = 1;
	next_time = TIME_SEC*1;
}

void main()
{
	init();
	display_clear();
	display_string("Guess game!!\r\n"
			"Are you ready?");
	display_stop();

	while (1) {   
		if (stay || carA_t || carB_t) {
			;  // do nothing
		} else if (car_adj) {
			if (!Aback) carAback();
			if (!Bback) carBback();
			if (Aback && Bback)
				car_adj = 0;
		} else if (first) {
			if (Aback && Bback) {
				if (RESTART == 0) {
					while (RESTART == 0);
					first = 0;
					next_q = 1;
				}
			} else {
				car_adj = 1;
			}
		} else if (next_q) {
			buzzer_play(0);  // Stop music
			next_q = 0;
			disp_q = 1;
			display_clear();
			display_string("Next question!!\r\n"
					"Ready?");
			display_stop();
			stay_for(TIME_SEC*1);
		} else if (disp_q) {
			disp_q = 0;
			wait_a = 1;
			/* get a question */
			question_next();
			question_display();
			temp_m = question_get_music();
			if (temp_m) {
				buzzer_play_num(temp_m);
			}

			/* reset timer1 */
			wait_time = TIME_SEC*2;
			A_enable = B_enable = 1;
		} else if (wait_a) {
			wait_answer();  
		} else if (car_ah) {
			car_ah = 0;
			display_clear();
			if (car_num == 0)
				display_string("Player1 is right!\r\n");
			else if (car_num == 1)
				display_string("Player1 is wrong!\r\n");
			else if (car_num == 2)
				display_string("Player2 is right!\r\n");
			else
				display_string("Player2 is wrong!\r\n");
			display_stop();

			if (car_num == 0 || car_num == 3)
				carAgo();
			else
				carBgo();
		} else {
			next_q = 1;
		}
		release_routine();
		buzzer_routine();
	}
}

void init(void)
{
	carAstop();
	carBstop();

	timer_init();
	display_init();
	question_init();
	buzzer_init();

	first = 1;  // First time play
	stay = 0;
	next_q = 0;
	car_adj = 0;
	wait_a = 0;
	disp_q = 0;
	car_ah = 0;


	buzzer_next = 0;
	buzzer_time = TIME_SEC/8;
	buzzer_pause = 0;


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
	if (userA || userB) {
		A_enable = 0;
		B_enable = 0;
		car_ah = 1;
		wait_a = 0;
	}
	if (userA != 0){
		if (question_get_answer() == userA) {
			car_num = 0;
		} else {  /* wrong */
			car_num = 1;
		}
	}

	if (userB != 0){
		if (question_get_answer() == userB) {
			car_num = 2;
		} else {   /* wrong */
			car_num = 3;
		}
	}
}

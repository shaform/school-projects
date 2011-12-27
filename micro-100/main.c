#include <reg52.h>
#include "common.h"
#include "display.h"
#include "questions.h"
#include "buzzer.h"
#include "car.h"
#include "io.h"

// ---------------------------------------------------- //
// Game states

#define INIT 0
#define BACK_CAR 1
#define GAME_START 2
#define GAME_CHOOSE 3
#define NEXT_Q
#define DISP_Q
#define WAIT_A
#define ADV_CAR

// ---------------------------------------------------- //

static uchar carA_t = 0, carB_t = 0;
static uchar guess_result;
static bit stay;
static uchar wait_time, stay_time, buzzer_time;
static uchar temp_m;
static bit buzzer_next = 0;
static bit buzzer_pause = 0;

// ---------------------------------------------------- //

void game_routine(void);
void wait_answer();
void init(void);
void release_routine(void);
void buzzer_routine(void);
void stay_for(uchar);

// ---------------------------------------------------- //

void main()
{
	init();
	while (1) game_routine();
}

// ---------------------------------------------------- //

static void game_routine(void)
{
	if (stay) return;

	switch (state) {
		case INIT:
			display_clear();
			display_stop();
			if (car_is_A_back() && car_is_B_back())
				state = GAME_START;
			else {
				display_string("Moving back the cars...\r\n");
				display_stop();
				state = BACK_CAR;
			}
			break;
		case BACK_CAR:
			if (!car_is_A_back())
				car_A_back();
			if (!car_is_B_back())
				car_B_back();

			if (car_is_A_back() && car_is_B_back())
				state = GAME_START;
			break;
		case GAME_START:
			display_clear();
			display_string("Guess Game!! Are you ready?\r\n"
					"Press A for standard game...\r\n"
					"Press B for click click click mode...\r\n");
			display_stop();
			break;
		case GAME_CHOOSE:
			if (get_input_A() == BTNA || get_input_B() == BTNA)
				state = NEXT_Q;
			if (get_input_A() == BTNB || get_input_B() == BTNB)
				state = CLICK_START;
			break;
		case NEXT_Q:
			buzzer_play(0);  // Stop music
			display_clear();
			display_string("Next question!!\r\n"
					"Ready?");
			display_stop();
			stay_for(TIME_SEC*1);
			break;
		case DISP_Q:
			// Get a question 
			question_next();
			question_display();

			// Play music if present
			temp_m = question_get_music();
			if (temp_m) buzzer_play_num(temp_m);

			// Set waiting time
			wait_time = TIME_SEC*2;
			state = WAIT_A;
			break;
		case WAIT_A:
			wait_answer();  
			break;
		case ADV_CAR:
			display_clear();
			switch (guess_result) {
				case ARIGHT:
					display_string("Player1 is right!\r\n");
					break;
				case AWRONG:
					display_string("Player1 is wrong!\r\n");
					break;
				case BRIGHT:
					display_string("Player2 is right!\r\n");
					break;
				case BWRONG:
					display_string("Player2 is wrong!\r\n");
					break;
			}
			display_stop();

			if (guess_result == ARIGHT || guess_result == BWRONG)
				car_adv_A();
			else
				car_adv_B();

			state = WAIT_CAR;
			break;
		case WAIT_CAR:
			break;
	}
	release_routine();
	buzzer_routine();
}

// ---------------------------------------------------- //

void init(void)
{
	car_stop_A();
	car_stop_B();

	timer_init();
	display_init();
	question_init();
	buzzer_init();

	buzzer_next = 0;
	buzzer_time = TIME_SEC/8;
	buzzer_pause = 0;

	A_release = 1;
	B_release = 1;
}

// ---------------------------------------------------- //

static void wait_answer()
{
	uchar userA, userB;

	userA = get_input_A();
	userB = get_input_B();

	if (userA || userB) {
		state = ADV_CAR;

		if (userA != 0) {
			if (question_get_answer() == userA)
				guess_result = ARIGHT;
			else
				guess_result = AWRONG;
		}

		if (userB != 0) {
			if (question_get_answer() == userB)
				guess_result = BRIGHT;
			else
				guess_result = BWRONG;
		}
	}
}

// ---------------------------------------------------- //

static void stay_for(uchar t)
{
	stay = 1;
	stay_time = t;
}
void release_routine(void)
{
	uchar t = P0;
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
			buzzer_start();
			buzzer_time = TIME_SEC/8;
		} else if (buzzer_step()) {
			buzzer_stop();
			buzzer_pause = 1;
			buzzer_time = 1;
		} else {
			buzzer_time = TIME_SEC/8;
		}
		buzzer_next = 0;
	}
}

// ---------------------------------------------------- //

void timer1_int(void) interrupt 3
{
	if (--wait_time == 0) {
		wait_a = 0;  // next question
	}
	if (--stay_time == 0) {
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

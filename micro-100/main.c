#include <regx51.h>
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
#define NEXT_Q 4
#define DISP_Q 5
#define WAIT_A 6
#define ADV_CAR 7
#define WAIT_CAR 8

#define CLICK_START 9
#define CLICK_GO 10
#define CLICK_RUN 11

#define AWIN 17
#define BWIN 18
#define FINAL 19

#define ARIGHT 0
#define AWRONG 1
#define BRIGHT 2
#define BWRONG 3

code const char *ANSWERS[4] = {"A", "B", "C", "D"};

// ---------------------------------------------------- //

uchar carA_t = 0, carB_t = 0;
bit carA_di, carB_di;
uchar guess_result;
bit stay, wait_a;
uchar wait_time, stay_time, buzzer_time;
uchar temp_m, temp_a, temp_b;
bit buzzer_next = 0;
bit buzzer_pause = 0;

uchar state, next_state;

// ---------------------------------------------------- //

void game_routine(void);
void wait_answer();
void init(void);
void release_routine(void);
void buzzer_routine(void);
void stay_for(uchar);

// ---------------------------------------------------- //

bit ct_car_is_A_free(void);
bit ct_car_is_B_free(void);
void ct_car_back_A(void);
void ct_car_back_B(void);
void ct_car_adv_A(void);
void ct_car_adv_B(void);

// ---------------------------------------------------- //

void main()
{
	init();
//	buzzer_play_num(1);
	while (1) game_routine();
}

// ---------------------------------------------------- //

void game_routine(void)
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
			if (ct_car_is_A_free() && !car_is_A_back())
				ct_car_back_A();
			if (ct_car_is_B_free() && !car_is_B_back())
				ct_car_back_B();

			if (car_is_A_back() && car_is_B_back() && ct_car_is_A_free() && ct_car_is_B_free())
				state = GAME_START;
			break;
		case GAME_START:
			display_clear();
			display_string("Guess Game!! Are you ready?\r\n"
					"Press A for standard game...\r\n"
					"Press B for click click click mode...\r\n");
			display_stop();
			state = GAME_CHOOSE;
			break;
		case GAME_CHOOSE:
			temp_a = get_input_A();
			temp_b = get_input_B();

			if (temp_a == BTNA || temp_b == BTNA)
				state = NEXT_Q;
			if (temp_a == BTNB || temp_b == BTNB)
				state = CLICK_START;
			break;
		case NEXT_Q:
			buzzer_play_num(0);  // Stop music
			display_clear();
			display_string("Next question!!\r\n"
					"Ready?");
			display_stop();
			stay_for(TIME_SEC*1);
			state = DISP_Q;
			break;
		case DISP_Q:
			// Get a question 
			question_next();
			question_display();

			// Play music if present
			temp_m = question_get_music();
			if (temp_m) buzzer_play_num(temp_m);

			// Set waiting time
			wait_time = TIME_SEC*4;
			wait_a = 1;
			state = WAIT_A;
			break;
		case WAIT_A:
			if (wait_a)
				wait_answer();  
			else
				state = NEXT_Q;
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
			if (guess_result == ARIGHT || guess_result == BRIGHT) {
				display_string("The answer is ");
				display_string(ANSWERS[question_get_answer()-1]);
				display_string("\r\n");
			}
			display_stop();

			if (guess_result == ARIGHT || guess_result == BWRONG)
				ct_car_adv_A();
			else
				ct_car_adv_B();

			state = WAIT_CAR;
			break;
		case WAIT_CAR:
			if (ct_car_is_A_free() && ct_car_is_B_free()) {
				state = NEXT_Q;
				if (car_is_A_front())
					state = AWIN;
				if (car_is_B_front())
					state = BWIN;
				stay_for(TIME_SEC*1);
			}
			break;
		case CLICK_START:
			display_clear();
			display_string("Click Click Click!\r\n"
					"Ready?\r\n");
			display_stop();
			state = CLICK_GO;
			stay_for(TIME_SEC*3);
			break;
		case CLICK_GO:
			display_clear();
			display_string("GO!\r\n");
			display_stop();
			state = CLICK_RUN;
			break;
		case CLICK_RUN:
			if (ct_car_is_A_free() && get_input_A())
				ct_car_adv_A();
			if (ct_car_is_B_free() && get_input_B())
				ct_car_adv_B();
			if (car_is_A_front())
				state = AWIN;
			if (car_is_B_front())
				state = BWIN;
			break;
		case AWIN:
			if (ct_car_is_A_free() && ct_car_is_B_free()) {
				display_clear();
				display_string("Player1 is the winner!\r\n");
				display_string("Press D to restart!!\r\n");
				display_stop();
				state = FINAL;
				stay_for(TIME_SEC*5);
			}
			break;
		case BWIN:
			if (ct_car_is_A_free() && ct_car_is_B_free()) {
				display_clear();
				display_string("Player2 is the winner!\r\n");
				display_string("Press D to restart!!\r\n");
				display_stop();
				state = FINAL;
				stay_for(TIME_SEC*5);
			}
			break;
		case FINAL:
			if (get_input_A() == BTND || get_input_B() == BTND)
				state = INIT;
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

	wait_a = 0;

	next_state = state = INIT;
}

// ---------------------------------------------------- //

void wait_answer()
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

bit ct_car_is_A_free(void)
{
	return carA_t == 0;
}
bit ct_car_is_B_free(void)
{
	return carB_t == 0;
}
void ct_car_back_A(void)
{
	car_back_A();
	carA_t = 10;
	carA_di = 0;
}
void ct_car_back_B(void)
{
	car_back_B();
	carB_t = 10;
	carB_di = 0;
}
void ct_car_adv_A(void)
{
	car_adv_A();
	carA_t = 10;
	carA_di = 1;
}
void ct_car_adv_B(void)
{
	car_adv_B();
	carB_t = 10;
	carB_di = 1;
}

void stay_for(uchar t)
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
		--carA_t;
		if (carA_t == 9 || carA_t == 4)
			car_stop_A();
		if (carA_t == 5) {
			if (carA_di) {
				car_adv_A();
			} else {
				car_back_A();
			}
		}
	}
	if (carB_t) {
		--carB_t;
		if (carB_t == 9 || carB_t == 4)
			car_stop_B();
		if (carB_t == 5) {
			if (carB_di) {
				car_adv_B();
			} else {
				car_back_B();
			}
		}
	}
}

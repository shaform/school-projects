#include <reg52.h>
#include "common.h"
#include "display.h"
#include "questions.h"

void main()
{
	display_init();
	/*
	display_string("Wow!");
	display_char('x');
	display_string("\rThis is good");
	display_string("\rTwo\r\nLines.");
	display_string("\x1b[1;1f"); // Go to start
	display_string("\x1b[J"); // Clear screen
	display_string("Test!!");
	*/
	question_init();
	question_display();
	display_flush();
	delay(200);
	question_next();
	question_display();
	while (1) {
		display_routine();
	}
}

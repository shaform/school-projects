#include <reg52.h>
#include "common.h"
#include "display.h"

bit send_next, buffer_empty;
const char *ptr;
char buf[2];
void serial_int(void) interrupt 4
{
	if (TI == 1) {
		TI = 0;
		send_next = 1;
	} else if (RI == 1) {
		RI = 0;
	}
}
void display_init(void)
{
	TMOD &= 0xCF;  // Clear timer1 mode
	TMOD |= 0x20;  // Set timer1 to 8-bit auto-reload
	TH1 = 0xF3;
	TL1 = 0xF3;
	TR1 = 1;  // Start timer1
	ET1 = 0;  // Disable timer1 interrupt
	SCON = 0xD0;
	ES = 1;  // Enable serial interrupt
	EA = 1;  // Enable interrupt

	send_next = 0;
	buffer_empty = 1;
	ptr = 0;
	buf[1] = '\0';
}
void display_string(const char *str)
{
	display_flush();

	buffer_empty = 0;
	send_next = 1;
	ptr = str;
}
void display_char(char ch)
{
	display_flush();

	buffer_empty = 0;
	send_next = 1;
	buf[0] = ch;
	ptr = buf;
}
void display_routine(void)
{
	if (send_next) {
		send_next = 0;
		if (ptr == 0 || *ptr == '\0') {
			buffer_empty = 1;
		} else {
			SBUF = *ptr++;
		}
		//delay_serial();
	}
}
bit display_busy(void)
{
	return !buffer_empty;
}
void display_clear(void)
{
	display_string("\x1b[1;1f"); // Go to start
	display_string("\x1b[J"); // Clear screen
}
void display_flush(void)
{
	while (display_busy()) {
		display_routine();
	}
}

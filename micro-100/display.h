#ifndef DISPLAY_H
#define DISPLAY_H

/*
 * Display function uses Timer1,
 * it would stop the general timer and restore it afterawards.
 */

void display_init(void);
void display_string(const char *str);  // Send a string to the buffer
void display_char(char ch);
void display_flush(void);  // Flush all data
void display_clear(void);

void display_start(void);  // Call before display
void display_stop(void);  // Call after display
#endif

#ifndef DISPLAY_H
#define DISPLAY_H

/*
 * Display function uses Timer1,
 * Do not use timer1 for other purposes.
 */

void display_init(void);
void display_string(const char *str);  // Send a string to the buffer
void display_char(char ch);
void display_routine(void);  // Call it in a loop for display functions to work
bit display_busy(void);  // Check if display function is busy
void display_flush(void);  // Flush all data
void display_clear(void);

#endif

#ifndef COMMON_H_
#define COMMON_H_

#define uchar unsigned char
#define uint unsigned int

//void delay(int ms);
void delay_us(int us);

void timer_init(void);
void timer_stop(void);
void timer_restore(void);

#define TIME_SEC 20

#endif

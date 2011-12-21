#include <reg52.h>
#include "common.h"
#include "buzzer.h"

sbit  buzzer=P3^2;
uchar buzzer_time, tl, th;
uchar *ptm, it;

code uchar test_note[] = {
	5,1,1, 5,1,1, 6,1,2, 5,1,2, 1,2,2, 7,1,4,
	5,1,1, 5,1,1, 6,1,2, 5,1,2, 2,2,2, 1,2,4,
	5,1,1, 5,1,1, 5,2,2, 3,2,2, 1,2,2, 7,1,2, 6,1,2,
	4,2,1, 4,2,1, 3,2,2, 1,2,2, 2,2,2, 1,2,4, 0,0,0
};

code uchar *music[] = {
	&test_note,
	&test_note,
	&test_note,
	&test_note,
};
// Freq table, get from internet
// 0 = END, 9 = SILIENCE
code uchar FREQH[] = {
	0xF2,0xF3,0xF5,0xF5,0xF6,0xF7,0xF8, 
	0xF9,0xF9,0xFA,0xFA,0xFB,0xFB,0xFC,0xFC, //1,2,3,4,5,6,7,8,i
	0xFC,0xFD,0xFD,0xFD,0xFD,0xFE,
	0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFF,
};
code uchar FREQL[] = {
	0x42,0xC1,0x17,0xB6,0xD0,0xD1,0xB6,
	0x21,0xE1,0x8C,0xD8,0x68,0xE9,0x5B,0x8F, //1,2,3,4,5,6,7,8,i
	0xEE,0x44, 0x6B,0xB4,0xF4,0x2D, 
	0x47,0x77,0xA2,0xB6,0xDA,0xFA,0x16,
};


/*
   C   => 0
   C#  => 1
   D   => 2
   D#  => 3
   E   => 4
   F   => 5
   F#  => 6
   G   => 7
   G#  => 8
   A   => 9
   A#  => 10
   B   => 11
   */
void buzzer_stop(void)
{
	TR0 = 0;
}
void buzzer_start(void)
{
	TR0 = 1;
}
void set_freq(uchar i, uchar j)
{
	uchar f;
	f = i+j*7-1;
	TH0 = th = FREQH[f];
	TL0 = tl = FREQL[f];
}

void buzzer_play(uchar *notes)
{
	it = 0;
	ptm = notes;
	buzzer_time = 1;
}
void buzzer_step(void)
{
	if (ptm && ptm[it]) {
		if (--buzzer_time == 0) {
			buzzer_time = ptm[it+2];
			if (ptm[it] == 9)
				buzzer_stop();
			else {
				set_freq(ptm[it], ptm[it+1]);
				buzzer_start();
			}
			it += 3;
		}
	} else {
		buzzer_stop();
	}
}

void timer0_int(void) interrupt 1
{
	TR0 = 0;
	buzzer = !buzzer;
	TH0 = th;
	TL0 = tl;
	TR0 = 1;
}
void buzzer_init(void)
{
	TMOD &= 0xFC;
	TMOD |= 0x01;
	TH0 = 0x3C;
	TL0 = 0xB0;
	ET0 = 1;  // Enable timer0 interrupt
	EA = 1;  // Enable interrupt
	TR0 = 0;  // Disable timer0
	ptm = 0;
}


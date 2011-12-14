#include <reg52.h>
#include "common.h"
sbit  buzzer=P3^2;
unsigned char code tone[3][12]={
	{229, 217, 204, 193, 182, 172,
		162, 153, 145, 136, 129, 121},
	{115, 108, 102, 97, 91, 86,
		81, 77, 72, 68, 64, 61},	
	{57, 54, 51, 48, 45, 43,
		41, 38, 36, 34, 32, 30}
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
void delay8us(unsigned char);
void buzz(uchar i, uchar j);
//=======================================
void delay8us(unsigned char x)
{	unsigned char i,j;		
	for (i=0;i<x;i++)
		for (j=0;j<1;j++);	
}	

void buzz(uchar i, uchar j)	
{
	unsigned char k;
	if (j < 12) {
		for (k=0;k<60+i*120+j*10;k++) {
			buzzer = 0;
			delay8us(tone[i][j]);
			buzzer = 1;
			delay8us(tone[i][j]);
			buzzer = 0;
		}
		buzzer = 0;
	} else {
		for (k=0; k<20; k++)
			delay8us(255);
	}
	delay8us(255);
}
void play_music(uchar *notes, uchar sz)
{
	uchar i;
	for (i=0; i<sz; i++)
		buzz(1, notes[i]);
}

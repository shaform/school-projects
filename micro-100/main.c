#include <reg52.h>
#include "common.h"
#include "display.h"
#include "questions.h"

#define INIT_SCORE 128
#define LOSE_SCORE 118
#define WIN_SCORE  138

/* A1-A4:P0.0-P0.3, B1-B4:P0.4-P0.7 */
sbit A1 = P0^0;
sbit A2 = P0^1;
sbit A3 = P0^2;
sbit A4 = P0^3;
sbit B1 = P0^4;
sbit B2 = P0^5;
sbit B3 = P0^6;
sbit B4 = P0^7;
uchar wait_time;
bit wait = 0;

uchar A_scores = INIT_SCORE;
uchar B_scores = INIT_SCORE;

void timer0_init(void);
uchar userA_answer();
uchar userB_answer();
void wait_answer();

void timer0Int(void) interrupt 1
{
	TMOD &= 0xFC;
	TMOD |= 0x01;
	
	if(--wait_time == 0)
	{
		wait = 0;              // next question
	}
}

void main()
{
	//timer0_init();
	
	while(1)
	{   
	    if(wait == 0)
	    {
	        wait = 1;
	        /* get a question */
	        
	        /* reset timer0 */
	        timer0_init();
	    }
	    
	    wait_answer();  
	}
}


void timer0_init(void)
{
    wait_time = 20*10;
	TMOD &= 0xFC;
	TMOD |= 0x01;
	TH1 = 0x3C;
	TL0 = 0xB0;
	ET0 = 1;
	EA = 1;
	TR0 = 1;
}

uchar userA_answer()
{
	if(A1 == 0) { return 1; }
	if(A2 == 0) { return 2; }
	if(A3 == 0) { return 3; }
	if(A4 == 0) { return 4; }
	
	return 0;
}

uchar userB_answer()
{
	if(B1 == 0) { return 1; }
	if(B2 == 0) { return 2; }
	if(B3 == 0) { return 3; }
	if(B4 == 0) { return 4; }
	
	return 0;
}

void wait_answer()
{
	uchar userA, userB;
    if(wait == 1)
	{
	    userA = userA_answer();
		userB = userB_answer();
	    if(userA != 0)
		{
		    if(question_get_answer() == userA)
			{
			    A_scores++;
				if(A_scores == WIN_SCORE)
				{
				    /* Display LED and alarm */
				}
				wait = 0;
			}
			else   /* wrong */
			{
				A_scores = ((A_scores-1 <= LOSE_SCORE)?LOSE_SCORE:(A_scores-1));
			}   
		}
		
		if(userB != 0)
		{
			if(question_get_answer() == userB)
			{
				B_scores++;
				if(B_scores == WIN_SCORE)
				{
					 /* Display LED and alarm */
				}
				wait = 0;
			}
		    else   /* wrong */
			{
			     B_scores = ((B_scores-1 <= LOSE_SCORE)?LOSE_SCORE:(B_scores-1));
			}
		}
    }
}
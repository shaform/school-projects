#ifndef CAR_H
#define CAR_H
#include <regx51.h>
sbit carAfront = P2^6;
sbit carBfront = P2^7;
sbit carAback = P2^4;
sbit carBback = P2^5;

sbit carA_1 = P3^4;
sbit carA_2 = P3^5;
sbit carB_1 = P3^6;
sbit carB_2 = P3^7;

void car_stop_A(void);
void car_stop_B(void);
void car_adv_A(void);
void car_adv_B(void);
void car_back_A(void);
void car_back_B(void);
bit car_is_A_back(void);
bit car_is_B_back(void);
bit car_is_A_front(void);
bit car_is_B_front(void);

#endif

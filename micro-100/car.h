#ifndef CAR_H
#define CAR_H
sbit carAfront = P1^0;
sbit carBfront = P1^1;
sbit carAback = P1^2;
sbit carBback = P1^3;

sbit carA_1 = P1^4;
sbit carA_2 = P1^5;
sbit carB_1 = P1^6;
sbit carB_2 = P1^7;

#define car_stop_A() {carA_1 = 1; carA_2 = 1;}
#define car_stop_B() {carB_1 = 1; carB_2 = 1;}
#define car_adv_A() {carA_1 = 1; carA_2 = 0;}
#define car_adv_B() {carB_1 = 1; carB_2 = 0;}
#define car_back_A() {carA_1 = 0; carA_2 = 1;}
#define car_back_B() {carB_1 = 0; carB_2 = 1;}
#define car_is_A_back() (carAback == 0)
#define car_is_B_back() (carBback == 0)
#define car_is_A_front() (carAfront == 0)
#define car_is_B_front() (carBfront == 0)

#endif

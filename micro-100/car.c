#include "car.h"

void car_stop_A(void) {carA_1 = 1; carA_2 = 1;}
void car_stop_B(void) {carB_1 = 1; carB_2 = 1;}
void car_adv_A(void) {carA_1 = 1; carA_2 = 0;}
void car_adv_B(void) {carB_1 = 1; carB_2 = 0;}
void car_back_A(void) {carA_1 = 0; carA_2 = 1;}
void car_back_B(void) {carB_1 = 0; carB_2 = 1;}
bit car_is_A_back(void) {return carAback == 0;}
bit car_is_B_back(void) {return carBback == 0;}
bit car_is_A_front(void) {return carAfront == 0;}
bit car_is_B_front(void) {return carBfront == 0;}

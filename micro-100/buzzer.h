#ifndef BUZZER_H_
#define BUZZER_H_
#include "common.h"

void buzzer_play_num(uchar n);
void buzzer_init(void);
bit buzzer_step(void);

void buzzer_stop(void);
void buzzer_start(void);

#endif

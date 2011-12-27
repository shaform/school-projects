#ifndef BUZZER_H_
#define BUZZER_H_

void buzzer_play_num(uchar n);
void buzzer_init(void);
bit buzzer_step(void);

#define buzzer_stop() {TR0 = 0;}
#define buzzer_start() {TR0 = 1;}

#endif

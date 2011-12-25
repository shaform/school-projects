#ifndef BUZZER_H_
#define BUZZER_H_

void buzzer_play(uchar *notes);
void buzzer_play_num(uchar n);
void buzzer_init(void);
void buzzer_stop(void);
void buzzer_start(void);
bit buzzer_step(void);
void buzzer_test(void);

#endif

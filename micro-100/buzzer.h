#ifndef BUZZER_H_
#define BUZZER_H_

void buzzer_play_num(uchar n);
void buzzer_init(void);
bit buzzer_step(void);

inline void buzzer_stop(void)
{
	TR0 = 0;
}
inline void buzzer_start(void)
{
	TR0 = 1;
}

#endif

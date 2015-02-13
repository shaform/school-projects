#ifndef QUESTIONS_H
#define QUESTIONS_H
#include "common.h"

void question_init(void);  // Initialize questions, must be called after display_init
void question_next(void);  // Get next question
uchar question_get_answer(void);
uchar question_get_music(void);
void question_display(void);  // Display current question

#endif

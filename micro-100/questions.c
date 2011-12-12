#include <reg52.h>
#include "common.h"
#include "display.h"
#include "questions.h"

#define QN 3
#define MAXN 100

uchar qnum;
code struct Question {
	const char *desc;
	uchar answer;
	uchar music_id;
} questions[QN] = {
	{
		"1+2+3+4+5+7+8+9+10 = ?\r\n"
			"(A) 55  (B) 100  (C) 49  (D) 44\r\n",
		3,
		0
	},
	{
		"微處理機實驗共有幾個Lab?\r\n"
			"(A) 99  (B) 49  (C) 13  (D) 10\r\n",
		4,
		0
	},
};

void question_init(void)
{
	qnum = 0;
}
void question_next(void)
{
	qnum = (qnum+1) % QN;
}
void question_display(void)
{
	display_clear();
	display_string(questions[qnum].desc);
}
uchar question_get_answer(void)
{
	return questions[qnum].answer;
}

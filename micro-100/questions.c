#include <reg52.h>
#include "common.h"
#include "display.h"
#include "questions.h"

#define QN 6
#define MAXN 100

// http://www.tintinpiano.com/module/detail.php?id=81888
// http://www.tintinpiano.com/module/detail.php?id=80068
// http://www.gangqinpu.com/html/16375.htm

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
	{
		"Pipelining can improve?\r\n"
			"(A) Throughput  (B) CPI  (C) Data rate  (D) Hazards\r\n",
		1,
		0
	},
	{
		"請問這是哪部作品的歌曲?\r\n"
			"(A) 那些年，我們一起追的女孩  (B) 小資女孩向前衝  (C) 犀利人妻  (D) 我可能不會愛你\r\n",
		3,
		1
	},
	{
		"請問這是哪部作品的歌曲?\r\n"
			"(A) 那些年，我們一起追的女孩  (B) 小資女孩向前衝  (C) 犀利人妻  (D) 我可能不會愛你\r\n",
		1,
		2
	},
	{
		"請問這是哪部作品的歌曲?\r\n"
			"(A) 那些年，我們一起追的女孩  (B) 小資女孩向前衝  (C) 犀利人妻  (D) 我可能不會愛你\r\n",
		2,
		3
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
	display_stop();
}
uchar question_get_answer(void)
{
	return questions[qnum].answer;
}
// vim: set fileencodings=utf-8:

#include <regx51.h>
#include "common.h"
#include "display.h"
#include "questions.h"

#define QN 30
//#define QN 1

uchar qnum;
code struct Question {
	const char *desc;
	uchar answer;
	uchar music_id;
} questions[QN] = {
	{
		"請按D\r\n",
		4,
		0
	},
	{
		"1+2+3+4+5+7+8+9+10 = ?\r\n"
			"A) 55  B) 100  C) 49  D) 44\r\n",
		3,
		0
	},
	{
		"誰不是交大資工的老師?\r\n"
			"A) 陳榮傑 B) 徐慰中 C) 傅心家 D) 陸曉風\r\n",
		4,
		0
	},
	{
		"今年是?\r\n"
			"A) 100 年 B) 2011 年 C) 2012 年 D) 99 年\r\n",
		3,
		0
	},
	{
		"5 4 3 2 1 _\r\n"
			"根據以上的數列,_的數字最有可能是\r\n"
			"A) 0 B) 4 C) 7 D) 8\r\n",
		1,
		0
	},
	{
		"請問這是哪部作品的歌曲?\r\n"
			"A) 那些年，我們一起追的女孩  B) 小資女孩向前衝  C) 犀利人妻  D) 我可能不會愛你\r\n",
		3,
		3
	},
	{
		"從民國58年以來，梅竹賽中交大贏幾場??\r\n"
			"A) 15-17場 B) 18-20場 C) 21-23場 D) 24-26場\r\n",
		2,
		0
	},
	{
		"微處理機共有幾個Lab?\r\n"
			"A) 99  B) 49  C) 13  D) 10\r\n",
		4,
		0
	},
	{
		"立委大選共選出幾人?\r\n"
			"A) 225 B) 234 C) 245 D) 113 人\r\n",
		4,
		0
	},
	{
		"請按C\r\n",
		3,
		0
	},
	{
		"老外都知道，台灣有個阿里山。請問阿里山在哪個縣市啊?\r\n"
			"A) 南投 B) 台東 C) 嘉義 D) 雲林\r\n",
		3,
		0
	},
	{
		"Pipelining can improve?\r\n"
			"A) Throughput  B) CPI  C) Data rate  D) Hazards\r\n",
		1,
		0
	},
	{
		"金正日的接班人是?\r\n"
			"A) 金正哲 B) 金正恩 C) 金正男 D) 張成澤\r\n",
		2,
		0
	},
	{
		"下列誰不是新人歌手??\r\n"
			"A) 韋禮安 B) 卓文萱 C) 郁可為 D) 李宗盛\r\n",
		4,
		0
	},
	/*
	{
		"在600個家庭中，150個家庭沒有小孩，200個家庭只有1個男孩，200個家庭只有一個女孩，請問在有幾個家庭有男孩也有女孩?\r\n"
			"A) 350 B) 250 C) 50 D) 0\r\n",
		3,
		0
	},
	*/
	{
		"2012年總統大選，國民黨、民進黨、親民黨的總統候選人分別是:\r\n"
			"A) 吳敦義、蔡英文、宋楚瑜  B) 馬英九、蔡英文、林瑞雄\r\n"
			"B) 吳敦義、蘇嘉全、林瑞雄  D) 馬英九、蔡英文、宋楚瑜\r\n",
		4,
		0
	},
	/*
	{
		"培養一點人文氣息吧!!請問歌劇源自於哪個國家??\r\n"
			"A) 義大利 -  O sole mio!!!\r\n"
			"B) 法國 - 法國是歐洲最浪漫的地方!!\r\n"
			"C) 西班牙 - 很多著名歌劇都發生在西班牙喔!!\r\n"
			"D) 台灣 - 哈哈,其實台灣最厲害...\r\n",
		1,
		0
	},
	*/
	{
		"哪個演算法複雜度不是O(nlogn)\r\n"
			"A) quick sort B) selection sort C) radix sort D) merge sort\r\n",
		2,
		0
	},
	{
		"請按對方的鍵\r\n",
		5,
		0
	},
	/*
	{
		"哪個國家哪個運動強，其實跟當地很有關係哩!!  但是有個怪怪的哩!! 是哪個？\r\n"
			"A) 英國-足球-在草地上跑來跑去才不會冷哩!!\r\n"
			"B) 印尼-羽球-沒有風的國家最適合打羽球哩!!\r\n"
			"C) 韓國-足球-靠裁判哩!!\r\n"
			"D) 美國-籃球-城市裡都是水泥地,最適合打籃球哩!!\r\n",
		3,
		0
	},
	*/
	{
		"請問這是哪部作品的歌曲?\r\n"
			"A) 那些年，我們一起追的女孩  B) 小資女孩向前衝  C) 犀利人妻  D) 我可能不會愛你\r\n",
		1,
		1
	},
	{
		"下列哪一個問題是decidable?\r\n"
			"A) HALT B) PCP\r\n"
			"C) TM eq\r\n"
			"D) clique problem\r\n",
		4,
		0
	},
	{
		"這搶答遊戲是哪一組做的 =w=?\r\n"
			"A) 1 B) 2 C) 11 C) 12\r\n",
		3,
		0
	},
	{
		"下面哪一個英文單字拼錯哩!?\r\n"
			"A) colour B) episode C) dialog D) finnaly\r\n",
		4,
		0
	},
	{
		"請不要按!! =w=\r\n",
		5,
		0
	},
	{
		"哪一個不是台灣保育類動物?\r\n"
			"A) 台灣黑熊 B) 櫻花鉤吻鮭 C) 百步蛇 D) 穿山甲\r\n",
		3,
		0
	},
	{
		"請按A\r\n",
		1,
		0
	},
	{
		"交大資工大學部學生通識學分總共有多少?\r\n"
			"A) 15  B) 18  C) 20  D) 22\r\n",
		3,
		0
	},
	{
		"1+1 =?\r\n"
			"A) 1  B) 2  C) 3  D) 4\r\n",
		2,
		0
	},
	{
		"請問這是哪部作品的歌曲?\r\n"
			"A) 那些年，我們一起追的女孩  B) 小資女孩向前衝  C) 犀利人妻  D) 我可能不會愛你\r\n",
		2,
		2
	},
	/*
	{
		"請問下列哪部作品預計在2012年1月開撥?\r\n"
			"A) 蘿球社 B) 要聽爸爸的話 C) 魔法少女小圓 D) 我的朋友很少\r\n",
		2,
		0
	},
	*/
	{
		"有些英文字是髒話低!!  下面哪一個名字有問題??\r\n"
			"A) Willy B) Gary C) Eric D) Jill\r\n",
		1,
		0
	},
	{
		"這次專題哪一種遊戲有出現？\r\n"
			"A) LOL B) 武藤遊戲 C) 太鼓達人 D) 打地鼠\r\n",
		4,
		0
	},
	/*
	{
		"好色的管哈哈在搜索系上的女生當作下手目標!!\r\n"
			"但是他不小心把一個男生也選進去了!!請問是誰呢??\r\n"
			"A) 朱若慈 B) 林欣宜 C) 謝琦皓 D) 鍾敏雅\r\n",
		3,
		0
	},
	*/
	{
		"系學會會長是男是女？\r\n"
			"A) 男	B) 女 C) 皆非 D) 皆是\r\n",
		2,
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
	display_stop();
}
uchar question_get_answer(void)
{
	return questions[qnum].answer;
}
uchar question_get_music(void)
{
	return questions[qnum].music_id;
}
// vim: set fileencodings=cp950:

#include <reg52.h>
#include "common.h"
#include "display.h"
#include "questions.h"

#define QN 20

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
		"�аݭ��@�줣�O��j��u�t���б�(�]�t�Ʊб¡B�Ʊб�)?\r\n"
			"(A) ���a�� (B) �}���� (C) �Ť߮a (D) ���孷\r\n",
		4,
		0
	},
	{
		"4 9 5 7 6 5 7 _\r\n"
			"�ھڥH�W���ƦC,_���Ʀr�̦��i��O\r\n"
			"(A) 3 (B) 4 (C) 7 (D) 8\r\n",
		1,
		0
	},
	{
		"�L�B�z������@���X��Lab?\r\n"
			"(A) 99  (B) 49  (C) 13  (D) 10\r\n",
		4,
		0
	},
	{
		"�ثe�ߩe�j��@��X�X�H?\r\n"
			"(A) 225 (B) 234 (C) 245 (D) 113 �H\r\n",
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
		"2011�~12��19�������u�@�A�аݥثe�L�����Z�H�O?\r\n"
			"(A) ������ (B) ������ (C) �����k (D) �i���A\r\n",
		2,
		0
	},
	{
		"�аݳo�O�����@�~���q��?\r\n"
			"(A) ���Ǧ~�A�ڭ̤@�_�l���k��  (B) �p��k�ĦV�e��  (C) �R�Q�H�d  (D) �ڥi�ण�|�R�A\r\n",
		3,
		1
	},
	{
		"�b600�Ӯa�x���A150�Ӯa�x�S���p�ġA200�Ӯa�x�u��1�Өk�ġA200�Ӯa�x�u���@�Ӥk�ġA�аݦb���X�Ӯa�x���k�Ĥ]���k��?\r\n"
			"(A) 350 (B) 250 (C) 50 (D) 0\r\n",
		3,
		0
	},
	{
		"2012�~(�ĤQ�T��)�`�Τj��A����ҡB���i�ҡB�˥��Ҫ��`�έԿ�H���O�O:\r\n"
			"(A) �d���q�B���^��B������  (B) ���^�E�B���^��B�L�綯\r\n"
			"(B) �d���q�BĬ�ť��B�L�綯  (D) ���^�E�B���^��B������\r\n",
		4,
		0
	},
	{
		"�U�C���@�ӱƧǺt��k�������ɶ������פ��OO(nlogn)\r\n"
			"(A) quick sort (B) selection sort (C) radix sort (D) merge sort\r\n",
		2,
		0
	},
	{
		"�аݳo�O�����@�~���q��?\r\n"
			"(A) ���Ǧ~�A�ڭ̤@�_�l���k��  (B) �p��k�ĦV�e��  (C) �R�Q�H�d  (D) �ڥi�ण�|�R�A\r\n",
		1,
		3
	},
	{
		"�U�C���@�Ӱ��D�Odecidable?\r\n"
			"(A) Halting problem (B) post correspondence problem\r\n"
			"(C) a turing machine determines whehter two turing marchines are equal\r\n"
			"(D) clique problem\r\n",
		4,
		0
	},
	{
		"�o�m���C���O���@�հ��� =w=?\r\n"
			"(A) 1 (B) 2 (C) 11 (C) 12\r\n",
		3,
		0
	},
	{
		"�bWOW���G�Ƥ��A���@��O�Q��Ѫ̪���ɤH\r\n"
			"(A) Sylvanas (B) Thrall (C) Malfurion (D) Arthas\r\n",
		1,
		0
	},
	{
		"��j��u�j�ǳ��ǥͳq�ѾǤ��`�@���h��?\r\n"
			"(A) 15  (B) 18  (C) 20  (D) 22\r\n",
		3,
		0
	},
	{
		"�аݳo�O�����@�~���q��?\r\n"
			"(A) ���Ǧ~�A�ڭ̤@�_�l���k��  (B) �p��k�ĦV�e��  (C) �R�Q�H�d  (D) �ڥi�ण�|�R�A\r\n",
		2,
		3
	},
	{
		"�аݤU�C�����@�~�w�p�b2012�~1��}��?\r\n"
			"(A) �ڲy�� (B) �nť�������� (C) �]�k�֤k�p�� (D) �ڪ��B�ͫܤ�\r\n",
		2,
		0
	},
	{
		"�аݽ֭��عC���b�o�������M�D�Q��տ�w�A�B�o�Q��]�]�ۦP?\r\n"
			"(A) �氫�C�� (B) �Z�ùC�� (C) �ӹ��F�H (D) ���a��\r\n",
		4,
		0
	},
	{
		"�аݳo�O�����@�~���q��?\r\n"
			"(A) ���Ǧ~�A�ڭ̤@�_�l���k��  (B) �p��k�ĦV�e��  (C) �R�Q�H�d  (D) �ڥi�ण�|�R�A\r\n",
		2,
		4
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
// vim: set fileencodings=utf-8:

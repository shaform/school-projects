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
		"�Ы�D\r\n",
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
		"�֤��O��j��u���Ѯv?\r\n"
			"A) ���a�� B) �}���� C) �Ť߮a D) ���孷\r\n",
		4,
		0
	},
	{
		"���~�O?\r\n"
			"A) 100 �~ B) 2011 �~ C) 2012 �~ D) 99 �~\r\n",
		3,
		0
	},
	{
		"5 4 3 2 1 _\r\n"
			"�ھڥH�W���ƦC,_���Ʀr�̦��i��O\r\n"
			"A) 0 B) 4 C) 7 D) 8\r\n",
		1,
		0
	},
	{
		"�аݳo�O�����@�~���q��?\r\n"
			"A) ���Ǧ~�A�ڭ̤@�_�l���k��  B) �p��k�ĦV�e��  C) �R�Q�H�d  D) �ڥi�ण�|�R�A\r\n",
		3,
		3
	},
	{
		"�q����58�~�H�ӡA�����ɤ���jĹ�X��??\r\n"
			"A) 15-17�� B) 18-20�� C) 21-23�� D) 24-26��\r\n",
		2,
		0
	},
	{
		"�L�B�z���@���X��Lab?\r\n"
			"A) 99  B) 49  C) 13  D) 10\r\n",
		4,
		0
	},
	{
		"�ߩe�j��@��X�X�H?\r\n"
			"A) 225 B) 234 C) 245 D) 113 �H\r\n",
		4,
		0
	},
	{
		"�Ы�C\r\n",
		3,
		0
	},
	{
		"�ѥ~�����D�A�x�W���Ӫ����s�C�аݪ����s�b���ӿ�����?\r\n"
			"A) �n�� B) �x�F C) �Ÿq D) ���L\r\n",
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
		"�����骺���Z�H�O?\r\n"
			"A) ������ B) ������ C) �����k D) �i���A\r\n",
		2,
		0
	},
	{
		"�U�C�֤��O�s�H�q��??\r\n"
			"A) ��§�w B) ���帩 C) ���i�� D) ���v��\r\n",
		4,
		0
	},
	/*
	{
		"�b600�Ӯa�x���A150�Ӯa�x�S���p�ġA200�Ӯa�x�u��1�Өk�ġA200�Ӯa�x�u���@�Ӥk�ġA�аݦb���X�Ӯa�x���k�Ĥ]���k��?\r\n"
			"A) 350 B) 250 C) 50 D) 0\r\n",
		3,
		0
	},
	*/
	{
		"2012�~�`�Τj��A����ҡB���i�ҡB�˥��Ҫ��`�έԿ�H���O�O:\r\n"
			"A) �d���q�B���^��B������  B) ���^�E�B���^��B�L�綯\r\n"
			"B) �d���q�BĬ�ť��B�L�綯  D) ���^�E�B���^��B������\r\n",
		4,
		0
	},
	/*
	{
		"���i�@�I�H��𮧧a!!�аݺq�@���۩���Ӱ�a??\r\n"
			"A) �q�j�Q -  O sole mio!!!\r\n"
			"B) �k�� - �k��O�ڬw�̮������a��!!\r\n"
			"C) ��Z�� - �ܦh�ۦW�q�@���o�ͦb��Z����!!\r\n"
			"D) �x�W - ����,���x�W�̼F�`...\r\n",
		1,
		0
	},
	*/
	{
		"���Ӻt��k�����פ��OO(nlogn)\r\n"
			"A) quick sort B) selection sort C) radix sort D) merge sort\r\n",
		2,
		0
	},
	{
		"�Ы���誺��\r\n",
		5,
		0
	},
	/*
	{
		"���Ӱ�a���ӹB�ʱj�A������a�ܦ����Y��!!  ���O���өǩǪ���!! �O���ӡH\r\n"
			"A) �^��-���y-�b��a�W�]�Ӷ]�h�~���|�N��!!\r\n"
			"B) �L��-�вy-�S��������a�̾A�X���вy��!!\r\n"
			"C) ����-���y-�a���P��!!\r\n"
			"D) ����-�x�y-�����̳��O���d�a,�̾A�X���x�y��!!\r\n",
		3,
		0
	},
	*/
	{
		"�аݳo�O�����@�~���q��?\r\n"
			"A) ���Ǧ~�A�ڭ̤@�_�l���k��  B) �p��k�ĦV�e��  C) �R�Q�H�d  D) �ڥi�ण�|�R�A\r\n",
		1,
		1
	},
	{
		"�U�C���@�Ӱ��D�Odecidable?\r\n"
			"A) HALT B) PCP\r\n"
			"C) TM eq\r\n"
			"D) clique problem\r\n",
		4,
		0
	},
	{
		"�o�m���C���O���@�հ��� =w=?\r\n"
			"A) 1 B) 2 C) 11 C) 12\r\n",
		3,
		0
	},
	{
		"�U�����@�ӭ^���r������!?\r\n"
			"A) colour B) episode C) dialog D) finnaly\r\n",
		4,
		0
	},
	{
		"�Ф��n��!! =w=\r\n",
		5,
		0
	},
	{
		"���@�Ӥ��O�x�W�O�|���ʪ�?\r\n"
			"A) �x�W�º� B) ���_�k�D C) �ʨB�D D) ��s��\r\n",
		3,
		0
	},
	{
		"�Ы�A\r\n",
		1,
		0
	},
	{
		"��j��u�j�ǳ��ǥͳq�ѾǤ��`�@���h��?\r\n"
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
		"�аݳo�O�����@�~���q��?\r\n"
			"A) ���Ǧ~�A�ڭ̤@�_�l���k��  B) �p��k�ĦV�e��  C) �R�Q�H�d  D) �ڥi�ण�|�R�A\r\n",
		2,
		2
	},
	/*
	{
		"�аݤU�C�����@�~�w�p�b2012�~1��}��?\r\n"
			"A) �ڲy�� B) �nť�������� C) �]�k�֤k�p�� D) �ڪ��B�ͫܤ�\r\n",
		2,
		0
	},
	*/
	{
		"���ǭ^��r�Oż�ܧC!!  �U�����@�ӦW�r�����D??\r\n"
			"A) Willy B) Gary C) Eric D) Jill\r\n",
		1,
		0
	},
	{
		"�o���M�D���@�عC�����X�{�H\r\n"
			"A) LOL B) �Z�ùC�� C) �ӹ��F�H D) ���a��\r\n",
		4,
		0
	},
	/*
	{
		"�n�⪺�ޫ����b�j���t�W���k�ͷ��@�U��ؼ�!!\r\n"
			"���O�L���p�ߧ�@�Өk�ͤ]��i�h�F!!�аݬO�֩O??\r\n"
			"A) ���Y�O B) �L�Y�y C) �µa�q D) ��Ӷ�\r\n",
		3,
		0
	},
	*/
	{
		"�t�Ƿ|�|���O�k�O�k�H\r\n"
			"A) �k	B) �k C) �ҫD D) �ҬO\r\n",
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
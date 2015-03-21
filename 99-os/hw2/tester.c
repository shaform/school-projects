#include "mythread.h"
#include <stdio.h>

int beg=0, end=0;

int arr[100];
int SZ = 10;

void *runner1(void *param); /* the producer */
void *runner2(void *param); /* the consumer */

int main()
{
	th_t ths[SZ];
	// the scheduler will be preemptive
	setpreemptive();

	int i;
	for (i=0; i<SZ; ++i) {
		th_init(&ths[i]);
		if (i%2)
			th_fork(&ths[i], runner1, NULL);
		else
			th_fork(&ths[i], runner2, NULL);
	}

	/* now wait for the thread to exit */
	int ret;
	for (i=0; i<SZ; ++i) {
		th_wait(&ths[i], &ret);
		printf("thread #%d, ret:%d\n", i, ret);
	}
	return 0;
}

void *runner1(void *param)
{
	int i, upper = 20;
	i=0;
	int j,k,l;
	while (++i <= upper) {
		printf("runner 1 %dth of %d.\n", i, upper);
		while ((end+1) % 100 == beg);
		arr[end++] = i;
		printf("runner 1 %d  produced %d\n", upper,  i);
		for (k=0; k< 1000000; ++k) ++l;
	}

	static int t = 1;
	return &t;
}
void *runner2(void *param)
{
	int i, upper = 20;
	i=0;
	int j,k,l;
	while (++i <= upper) {
		printf("runner 2 %dth of %d.\n", i, upper);
		while (end == beg);
		j = arr[beg++];
		printf("runner 2 %d consumed %d\n", upper, j);
		for (k=0; k< 1000000; ++k) ++l;
	}

	static int t = 2;
	return &t;
}

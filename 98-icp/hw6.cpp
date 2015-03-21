#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define RANDN(n) (rand()%(n))

const int sz=20;
int a[sz];


void in(int n);
void out(int n);
void ssort(int n);


bool bsearch(int k, int l, int h);
bool find(int k, int n);

int main(void)
{
	time_t t;
	srand(time(&t));

	in(sz);
	printf("Before sorting ...\n");
	out(sz);
	printf("\n");

	ssort(sz);

	printf("After sorting ...\n");
	out(sz);
	printf("\n");

	int to_find = RANDN(100);
	printf("Binary searching for %d ...\n", to_find);
	if (find(to_find, sz))
		printf("%d is found\n", to_find);
	else
		printf("%d isn't found\n", to_find);
	system("pause");

	return 0;
}


void in(int n)
{
	if (n!=0) {
		a[n-1] = RANDN(100);
		in(n-1);
	}
}

void out(int n)
{
	if (n!=1) {
		out(n-1);
		printf(" ");
	}
	printf("%d", a[n-1]);
}
void ssort(int n)
{
	if (n!=0) {
		int min_idx, first;
		first = min_idx = sz-n;

		for (int i=min_idx+1; i<sz; ++i)
			if (a[i]<a[min_idx])
				min_idx = i;

		int temp = a[first];
		a[first] = a[min_idx];
		a[min_idx] = temp;
		ssort(n-1);
	}
}


bool bsearch(int k, int l, int h)
{
	if (l>h)
		return false;

	int mid = (l+h)/2;
	if (k<a[mid])
		return bsearch(k, l, mid-1);
	else if (k>a[mid])
		return bsearch(k, mid+1, h);
	else
		return true;
}

bool find(int k, int n)
{
	return bsearch(k, 0, n-1);
}

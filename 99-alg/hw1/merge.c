#include <stdio.h>

#define MAX 100000


int input[MAX], temp[MAX];

void merge(int *beg, int *mid, int *end)
{
	int *it1 = beg, *it2 = mid;
	int *it_back = temp, *it_front = temp;

	while (it1 != mid && it2 != end) {
		if (*it1 < *it2)
			*(it_back++) = *(it1++);
		else
			*(it_back++) = *(it2++);
	}

	while (it1 != mid)
		*(it_back++) = *(it1++);

	while (it2 != end)
		*(it_back++) = *(it2++);

	while (beg != end)
		*(beg++) = *(it_front++);

}

void merge_sort(int *beg, int *end)
{
	int size = end-beg;
	if (size <= 1)
		return;
	if (size == 2) {
		if (*beg > *(beg+1)) {
			int t = *beg;
			*beg = *(beg+1);
			*(beg+1) = t;
		}
		return;
	}


	int *mid = beg + size/2;
	merge_sort(beg, mid);
	merge_sort(mid, end);

	merge(beg, mid, end);

}

int main()
{
	int cases;
	scanf("%d", &cases);
	while (cases--) {
		int n;
		scanf("%d", &n);
		int i;
		for (i=0; i<n; ++i)
			scanf("%d", &input[i]);

		merge_sort(input, input+n);

		for (i=0; i<n; ++i)
			printf("%d\n", input[i]);
	}
	return 0;
}

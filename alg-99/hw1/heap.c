#include <stdio.h>

#define MAX 100000


int input[MAX];

void heapify(int vec[], int u, int size)
{
	int l = u*2+1, r = u*2+2;

	int maxi = u;

	if (l < size && vec[l] > vec[maxi])
		maxi = l;
	if (r < size && vec[r] > vec[maxi])
		maxi = r;

	if (maxi != u) {
		int t = vec[u];
		vec[u] = vec[maxi];
		vec[maxi] = t;
		heapify(vec, maxi, size);
	}
}
void build_heap(int vec[], int size)
{
	int i;
	for (i=size/2-1; i>=0; --i)
		heapify(vec, i, size);

}
void heap_pop(int vec[], int size)
{
	int t = vec[0];
	vec[0] = vec[size-1];
	vec[size-1] = t;
	heapify(vec, 0, size-1);
}
void heap_sort(int vec[], int size)
{
	build_heap(vec, size);
	while (size>1) heap_pop(vec, size--);
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

		heap_sort(input, n);

		for (i=0; i<n; ++i)
			printf("%d\n", input[i]);
	}
	return 0;
}

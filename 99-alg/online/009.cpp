#include <cstdio>

using namespace std;
const int MAX = 3000;
int arr[MAX];
int maxi[MAX];

int main()
{
	int T;
	scanf("%d", &T);
	while (T--) {
		int N;
		scanf("%d", &N);
		for (int i=0; i<N; ++i) {
			scanf("%d", &arr[i]);
			maxi[i] = 1;
		}

		int _max = 1;

		for (int i=N-1; i>0; --i) {
			int p = (i-1)/2;
			if (maxi[i] && maxi[p] && arr[i] > arr[p]) maxi[p] += maxi[i];
			else maxi[p] = 0;

			if (maxi[i] > _max) _max = maxi[i];
		}
		if (maxi[0] > _max) _max = maxi[0];

		printf("%d\n", _max);

	}
}


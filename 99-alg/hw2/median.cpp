#include <cstdio>
#include <algorithm>
using namespace std;

int getNumberOfTestCase();
int getLengthOfArray(int);
int getElementOfArray(int, int, int);

int cs;
struct Getter
{
	int num;
	int operator[](int n) const
	{
		return getElementOfArray(cs, num, n);
	}
} getter[2];

int solve(int n)
{
	// Search for median in array 0 & array 1
	for (int i=0, j=1; i<2; ++i, --j) {
		int l=0, r=n;
		while (l<r) {
			int m = (l+r)/2;
			// Checks if there are at least n-1 elements <= getter[i][m].
			if (m != n-1 && getter[j][n-m-2] > getter[i][m])
				l = m + 1;
			// Checks if there are at least n elements >= getter[i][m].
			else if (getter[j][n-m-1] < getter[i][m])
				r = m;
			// Finds the answer!!
			else return getter[i][m];
		}
	}
}

int main()
{
	getter[0].num = 0;
	getter[1].num = 1;

	int T = getNumberOfTestCase();

	for(cs = 0; cs < T; ++cs) {
		printf("%d\n", solve(getLengthOfArray(cs)));
	}
	return 0;
}

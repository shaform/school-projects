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

int median_res(int p, int q, int r, int s, int n)
{
	int m1 = (p+q)/2;
	int m2 = (r+s)/2;
	int a = getter[0][m1], b = getter[1][m2];

	if (n == 1)
		return a > b ? b : a;

	if (a == b)
		return a;
	else if (a > b) {
		if (n % 2)
			return median_res(p, m1, m2, s, (n+1)/2);
		else
			return median_res(p, m1, m2+1, s, n/2);
	} else {
		if (n % 2)
			return median_res(m1, q, r, m2, (n+1)/2);
		else
			return median_res(m1+1, q, r, m2, n/2);
	}
}
int solve(int n)
{
	return median_res(0, n-1, 0, n-1, n);
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

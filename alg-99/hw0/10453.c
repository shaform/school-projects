#include <stdio.h>


int main()
{
	int cases;
	scanf("%d", &cases);
	int i;
	for (i=1; i<=cases; ++i) {
		int a, b;
		scanf("%d%d", &a, &b);
		
		if (a%2 == 0)
			++a;
		if (b%2 == 0)
			--b;

		printf("Case %d: %d\n", i, a<=b ? (a+b)*((b-a)/2+1)/2 : 0);

	}
	return 0;
}

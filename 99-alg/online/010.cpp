#include <cstdio>
#include <algorithm>

using namespace std;

typedef long long LLT;

const int MAX = 200;
int cut[MAX+2];
LLT dp[MAX+2][MAX+2];
int main()
{
	int L;
	while (scanf("%d", &L) == 1 && L) {
		int N;
		scanf("%d", &N);
		for (int i=1; i<=N; ++i)
			scanf("%d", &cut[i]);
		cut[N+1] = L;

		for (int k=1; k<=N; ++k)
			for (int i=0; i<N+1-k; ++i) {
				dp[i][i+k+1] = dp[i+1][i+k+1];
				for (int j = i+2; j<=i+k; ++j)
					dp[i][i+k+1] = min(dp[i][j]+dp[j][i+k+1], dp[i][i+k+1]);

				dp[i][i+k+1] += cut[i+k+1] - cut[i];
			}
		printf("%d\n", dp[0][N+1]);
	}
	return 0;
}


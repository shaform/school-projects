#include <cstdio>
#include <cstring>

using namespace std;

const int MAXM = 10000;
const int MAXN = 50;
const int MAXP = 50;

int dp[2][MAXM+1];
bool hv[2][MAXM+1];
struct Person {
	int w, v;
};

Person people[MAXN][MAXP];

int main()
{
	int T;
	scanf("%d", &T);
	int M, N, P;
	while (T-- && scanf("%d%d%d", &M, &N, &P) == 3) {
		memset(dp, 0, sizeof(dp));
		memset(hv, 0, sizeof(hv));

		for (int i=0; i<N; ++i)
			for (int j=0; j<P; ++j)
				scanf("%d%d", &people[i][j].w, &people[i][j].v);
		int up = 0;
		int now = 0, next = 1;
		hv[0][0] = true;
		if (N > 0)
			for (int i=0; i<P; ++i) {
				if (people[0][i].w <= M && dp[0][people[0][i].w] < people[0][i].v) {
					dp[0][people[0][i].w] = people[0][i].v;
					hv[0][people[0][i].w] = true;
					if (people[0][i].w > up)
						up = people[0][i].w;
				}
			}
		for (int i=1; i<N; ++i) {
			int nextup = up;
			memcpy(dp[next], dp[now], sizeof(dp[next]));
			memcpy(hv[next], hv[now], sizeof(hv[next]));
			for (int j=0; j<=up; ++j) {
				if (!hv[now][j])
					continue;
				for (int k=0; k<P; ++k) {
					int nextw = people[i][k].w + j;
					if (nextw <= M && dp[next][nextw] < dp[now][j] + people[i][k].v) {
						dp[next][nextw] = dp[now][j] + people[i][k].v;
						hv[next][nextw] = true;
						if (nextw > nextup)
							nextup = nextw;
					}

				}
			}

			up = nextup;

			now = (now+1)%2;
			next = (next+1)%2;
		}
		int ans = dp[now][0];
		for (int i=1; i<=M; ++i)
			if (dp[now][i] > ans)
				ans = dp[now][i];
		printf("%d\n", ans);
		if (T) scanf("%d", &ans);
	}
	return 0;
}

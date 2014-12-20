#include <cstdio>
#include <cstring>
using namespace std;

const int MAXN = 1000;

int board[MAXN][MAXN];
int dp[MAXN][MAXN];
bool visited[MAXN][MAXN];

int main()
{
	int T;
	scanf("%d", &T);
	while (T--) {
		int N;
		scanf("%d", &N);
		for (int i=0; i<N; ++i)
			for (int j=0; j<N; ++j)
				scanf("%d", &board[i][j]);
		memset(dp, 0xff, sizeof(dp));
		memset(visited, 0, sizeof(visited));
		memcpy(dp[0], board[0], sizeof(dp[0]));
		for (int i=0; i<N-1; ++i)
			for (int j=0; j<N; ++j) {
				if (!visited[i+1][j] || dp[i+1][j] < dp[i][j] + board[i+1][j]) {
					dp[i+1][j] = dp[i][j] + board[i+1][j];
					visited[i+1][j] = true;
				}
				if (j+1 < N && (!visited[i+1][j+1] || dp[i+1][j+1] < dp[i][j] + board[i+1][j+1])) {
					dp[i+1][j+1] = dp[i][j] + board[i+1][j+1];
					visited[i+1][j+1] = true;
				}
				if (j-1 >= 0 && (!visited[i+1][j-1] || dp[i+1][j-1] < dp[i][j] + board[i+1][j-1])) {
					dp[i+1][j-1] = dp[i][j] + board[i+1][j-1];
					visited[i+1][j-1] = true;
				}
			}
		int ans = dp[N-1][0];
		for (int i=1; i<N; ++i)
			if (dp[N-1][i] > ans)
				ans = dp[N-1][i];
		printf("%d\n", ans);
	}
	return 0;
}

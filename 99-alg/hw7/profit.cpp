#include <cstdio>
#include <cstring>
#include <algorithm>
#include <climits>
#include <queue>
using namespace std;

const int MAXN = 50;
const int MAXM = 50;
const int MAX = MAXN + MAXM + 2;
const unsigned INF = UINT_MAX;
int N, M;

unsigned capacity[MAX][MAX];
bool visited[MAX];
int py[MAX];
struct Node {
	int u;
	unsigned minc;
	Node() {}
	Node(int a, int b) : u(a), minc(b) {}
};

bool aug()
{
	memset(visited, 0, sizeof(visited));
	memset(py, 0, sizeof(py));

	queue<Node> q;
	q.push(Node(0, INF));
	visited[0] = true;

	while (!q.empty()) {
		Node u = q.front();
		q.pop();
		if (u.u==N+M+1) {
			unsigned c = u.minc;
//			printf("##Aug## : %u\n", c);
			int v = u.u;
			while (v != 0) {
//				printf("%d <- ", v);
				if (capacity[v][py[v]] != INF)
					capacity[v][py[v]] += c;
				if (capacity[py[v]][v] != INF)
					capacity[py[v]][v] -= c;
				v = py[v];
			}
//			printf("\n");
			return true;
		}
		for (int i=0; i<=N+M+1; ++i) {
			if (capacity[u.u][i] && !visited[i]) {
				visited[i] = true;
				py[i] = u.u;
				q.push(Node(i, min(u.minc, capacity[u.u][i])));
			}
		}
	}
	return false;
}
int main()
{
	int T;
	scanf("%d", &T);
	while (T--) {
		scanf("%d%d", &N, &M);
		memset(capacity, 0, sizeof(capacity));
		unsigned ans = 0;
		for (int i=1; i<=N; ++i) {
			scanf("%u", &capacity[i][N+M+1]);
			ans += capacity[i][N+M+1];
		}
		for (int i=1; i<=M; ++i)
			scanf("%u", &capacity[0][N+i]);
		for (int i=1; i<=N; ++i) {
			int t;
			scanf("%d", &t);
			for (int j=0; j<t; ++j) {
				int u;
				scanf("%d", &u);
				capacity[N+u][i] = INF;
			}
		}
		while (aug())
			;
		for (int i=1; i<=M; ++i)
			ans -= capacity[N+i][0];

		printf("%u\n", ans);
	}
	return 0;
}

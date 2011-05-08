#include <cstdio>
#include <algorithm>
#include <climits>
#include <cstring>

using namespace std;
// --- heapsort --- //
template <typename T, typename CMP>
void heapify(T vec[], int u, int size, CMP cmp)
{
	int l = u*2+1, r = u*2+2;

	int mini = u;

	if (l < size && cmp(vec[l], vec[mini]))
		mini = l;
	if (r < size && cmp(vec[r], vec[mini]))
		mini = r;

	if (mini != u) {
		T t = vec[u];
		vec[u] = vec[mini];
		vec[mini] = t;
		heapify(vec, mini, size, cmp);
	}
}
template <typename T, typename CMP>
void build_heap(T vec[], int size, CMP cmp)
{
	int i;
	for (i=size/2-1; i>=0; --i)
		heapify(vec, i, size, cmp);

}
template <typename T, typename CMP>
void heap_pop(T vec[], int size, CMP cmp)
{
	T t = vec[0];
	vec[0] = vec[size-1];
	vec[size-1] = t;
	heapify(vec, 0, size-1, cmp);
}
template <typename T, typename CMP>
void heap_sort(T vec[], int size, CMP cmp)
{
	build_heap(vec, size, cmp);
	while (size>1) heap_pop(vec, size--, cmp);
}

const int MAX = 6000;
const int INF = INT_MAX;
struct Turtle {
	int w, p;
};
Turtle arr[MAX];
int sz = 0;
bool t_cmp(const Turtle &fst, const Turtle &snd)
{
	return fst.p < snd.p;
}
int dp[2][MAX];

int main()
{
	while (scanf("%d%d", &arr[sz].w, &arr[sz].p) == 2)
		++sz;

	heap_sort(arr, sz, t_cmp);

	int now = 0, next = 1, ans = 0, up = 0;

	memset(dp[0], 0xff, sizeof(dp[0]));
	if (sz > 0 && arr[0].p >= arr[0].w) {
		dp[0][0] = INF;
		dp[0][1] = arr[0].p - arr[0].w;
		ans = 1;
		if (dp[0][1] > 0)
			up = 1;
	}

	for (int i=1; i<sz; ++i) {
		if (arr[i].p < arr[i].w)
			continue;
		memset(dp[next], 0xff, sizeof(dp[next]));
		for (int j=0; j<=up; ++j) {
			if (dp[now][j] >= arr[i].w) {
				dp[next][j+1] = min(dp[now][j]-arr[i].w, arr[i].p-arr[i].w);
			}
			if (dp[now][j] > dp[next][j])
				dp[next][j] = dp[now][j];
		}

		if (dp[next][up+1] >= 0)
			ans = max(up+1, ans);
		if (dp[next][up+1] > 0)
			++up;

		now = (now+1)%2;
		next = (next+1)%2;
	}
	printf("%d\n", ans);

	return 0;
}

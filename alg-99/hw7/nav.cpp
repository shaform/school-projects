#include <cstdio>
#include <cstring>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;
const int MAXN = 90000;
const int MAXM = 130000;
typedef vector<int>::iterator It;

struct Node {
	int x, y;
	int h;
	double len;
	double dl;
	vector<int> edge;
};
double operator-(const Node &a, const Node &b)
{
	return sqrt(static_cast<double>((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y)));
}
Node nds[MAXN];
bool marked[MAXN];
int dst;
bool cmp(int a, int b)
{
	return nds[a].dl + nds[a].len < nds[b].dl + nds[b].len;
}

// ------------------------------------------------------------------ //
int heap[MAXN];
int sz;
void heap_init()
{
	sz = 0;
}
void heapify_up(int idx)
{
	int p = (idx-1)/2;

	if (p >= 0 && cmp(heap[idx], heap[p])) {
		swap(heap[p], heap[idx]);
		swap(nds[heap[p]].h, nds[heap[idx]].h);
		heapify_up(p);
	}
}
void heapify_down(int idx)
{
	int l = idx*2+1, r = idx*2+2;

	int maxi = idx;

	if (l < sz && cmp(heap[l], heap[maxi]))
		maxi = l;
	if (r < sz && cmp(heap[r], heap[maxi]))
		maxi = r;

	if (maxi != idx) {
		swap(heap[idx], heap[maxi]);
		swap(nds[heap[idx]].h, nds[heap[maxi]].h);
		heapify_down(maxi);
	}
}
void heap_insert(int u, double d)
{
	if (sz == 0) {
		nds[u].h = 0;
		nds[u].len = d;
		nds[u].dl = nds[dst]-nds[u];
		heap[0] = u;
		sz = 1;
	} else if (nds[u].h == -1) {
		heap[sz] = u;
		nds[u].h = sz;
		nds[u].len = d;
		nds[u].dl = nds[dst]-nds[u];
		heapify_up(sz);
		++sz;
	} else if (d < nds[u].len) {
		nds[u].len = d;
		heapify_up(nds[u].h);
	}
}
int heap_pop()
{
	int u = heap[0];
	nds[heap[0]].h = -1;
	heap[0] = heap[--sz];
	if (sz) {
		nds[heap[0]].h = 0;
		heapify_down(0);
	}
	return u;
}
void heap_clear()
{
	for (int i=0; i<sz; ++i)
		nds[heap[i]].h = -1;
	sz = 0;
}
bool heap_empty()
{
	return sz == 0;
}
// ------------------------------------------------------------------ //

int main()
{
	int N, M;
	scanf("%d%d", &N, &M);
	int n, x, y;
	for (int i=0; i<N; ++i) {
		scanf("%d%d%d", &n, &x, &y);
		nds[n].x = x;
		nds[n].y = y;
		nds[n].h = -1;
	}
	for (int i=0; i<M; ++i) {
		scanf("%d%d", &x, &y);
		nds[x].edge.push_back(y);
		nds[y].edge.push_back(x);
	}
	while (scanf("%d%d", &x, &y) == 2 && x != -1) {
		dst = y;
		heap_init();

		memset(marked, 0, sizeof(marked));
		marked[x] = true;
		for (It it=nds[x].edge.begin(); it != nds[x].edge.end(); ++it)
			if (!marked[*it]) {
				heap_insert(*it, nds[*it]-nds[x]);
			}
		while (!heap_empty()) {
			int u = heap_pop();
			marked[u] = true;
			if (u == y) {
				printf("%.1f\n", nds[u].len);
				heap_clear();
			} else {
				for (It it = nds[u].edge.begin(); it != nds[u].edge.end(); ++it)
					if (!marked[*it]) {
						double d = nds[u].len+(nds[*it]-nds[u]);
						heap_insert(*it, d);
					}
			}
		}
	}
	return 0;
}

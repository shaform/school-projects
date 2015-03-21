/**
 * -- hw5 version 3 --
 * The right-threaded splay tree version.
 * -------------------
 * I_refs	=??????????
 * m_total	=??????????
 * priority	=!!!!!!!!!!
 * -------------------
 */
#include <cstring>
//#include <cstdlib>
//#include <cassert>
//#include <cstdio>
#define USE_POOL
//#define USE_PTN
//#define USE_STATIC
namespace HOMEWORK
{
	using namespace std;
	const int SIZE = 128;
#if defined(USE_PTN) && defined(USE_POOL)
	const int BUF_SZ = 57001;
#else
	const int BUF_SZ = 57000;
#endif
	struct tKeyValuePack
	{
		char key[SIZE];
		char value[SIZE];
	};
	static tKeyValuePack set_kv(tKeyValuePack &kv, const char *k, const char *v, unsigned int size)
	{
		memcpy(kv.key, k, size);
		strcpy(kv.value, v);
	}
#if defined(USE_PTN) && defined(USE_POOL)
	struct Node;
	struct PtNode {
		unsigned int idx;
		Node *operator->();
		PtNode() {}
		PtNode(unsigned int i) : idx(i) {}
		operator const unsigned int&() { return idx; }
	};
	//static inline bool operator==(const PtNode &a, const PtNode &b) { return a.idx == b.idx; }
#endif
	struct Node {
#ifdef USE_PTN
		PtNode ln[2];
#else
		Node *ln[2];
#endif
		bool threaded;
		unsigned int size;
		tKeyValuePack data;
	};
	struct DummyNode {
#ifdef USE_PTN
		PtNode ln[2];
#else
		Node *ln[2];
#endif
		bool threaded;
	};
	// ------ The Memory Pool ------ //
#ifdef USE_STATIC
	static Node storage[BUF_SZ];
#else
	static Node *storage;
#endif
#if defined(USE_PTN) && defined(USE_POOL)
	inline Node *PtNode::operator->() { return &storage[idx]; }
#else
	typedef Node *PtNode;
#endif
#ifdef USE_POOL
	struct MemPool {
		PtNode head;
		PtNode acquire()
		{
			PtNode u = head;
			head = head->ln[1];
			u->threaded = false;
			return u;
		}
		void release(PtNode u)
		{
			u->ln[1] = head;
			head = u;
		}
		MemPool()
#ifdef USE_PTN
			: head(1)
#endif
		{
#ifndef USE_STATIC
			storage = static_cast<Node *>(operator new(sizeof(Node)*(BUF_SZ)));
#endif
#ifndef USE_PTN
			head = storage;
#endif
			int i=BUF_SZ;
#ifdef USE_PTN
			while (--i) storage[i-1].ln[1] = i;
#else
			while (--i) storage[i-1].ln[1] = &storage[i];
#endif
			storage[BUF_SZ-1].ln[1] = 0;
		}
		void destroy()
		{
#ifndef USE_STATIC
			if (storage) operator delete(storage);
			storage = 0;
#endif
		}
		~MemPool() { destroy(); }
	} pool;
#else
	struct MemPool {
		Node *acquire()
		{
			Node *u =static_cast<Node *>(operator new(sizeof(Node)));
			u->threaded = false;
			return u;
		}
		void release(Node *u) { operator delete(u); }
	} pool;
#endif
	// ------ The Splay Tree ------ //
	static int sz = 0;
	static PtNode root = 0, first = 0;
	static inline PtNode splay_next(PtNode u)
	{
		if (u->threaded) return u->ln[1];
		if (u = u->ln[1]) while (u->ln[0]) u = u->ln[0];
		return u;
	}
	static int splay(const char *key)
	{
		PtNode lroot = root;
#if defined(USE_PTN) && defined(USE_POOL)
		Node &nd = storage[0];
		PtNode l = 0, r = 0;
#else
		DummyNode nd;
		PtNode l = reinterpret_cast<Node *>(&nd), r = reinterpret_cast<Node *>(&nd);
#endif
		int cmp;
		while (true) {
			cmp = memcmp(key, lroot->data.key, lroot->size);
			if (cmp < 0) {
				PtNode left = lroot->ln[0];
				if (left == 0) break;
				if ((cmp = memcmp(key, left->data.key, left->size)) < 0) {
					if (left->threaded) {
						lroot->ln[0] = 0;
						left->threaded = false;
					} else lroot->ln[0] = left->ln[1];
					left->ln[1] = lroot;
					lroot = left;
					if ((left = lroot->ln[0]) == 0) break;
				}
				r->ln[0] = lroot;
				r = lroot;
				lroot = left;
			} else {
				if (cmp == 0) break;
				PtNode right;
				if (lroot->threaded || (right = lroot->ln[1]) == 0) break;
				if ((cmp = memcmp(key, right->data.key, right->size)) > 0) {
					PtNode lr = right->ln[0];
					if (lr) lroot->ln[1] = lr;
					else {
						lroot->ln[1] = right;
						lroot->threaded = true;
					}
					right->ln[0] = lroot;
					lroot = right;
					if (lroot->threaded || (right = lroot->ln[1]) == 0) break;
				}
				l->ln[1] = lroot;
				l->threaded = false;
				l = lroot;
				lroot = right;
			}
		}
		if (lroot->ln[0]) {
			l->ln[1] = lroot->ln[0];
			l->threaded = false;
		} else {
			l->ln[1] = lroot;
			l->threaded = true;
		}
		if (lroot->threaded) r->ln[0] = 0;
		else r->ln[0] = lroot->ln[1];
		if (nd.threaded) lroot->ln[0] = 0;
		else lroot->ln[0] = nd.ln[1];
		lroot->ln[1] = nd.ln[0];
		lroot->threaded = false;
		root = lroot;
		return cmp;
	}
	static inline PtNode splay_find_then_splay_res(const char *key, bool left)
	{
		PtNode lroot = root;
		if (!lroot) return 0;
		int cmp;
		PtNode x;
		if ((cmp = memcmp(key, lroot->data.key, lroot->size) < 0)) {
			if (lroot) {
				// zig-zag
				if (x->threaded) {
					lroot->ln[0] = 0;
					x->threaded = false;
				} else lroot->ln[0] = x->ln[1];
				x->ln[1] = lroot;
				return root = x;
			}
		} else {
			if (cmp == 0) return 0;
			if (lroot) {
				// zig-zag
				lroot->ln[1] = x->ln[0];
				x->ln[0] = lroot;
				return root = x;
				// zig-zig
			}
		}
		return 0;
	}
	static inline PtNode splay_find_then_splay(const char *key)
	{
		PtNode lroot = root;
		if (!lroot) return 0;
		int cmp;
		PtNode x;
		if ((cmp = memcmp(key, lroot->data.key, lroot->size) < 0)) {
			if (lroot) {
				// zig-zag
				if (x->threaded) {
					lroot->ln[0] = 0;
					x->threaded = false;
				} else lroot->ln[0] = x->ln[1];
				x->ln[1] = lroot;
				return root = x;
			}
		} else {
			if (cmp == 0) return 0;
			if (lroot) {
				// zig-zag
				lroot->ln[1] = x->ln[0];
				x->ln[0] = lroot;
				return root = x;
				// zig-zig
			}
		}
		return 0;
	}
	static inline PtNode splay_find(const char *key)
	{
		if (!root || splay(key)) return 0;
		return root;
	}
	/*
	static inline PtNode splay_find_basic(const char *key)
	{
		PtNode it = root;
		int di;
		while (it) {
			int c = strcmp(key, it->data.key);
			if (c<0)
				it = it->ln[0];
			else {
				if (c == 0) return it;
				if (it->threaded) return 0;
				it = it->ln[1];
			}
		}
		return 0;
	}
	*/

	// ------ The Container API ------ //
	bool Insert(const char* key, const char* value)
	{
		if (root) {
			int cmp = splay(key);
			if (cmp == 0) return false;
			PtNode u = pool.acquire();
			u->size = (strlen(key)+1)*sizeof(char);
			set_kv(u->data, key, value, u->size);
			PtNode lroot = root;
			if (cmp < 0) {
				PtNode t = lroot->ln[0];
				u->ln[1] = lroot;
				if (u->ln[0] = t) {
					while (!t->threaded && t->ln[1]) t = t->ln[1];
					t->ln[1] = u;
					t->threaded = true;
				} else first = u;
				lroot->ln[0] = 0;
			} else {
				u->ln[0] = lroot;
				u->ln[1] = lroot->ln[1];
				lroot->ln[1] = u;
				lroot->threaded = true;
			}
			root = u;
			++sz;
			return true;
		} else {
			root = first = pool.acquire();
			root->ln[0] = root->ln[1] = 0;
			root->size = (strlen(key)+1)*sizeof(char);
			set_kv(root->data, key, value, root->size);
			sz = 1;
			return true;
		}
	}
	bool Find(const char* key, char* value)
	{
		PtNode it = splay_find(key);
		if (it) {
			strcpy(value, it->data.value);
			return true;
		}
		return false;
	}
	bool Erase(const char *key) {
		if (splay_find_then_splay(key)) {
			PtNode u = root;
			PtNode right = u->ln[1], left = u->ln[0];
			if (left) {
				if (left->threaded)
					root = left;
				if (!left->threaded) {
					PtNode v = left, w = left;
					while (!v->threaded) {
						w = v;
						v = v->ln[1];
					}
					if (v->ln[0]) w->ln[1] = v->ln[0];
					else w->threaded = true;
					root = v;
					v->ln[0] = left;
				}
				root->ln[1] = right;
				root->threaded = false;
			} else {
				root = right;
				first = splay_next(u);
			}
			--sz;
			pool.release(u);
			return true;
		} else return false;
	}
	int Enumerate(tKeyValuePack* key_value_in_order, int N_Max, const char* szLowKey=0, const char* szHighKey=0)
	{
		PtNode l = first, r = 0;
		if (szLowKey) l = splay_find(szLowKey);
		if (szHighKey) r = splay_next(splay_find(szHighKey));
		int t = 0;
		while (t<N_Max && l != r) {
			memcpy(key_value_in_order[t].key, l->data.key, l->size);
			//strcpy(key_value_in_order[t].key, l->data.key);
			strcpy(key_value_in_order[t].value, l->data.value);
			++t;
			l = splay_next(l);
		}
		return t;
	}
	int GetSize() { return sz; }
	void Clear()
	{
#ifdef USE_POOL
		pool.destroy();
#else
		while (first) {
			PtNode t = splay_next(first);
			pool.release(first);
			first = t;
		}
#endif
		root = 0;
		sz = 0;
	}
}

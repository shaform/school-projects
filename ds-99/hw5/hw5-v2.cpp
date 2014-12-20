/**
 * -- hw5 version 2 --
 * This version uses a threaded binary tree
 * to keep all records.
 *
 * !! This version is abandoned !!
 * -------------------
 * I_refs	=xxxx
 * m_total	=xxxx
 * priority	=xxxx
 * -------------------
 */
#include <cstring>
#include <cstdlib>

//#define USE_POOL


namespace HOMEWORK
{
	using namespace std;
	// //
	const int SIZE = 128;
	const int BUF_SZ = 57000;
	enum {L, R};
	struct tKeyValuePack
	{
		char key[SIZE];
		char value[SIZE];
	};

	static tKeyValuePack set_kv(tKeyValuePack &kv, const char *k, const char *v)
	{
		strcpy(kv.key, k);
		strcpy(kv.value, v);
	}

	struct Node {
		Node *ln[2];
		bool thread;
		tKeyValuePack data;
	};

	Node *root = 0;

	int sz = 0;

	// ------ The Memory Pool ------ //

#ifdef USE_POOL
	struct MemPool {
		Node *pool;
		Node *head;

		Node *acquire()
		{
			Node *u = head;
			head = head->ln[1];
			u->ln[0] = u->ln[1] = 0;
			u->thread = true;
			return u;
		}
		void release(Node *u)
		{
			u->ln[1] = head;
			head = u;
		}

		void release_all()
		{
			for (int i=0; i<BUF_SZ; ++i)
				pool[i].ln[1] = &pool[i+1];

			pool[BUF_SZ-1].ln[1] = 0;
			head = pool;
		}

		MemPool()
			: pool(static_cast<Node *>(operator new(sizeof(Node)*BUF_SZ))), head(pool)
		{
			release_all();
		}
		~MemPool() { operator delete(pool); }
	};
#else
	struct MemPool {
		Node *acquire()
		{
			Node *u =static_cast<Node *>(operator new(sizeof(Node)));
			u->ln[0] = u->ln[1] = 0;
			u->thread = true;
			return u;
		}
		void release(Node *u) { operator delete(u); }
	};
#endif
	MemPool pool;

	// ------ The Container ------ //

	bool Insert(const char* key, const char* value)
	{
		if (root) {
			Node *it = root, *nd;
			int di;
			while (true) {
				int c = strcmp(key, it->data.key);

				if (c == 0)
					return false;
				else if (c>0) {
					di = 1;
					if (it->thread) break;
				} else {
					di = 0;
					if (it->ln[0] == 0)
						break;
				}
				it = it->ln[di];
			}
			nd = pool.acquire();
			set_kv(nd->data, key, value);

			if (di) {
				nd->ln[1] = it->ln[1];
				it->thread = false;
			} else
				nd->ln[1] = it;
			it->ln[di] = nd;
		} else {
			root = pool.acquire();
			set_kv(root->data, key, value);
		}
		++sz;
		return true;
	}

	bool Find(const char* key, char* value)
	{
		Node *it = root;
		int di;
		while (it) {
			int c = strcmp(key, it->data.key);

			if (c == 0) {
				strcpy(value, it->data.value);
				return true;
			} else if (c>0) {
				di = 1;
				if (it->thread) return false;
			} else {
				di = 0;
			}
			it = it->ln[di];
		}
		return false;
	}

	bool Erase(const char *key)
	{
		Node *u, *v;
		int di, cmp;

		Node nd;
		nd.ln[0] = root;

		if (root) {
			u = root;
			v = &nd;
			di = 0;

			while (true) {
				cmp = strcmp(key, u->data.key);
				if (cmp == 0) break;
				di = cmp > 0 ? 1 : 0;

				if (di)  {
					if (u->thread) return false;
				} else if (u->ln[0] == 0) return false;

				v = u;
				u = u->ln[di];
			}

			if (u->ln[0]) {
				Node *t = u->ln[0];
				if (t->thread) {
					t->ln[1] = u->ln[1];
					t->thread = u->thread;
					v->ln[di] = t;
				} else {
					Node *w;
					while (true) {
						w = t->ln[1];
						if (w->thread)
							break;
						t = w;
					}

					if (w->ln[0])
						t->ln[1] = w->ln[0];
					else {
						t->thread = true;
						t->ln[1] = w;
					}

					w->ln[0] = u->ln[0];
					w->ln[1] = u->ln[1];
					w->thread = u->thread;
					v->ln[di] = w;
				}
			} else if (u->thread) {
				v->ln[di] = u->ln[di];
				if (di) v->thread = true;
			} else {
				v->ln[di] = u->ln[1];
			}

			root = nd.ln[0];
			pool.release(u);
			--sz;
			return true;
		}
		return false;
	}

	static Node *find_first()
	{
		if (root) {
			Node *u = root;
			while (u->ln[0])
				u = u->ln[0];
			return u;
		} else
			return 0;
	}

	static Node *find_bst(const char *key)
	{
		Node *it = root;
		int di;
		while (it) {
			int c = strcmp(key, it->data.key);

			if (c == 0) {
				return it;
			} else if (c>0) {
				di = 1;
				if (it->thread) return false;
			} else {
				di = 0;
			}
			it = it->ln[di];
		}
		return 0;
	}

	static Node *find_next(Node *u)
	{
		if (u) {
			if (u->thread)
				return u->ln[1];
			else {
				u = u->ln[1];
				while (u->ln[0])
					u = u->ln[0];
				return u;
			}

		} else
			return find_first();
	}


	int Enumerate(tKeyValuePack* key_value_in_order, int N_Max, const char* szLowKey =0, const char* szHighKey =0)
	{

		Node *l, *r;
		if (szLowKey) l = find_bst(szLowKey);
		else l = find_first();
		if (szHighKey) r = find_bst(szHighKey);
		else r = 0;
		if (r) r = r->ln[1];

		int t = -1;
		while (l != r && ++t<N_Max) {
			strcpy(key_value_in_order[t].key, l->data.key);
			strcpy(key_value_in_order[t].value, l->data.value);

			l = find_next(l);
		}

		return t+1;
	}

	int GetSize()
	{
		return sz;
	}

	void Clear()
	{
		// release the memory
#ifdef USE_POOL
		pool.release_all();
#else
		Node *it = find_first();
		while (it) {
			Node *t = find_next(it);
			pool.release(it);
			it = t;
		}
#endif

		// reset the binary search tree
		root = 0;
		sz = 0;
	}
}

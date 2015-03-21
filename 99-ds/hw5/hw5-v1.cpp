/**
 * -- hw5 version 1 --
 * This version uses a simple array to keep
 * all records.
 * -------------------
 * I_refs	=4439974251
 * m_total	=14713945
 * priority	=0.258
 * -------------------
 */
#include <cstring>
//#include <cstdio>
#include <cstdlib>
#define USE_POOL
//#define CACHE
namespace HOMEWORK
{
	using namespace std;
#ifdef USE_POOL
	typedef unsigned short IDX;
#endif
	const int SIZE = 128;
	const int BUF_SZ = 57000;
#ifdef CACHE
	const int CACHE_SZ = 5500;
#endif
	struct tKeyValuePack
	{
		char key[SIZE];
		char value[SIZE];
		tKeyValuePack() { }
		tKeyValuePack(const char *k, const char *v)
		{
			//memset(this, 0, sizeof(*this));
			strcpy(key, k);
			strcpy(value, v);
		}
	};
	static inline void set_kv(tKeyValuePack &kv, const char *key, const char *value)
	{
			strcpy(kv.key, key);
			strcpy(kv.value, value);
	}


#ifdef USE_POOL
	IDX buf[BUF_SZ];
	struct MemPool {
		tKeyValuePack *pool;
		MemPool() : pool(static_cast<tKeyValuePack *>(operator new(sizeof(tKeyValuePack)*BUF_SZ)))
		{
			for (IDX i=0; i<BUF_SZ; ++i)
				buf[i] = i;
		}
		~MemPool() { operator delete(pool); }
	} pool;
	static inline tKeyValuePack &get_item(int idx)
	{
		return pool.pool[buf[idx]];
	}
#else
	tKeyValuePack *buf[BUF_SZ];
	static inline tKeyValuePack &get_item(int idx)
	{
		return *buf[idx];
	}
#endif
	int sz = 0;

#ifdef CACHE
	tKeyValuePack *cache[CACHE_SZ];

	static int hash(const char *str)
	{
		unsigned long hash = 5381;
		int c;

		while (c = *str++)
			hash = ((hash << 5) + hash) + c;

		return hash % CACHE_SZ;
	}
#endif

	static bool bsearch(const char *key, int &idx, int l = 0, int r = sz)
	{
		idx = 0;
		while (l<r)
		{
			idx = (l+r)/2;
			int t = strcmp(key, get_item(idx).key);
			if (t<0) r = idx;
			else if (t>0) l = ++idx;
			else break;
		}
		return l < r;
	}



	bool Insert(const char* key, const char* value)
	{
		int it;
		if (bsearch(key, it))
			return false;
		else {
			/*
			   for (int i=0; i<sz; ++i) {
			   if (buf[i] == 0) {
			   printf("before %d %d %d\n", i, sz, it);
			   abort();
			   }
			   }
			   */

#ifdef USE_POOL
			IDX item;
#endif
			if (it != sz) {
#ifdef USE_POOL
				item = buf[sz];
#endif
				memmove(buf+it+1, buf+it, (sz-it)*sizeof(buf[0]));
#ifdef USE_POOL
				buf[it] = item;
#endif
			}

#ifdef USE_POOL
			set_kv(get_item(it), key, value);
#else
			buf[it] = new tKeyValuePack(key, value);
#endif
			++sz;
			/*
			   for (int i=0; i<sz; ++i) {
			   if (buf[i] == 0) {
			   printf("after %d %d %d\n", i, sz, it);
			   abort();
			   }
			   }
			   */
			return true;
		}
	}

	bool Find(const char* key, char* value)
	{
#ifdef CACHE
		int h = hash(key);
		if (cache[h] && strcmp(cache[h]->key, key) == 0) {
			strcpy(value, cache[h]->value);
			//printf("in cache!!\n");
			return true;
		}
		//printf("no cache!!\n");
#endif
		//printf("testc!!\n");

		int it;
		if (bsearch(key, it)) {
#ifdef CACHE
			cache[h] = &get_item(it);
#endif
			strcpy(value, get_item(it).value);
			//memcpy(value, buf[it]->value, sizeof(buf[it]->value));
			return true;
		} else
			return false;

	}

	bool Erase(const char* key)
	{
#ifdef CACHE
		int h = hash(key);
		if (cache[h] && strcmp(cache[h]->key, key) == 0)
			cache[h] = 0;
#endif

		int it;
		if (bsearch(key, it)) {
#ifdef USE_POOL
			IDX item = buf[it];
#else
			delete buf[it];
#endif
			--sz;

			if (it != sz) {
				memmove(buf+it, buf+it+1, (sz-it)*sizeof(buf[0]));
#ifdef USE_POOL
				buf[sz] = item;
#endif
			}

			return true;
		} else
			return false;
	}




	int Enumerate(tKeyValuePack* key_value_in_order, int N_Max, const char* szLowKey =0, const char* szHighKey =0)
	{
		int l=0, r=sz-1, t=0;
		if (szLowKey) bsearch(szLowKey, l);
		if (szHighKey) bsearch(szHighKey, r, l);

		while (l<=r && t<N_Max) {
			//key_value_in_order[t] = *buf[l];
			strcpy(key_value_in_order[t].key, get_item(l).key);
			strcpy(key_value_in_order[t].value, get_item(l).value);


			++l;
			++t;
		}
		return t;
	}

	int GetSize()
	{
		return sz;
	}

	void Clear()
	{
#ifdef USE_POOL
		sz = 0;
#else
		while (sz) delete buf[--sz];
#endif

#ifdef CACHE
		memset(cache, 0, sizeof(cache));
#endif
	}
#ifdef CACHE
	/*
	void check_cache()
	{
		int empty = 0;
		for (int i=0; i<CACHE_SZ; ++i)
			if (cache[i] == 0)
				++empty;
		printf("%d cache empty\n", empty);
	}
	*/
#endif
}

/**
 * -- hw2 version 4 --
 * This version uses hash to split free lists.
 * By carefully selecting the hash,
 * it successfully surpasses the performance of
 * the Buddy System slightly in these test cases.
 * 
 * -------------------
 * I_refs	=3542760
 * m_total	=10937
 * priority	=5.171
 * -------------------
 */
#pragma once

namespace HOMEWORK
{
	typedef unsigned long long addr_t;
	typedef unsigned long long sz_t;
	typedef addr_t ptn;


	// Node management
	struct header { sz_t sz, up, down, larger, lesser; }; 
	

	const int HEADER = sizeof(header);
	const int FOOTER = sizeof(sz_t);
	const int DATA = HEADER + FOOTER;
	const int BOUND = DATA;
	const int LOW = BOUND;
	const ptn ACQUIRED = 0u;
	const ptn NOTINLIST = ACQUIRED-1u;
	const ptn BOTTOM = NOTINLIST-1u;

	// hash
	const int BUF_SZ = 7;

	//3560000
	sz_t POW2[BUF_SZ]  = {0, 100, 200, 300, 500, 1500, 5000}; // 3542760


	/*
	static void init_hash()
	{
		int sz = DATA;
		for (int i=0; i<BUF_SZ; ++i)
			POW2[i] = (sz*=4);
	}
	*/

	static int hash(sz_t sz)
	{
		int i;
		for (i=BUF_SZ-1; POW2[i] > sz; --i);

		return i;
	}

	ptn root[BUF_SZ], tail;

	static ptn get_ptn(void *addr) { return ((sz_t ) addr) - HEADER; }
	static void* get_mem(ptn u) { return (void *) (u + HEADER); }
	static sz_t &get_size(ptn ptr) { return *((sz_t *) ptr); }
	static ptn &get_down(ptn ptr) { return ((header *) ptr)->down; }
	static ptn &get_up(ptn ptr) { return ((header *) ptr)->up; }
	static ptn &get_larger(ptn ptr) { return ((header *) ptr)->larger; }
	static ptn &get_lesser(ptn ptr) { return ((header *) ptr)->lesser; }
	static void set_second_size(ptn ptr) {
		sz_t sz = get_size(ptr);
		*((sz_t *) (ptr + HEADER + sz)) = sz;
	}
	static ptn get_next(ptn u) { return u + DATA + get_size(u); }
	static ptn get_prev(ptn u) { return u - *((sz_t *) (u-FOOTER)) - DATA; }


	static void initialize(ptn u, sz_t sz)
	{
		get_size(u) = sz;
		set_second_size(u);
	}


	// Free list
	
	static bool is_top(ptn u) { return get_up(u) == BOTTOM; }

	static void pop(ptn u)
	{
		ptn &t = get_down(u);
		if (t != BOTTOM) get_up(t) = BOTTOM;

		ptn larger = get_larger(u), lesser = get_lesser(u);
		if (t == BOTTOM) {
			get_larger(lesser) = larger;
			get_lesser(larger) = lesser;
		} else {

			get_larger(t) = larger;
			get_lesser(t) = lesser;

			get_larger(lesser) = t;
			get_lesser(larger) = t;
		}
		t = NOTINLIST;
	}


	static void delete_lst(ptn u)
	{
		if (is_top(u))
			pop(u);
		else {
			ptn up = get_up(u), down = get_down(u);

			get_down(up) = down;

			if (down != BOTTOM)
				get_up(down) = up;


			get_down(u) = NOTINLIST;
		}
	}
	static void insert_lst(ptn u, int h)
	{
		sz_t sz = get_size(u), itsz;
		ptn it = get_lesser(root[h]);

		while ((itsz = get_size(it)) > sz)
			it = get_lesser(it);


		if (itsz == sz) {
			ptn nd = get_down(it);

			get_up(u) = it;
			get_down(u) = nd;

			get_down(it) = u;

			if (nd!=BOTTOM)
				get_up(nd) = u;
		} else {
			ptn larger = get_larger(it);;
			get_lesser(u) = it;
			get_larger(u) = larger;


			
			get_larger(it) = u;
			get_lesser(larger) = u;
			get_down(u) = BOTTOM;
			get_up(u) = BOTTOM;

		}
	}

	// Free list
	static ptn merge(ptn u, ptn v)
	{
		// Assuming that u's data is right & u, v are not in the list.
		initialize(u, get_size(u) + get_size(v) + DATA);

		return u;
	}
	static void delete_f(ptn u) { get_down(u) = ACQUIRED; }
	static bool is_free(ptn u) { return get_down(u) != ACQUIRED; }
	static bool in_lst(ptn u) { return get_down(u) != NOTINLIST; }


	static ptn insert_f(ptn u)
	{
		get_down(u) = NOTINLIST;

		ptn s = get_prev(u);

		if (is_free(s)) {
			if (in_lst(s))
				delete_lst(s);

			u = merge(s, u);
		}

		s = get_next(u);

		if (is_free(s)) {
			if (in_lst(s))
				delete_lst(s);

			u = merge(u, s);
		}

		return u;
	}


	static void initialize_pool(void* addr, sz_t nSize)
	{
		//init_hash();

		nSize -= DATA*(BUF_SZ+2);

		ptn it = (ptn ) addr;
		//Initialize root
		for (int i=0; i<BUF_SZ; ++i) {
			root[i] = it;
			initialize(root[i], 0);
			get_down(root[i]) = ACQUIRED;

			get_larger(root[i]) = root[i];
			get_lesser(root[i]) = root[i];

			it = get_next(it);
		}

		//Initialize memory pool
		ptn pool = it;
		initialize(pool, nSize);
		insert_lst(pool, hash(nSize));

		//Initialize tail
		tail = get_next(pool);
		initialize(tail, 0);
		get_down(tail) = ACQUIRED;
	}

	static void* acquire(unsigned long long nSize)
	{

		int h = hash(nSize);

		ptn it;

		it = get_lesser(root[h]);

		while (get_size(it) >= nSize)
			it = get_lesser(it);

		it = get_larger(it);
		/*
		it = get_lesser(root[h]);
		if (get_size(it) < nSize)
			it = root[h];
		 */

		while (it == root[h] && ++h<BUF_SZ)
			it = get_larger(root[h]);

		if (h==BUF_SZ) return 0;


		sz_t sz = get_size(it);

		pop(it);
		delete_f(it);

		// Split the memory
		if (sz > nSize+DATA+BOUND) {
			initialize(it, nSize);

			ptn t = get_next(it);

			sz -= nSize + DATA;
			h = hash(sz);

			initialize(t, sz);

			t = insert_f(t);

			insert_lst(t, h);
		}

		return get_mem(it);
	}
	static void release(void* addr)
	{
		ptn it = get_ptn(addr);

		it = insert_f(it);

		get_down(it) = NOTINLIST;

		int sz = get_size(it);
		if (sz > BOUND)
			insert_lst(it, hash(sz));
	}
}

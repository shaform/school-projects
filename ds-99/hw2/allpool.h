#pragma once

namespace HOMEWORK
{
	typedef unsigned long long addr_t;
	typedef unsigned long long sz_t;
	typedef addr_t ptn;


	// Node management
	struct header { sz_t sz, up, down, larger, lesser; }; 
	
	const size_t HEADER = sizeof(header);
	const size_t FOOTER = sizeof(sz_t);
	const size_t DATA = HEADER + FOOTER;
	const size_t BOUND = DATA;
	const size_t LOW = BOUND;
	const ptn ACQUIRED = 0u;
	const ptn NOTINLIST = ACQUIRED-1u;

	ptn root, tail;

	inline ptn get_ptn(void *addr) { return ((sz_t ) addr) - HEADER; }
	inline void* get_mem(ptn u) { return (void *) (u + HEADER); }
	inline sz_t &get_size(ptn ptr) { return *((sz_t *) ptr); }
	inline ptn &get_down(ptn ptr) { return ((header *) ptr)->down; }
	inline ptn &get_up(ptn ptr) { return ((header *) ptr)->up; }
	inline ptn &get_larger(ptn ptr) { return ((header *) ptr)->larger; }
	inline ptn &get_lesser(ptn ptr) { return ((header *) ptr)->lesser; }
	inline void set_second_size(ptn ptr) {
		sz_t sz = get_size(ptr);
		*((sz_t *) (ptr + HEADER + sz)) = sz;
	}
	inline ptn get_next(ptn u) { return u + DATA + get_size(u); }
	inline ptn get_prev(ptn u) { return u - *((sz_t *) (u-FOOTER)) - DATA; }

#if DEBUG
#include <cstdio>
	void check_list()
	{
		using namespace std;
		ptn it = root;
		while (it != tail) {
			printf("#%llu : %llu\n", it, get_size(it));
			it = get_next(it);
		}
		it = get_larger(root);
		while (it != root) {
			ptn it2 = it;
			printf("#size: %llu\n", get_size(it));
			while (it2 != root) {
				printf("#%llu : %llu\n", it2, get_size(it2));
				it2 = get_down(it2);
			}
			it = get_larger(it);
		}

	}
#endif

	inline void initialize(ptn u, sz_t sz)
	{
		get_size(u) = sz;
		set_second_size(u);
	}


	// Free list
	
	inline bool is_top(ptn u) { return get_up(u) == root; }

	inline void pop(ptn u)
	{
		ptn &t = get_down(u);
		get_up(t) = root;
		t = NOTINLIST;

		ptn larger = get_larger(u), lesser = get_lesser(u);
		if (get_up(u) == root) {
			get_larger(lesser) = larger;
			get_lesser(larger) = lesser;
		} else {

			get_larger(t) = larger;
			get_lesser(t) = lesser;

			get_larger(lesser) = t;
			get_lesser(larger) = t;
		}
	}


	inline void delete_lst(ptn u)
	{
		if (is_top(u))
			pop(u);
		else {
			ptn up = get_up(u), down = get_down(u);

			get_down(up) = down;

			if (down != root)
				get_up(down) = up;


			get_down(u) = NOTINLIST;
		}
	}
	inline void insert_lst(ptn u)
	{
		sz_t sz = get_size(u), itsz;
		ptn it = get_lesser(root);

		while ((itsz = get_size(it)) > sz)
			it = get_lesser(it);


		if (itsz == sz) {
			ptn nd = get_down(it);

			get_up(u) = it;
			get_down(u) = nd;

			get_down(it) = u;

			if (nd!=root)
				get_up(nd) = u;
		} else {
			ptn larger = get_larger(it);;
			get_lesser(u) = it;
			get_larger(u) = larger;


			
			get_larger(it) = u;
			get_lesser(larger) = u;
			get_down(u) = root;
			get_up(u) = root;

		}
	}

	// Free list
	inline ptn merge(ptn u, ptn v)
	{
		// Assuming that u's data is right & u, v are not in the list.
		initialize(u, get_size(u) + get_size(v) + DATA);

		return u;
	}
	inline void delete_f(ptn u) { get_down(u) = ACQUIRED; }
	inline bool is_free(ptn u) { return get_down(u) != ACQUIRED; }
	inline bool in_lst(ptn u) { return get_down(u) != NOTINLIST; }


	inline ptn insert_f(ptn u)
	{
		get_down(u) = root;

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


	inline void initialize_pool(void* addr, sz_t nSize)
	{

		nSize -= DATA*3;

		//Initialize root
		root = (addr_t ) addr;
		initialize(root, 0);
		get_down(root) = ACQUIRED;


		get_larger(root) = root;
		get_lesser(root) = root;

		//Initialize memory pool
		ptn pool = get_next(root);
		initialize(pool, nSize);

		insert_lst(pool);


		//Initialize tail
		tail = get_next(pool);
		initialize(tail, 0);
		get_down(tail) = ACQUIRED;



	}

	inline void* acquire(unsigned long long nSize)
	{

		ptn it = get_lesser(root);

		sz_t sz;
		while ((sz = get_size(it)) > nSize)
			it = get_lesser(it);

		it = get_larger(it);

		if (it == root)
			return 0;

		sz = get_size(it);

		pop(it);
		delete_f(it);


		// Split the memory
		if (sz > nSize+DATA+BOUND) {
			initialize(it, nSize);

			ptn t = get_next(it);
			initialize(t, sz-nSize-DATA);
			get_down(t) = NOTINLIST;

			t = insert_f(t);

			if (get_size(t) > BOUND)
				insert_lst(t);
		}

		return get_mem(it);
	}





	inline void release(void* addr)
	{
		ptn it = get_ptn(addr);

		it = insert_f(it);

		if (get_size(it) > BOUND)
			insert_lst(it);
	}

}

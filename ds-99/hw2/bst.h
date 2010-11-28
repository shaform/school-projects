/**
 * -- hw4 version 3 --
 * Taking advantage of the original design,
 * this version uses a binary search tree instead of
 * a list to keep track of free memory.
 *
 *
 * 
 * -------------------
 * I_refs	=
 * m_total	=
 * priority	=
 * -------------------
 */
#include <cstdio>
using namespace std;
#pragma once

namespace HOMEWORK
{
	typedef char *addr_t;
	typedef unsigned long long sz_t;


	// node management
	struct header { sz_t sz; header *up, *down, *larger, *lesser; }; 
	typedef header *ptn;
	
	const int HEADER = sizeof(header);
	const int FOOTER = sizeof(sz_t);
	const int DATA = HEADER + FOOTER;
	const int BOUND = DATA;
	const int LOW = BOUND;
	const ptn ACQUIRED = 0u;
	const ptn NOTINLIST = ACQUIRED-1u;

	ptn root, tail;

	static ptn get_ptn(void *addr) { return (ptn )((addr_t ) addr - HEADER); }
	static void* get_mem(ptn u) { return (void *) ((addr_t )u + HEADER); }
	static void set_second_size(ptn ptr) {
		sz_t sz = ptr->sz;
		*((sz_t *) ((addr_t )ptr + HEADER + sz)) = sz;
	}
	static ptn get_next(ptn u) { return (ptn )((addr_t )u + DATA + u->sz); }
	static ptn get_prev(ptn u) { return (ptn )((addr_t) u - *((sz_t *) ((addr_t) u-FOOTER)) - DATA); }


	// memory pool list
	static void initialize(ptn u, sz_t sz)
	{
		u->sz = sz;
		set_second_size(u);
	}


	/*
	void res_check(ptn u)
	{
		if (u == root)
			return;
		res_check(u->lesser);
		ptn it = u;
		do {
			it = it->down;
		} while (it != root);

		res_check(u->larger);
	}
	void check_bst()
	{
		res_check(root->up);
	}
	void res_print(ptn u)
	{
		if (u == root)
			return;
		res_print(u->lesser);
		ptn it = u;
		do {
			printf("%llu\n", it->sz);
			sz_t t = it->sz;
			it->sz = t;
			it = it->down;
		} while (it != root);

		res_print(u->larger);
	}
	void print_bst()
	{
		res_print(root->up);
	}
	void check_down(ptn u)
	{
		if (u != root && u->down == 0) {
			abort();
		}
	}
	*/
	// free node BST
	
	static bool is_top(ptn u) { return u->up->down != u; }
	static ptn &find_pc(ptn u)
	{
		ptn t = u->up;
		if (t == root)
			return root->up;
		else {
			if (t->sz > u->sz)
				return t->lesser;
			else
				return t->larger;
		}
	}
	static ptn delete_min(ptn u)
	{
		ptn t = u;
		while (u->lesser != root)
			u = u->lesser;

		if (t != u) {
			u->up->lesser = u->larger;

			if (u->larger != root)
				u->larger->up = u->up;
		}

		//check_bst();
		return u;
	}

	// done
	static void pop(ptn u)
	{
		ptn &t = u->down;

		ptn larger = u->larger, lesser = u->lesser;
		ptn &pc = find_pc(u);
		if (t == root) {
			/* delete the super node */
			if (larger == root) {
				if (lesser == root) {
					pc = root;
					//t = NOTINLIST;
					//check_bst();
				} else {
					pc = lesser;
					lesser->up = u->up;
					//t = NOTINLIST;
					//check_bst();
				}
			} else if (lesser == root) {
				pc = larger;
				larger->up = u->up;
				//t = NOTINLIST;
				//check_bst();
			} else {
				pc = delete_min(larger);
				lesser = pc->lesser = u->lesser;
				lesser->up = pc;


				if (pc != larger) {
					larger = pc->larger = u->larger;
					larger->up = pc;
				}

				pc->up = u->up;
				//t = NOTINLIST;
				//check_bst();
			}
		} else {
			pc = t;
			t->up = u->up;

			t->larger = larger;
			t->lesser = lesser;


			if (lesser != root)
				lesser->up = t;
			if (larger != root)
				larger->up = t;

			//t = NOTINLIST;
			//check_bst();
		}
		//check_bst();
		//u->larger = u->lesser = u->up = root;
		t = NOTINLIST;
		//check_bst();
	}


	// done
	static void delete_bst(ptn u)
	{
		if (is_top(u))
			pop(u);
		else {
			ptn up = u->up, down = u->down;

			up->down = down;

			if (down != root)
				down->up = up;


			u->down = NOTINLIST;
		}
		//check_bst();
	}

	// done
	static void insert_bst(ptn u)
	{
		/* the first node */
		if (root->up == root) {
			root->up = u;
			u->lesser = u->larger = u->down = u->up = root;
			return;
		}


		/* insert into the tree */

		sz_t sz = u->sz, itsz;
		ptn *ptp = &root->up, prev = root;

		/* find the place to insert */
		do {
			ptn t = *ptp;
			itsz = t->sz;
			if (sz > itsz)
				ptp = &t->larger;
			else if (sz < itsz)
				ptp = &t->lesser;
			else
				break;
			prev = t;

		} while (*ptp != root);

		/* do the insert */
		if (sz == itsz) {
			ptn it = *ptp;
			ptn nd = it->down;
			u->up = it;
			u->down = nd;
			it->down = u;
			if (nd != root)
				nd->up = u;
			//check_down(*ptp);
		} else {
			*ptp = u;
			u->up = prev;
			u->larger = u->lesser = u->down = root;
			//check_down(prev);
			//check_down(u);
		}
		//check_bst();
	}

	// Free list
	static ptn merge(ptn u, ptn v)
	{
		// Assuming that u's data is right & u, v are not in the list.
		initialize(u, u->sz + v->sz + DATA);

		return u;
	}
	static void delete_f(ptn u) { u->down = ACQUIRED;
		//check_bst();
	}
	static bool is_free(ptn u) { return u->down != ACQUIRED; }
	static bool in_lst(ptn u) { return u->down != NOTINLIST; }


	static ptn insert_f(ptn u)
	{

		ptn s = get_prev(u);

		if (is_free(s)) {
			if (in_lst(s))
				delete_bst(s);

			u = merge(s, u);
		}

		s = get_next(u);

		if (is_free(s)) {
			if (in_lst(s))
				delete_bst(s);

			u = merge(u, s);
		}

		u->down = NOTINLIST;

		//check_bst();
		return u;
	}


	inline void initialize_pool(void* addr, sz_t nSize)
	{

		nSize -= DATA*3;

		//Initialize root
		root = (ptn ) addr;
		initialize(root, 0);
		root->down = ACQUIRED;
		root->up = root;



		//Initialize memory pool
		ptn pool = get_next(root);
		initialize(pool, nSize);

		insert_bst(pool);


		//Initialize tail
		tail = get_next(pool);
		initialize(tail, 0);
		tail->down = ACQUIRED;


	}

	inline void* acquire(unsigned long long nSize)
	{

		ptn it = root->up, got = root;

		/* search for the place to delete */
		while (it != root) {
			if (it->sz >= nSize) {
				got = it;
				it = it->lesser;
				break;
			} else
				it = it->larger;
		}

		if (got == root)
			return 0;

		sz_t sz = got->sz;

		delete_bst(got);
		delete_f(got);


		// Split the memory
		if (sz > nSize+DATA+BOUND) {
			initialize(got, nSize);

			ptn t = get_next(got);
			initialize(t, sz-nSize-DATA);
			t->down = NOTINLIST;

			t = insert_f(t);

			insert_bst(t);
		}
		//print_bst();

		//check_bst();
		return get_mem(got);
	}




	inline void release(void* addr)
	{
		ptn it = get_ptn(addr);
		it = insert_f(it);

		if (it->sz > BOUND)
			insert_bst(it);
		//check_bst();
		//print_bst();
	}
}

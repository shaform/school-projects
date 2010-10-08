#pragma once

namespace HELPER
{
}

namespace HOMEWORK
{
	typedef unsigned long long sz_t;
	typedef unsigned long long sz_f;
	const size_t SIZE = sizeof(sz_f);

	inline void set_size(sz_t ptr, sz_f sz)
	{
		ptr -= SIZE;
		*((sz_f *) ptr) = sz;

	}
	inline sz_f get_size(sz_t ptr)
	{
		ptr -= SIZE;
		return *((sz_f *) ptr);
	}


	struct lst;
	struct node {
		node *next;
		node *prev;

		lst *top;
		node *down;
		

		sz_f sz;
		sz_t mem;


		node(node *next, node *prev, sz_t sz, sz_t mem)
			: next(next), prev(prev), sz(sz), mem(mem), top(0), down(0)
		{
		}
		node() {}
	};

	struct lst {
		node *top;
		lst *larger;
		lst *lesser;

		lst (node *top, lst *larger, lst *lesser)
			: top(top), larger(larger), lesser(lesser)
		{
		}
		lst() {}
	};

	node nd;
	lst fl;


	inline node *pop(lst *u)
	{
		node *ret = u->top;
		ret->top = 0;

		if ((u->top = ret->down) == 0) {
			u->lesser->larger = u->larger;
			u->larger->lesser = u->lesser;
			delete u;
		}

		ret->down = 0;
		return ret;
	}


	inline void delete_lst(node *u)
	{
		node *it = u->top->top;
		if (it == u) {
			pop(u->top);
			return;
		}

		while (it->down != u)
			it = it->down;

		it->down = u->down;
		u->down = 0;
		u->top = 0;
	}
	inline void insert_lst(node *u)
	{
		lst *it = fl.larger;

		while (it->top->sz < u->sz)
			it = it->larger;

		if (it->top->sz == u->sz) {
			u->top = it;
			u->down = it->top;
			it->top = u;
		} else {
			lst *t = new lst(u, it, it->lesser);
			it->lesser->larger = t;
			it->lesser = t;
			u->top = t;
			u->down = 0;
		}
	}

	// Free list
	inline bool connected(node *u, node *v)
	{
		return u->mem + u->sz + SIZE == v->mem;
	}
	inline node *merge(node *u, node *v)
	{
		u->next = v->next;
		v->next->prev = u;

		u->sz += (v->sz + SIZE);
		set_size(u->mem, u->sz);

		if (v->top) delete_lst(v);
		if (u->top) delete_lst(u);
		delete v;

		return u;
	}
	inline void delete_f(node *u)
	{
		u->prev->next = u->next;
		u->next->prev = u->prev;
		u->prev = u->next = 0;
	}
	inline node *insert_f(node *u)
	{
		node *it = nd.next;
		while (it->mem < u->mem)
			it = it->next;

		u->prev = it->prev;
		u->next = it;

		u->prev->next = u;
		it->prev = u;


		if (connected(u->prev, u))
			u = merge(u->prev, u);
		if (connected(u, u->next))
			u = merge(u, u->next);

		return u;
	}


	inline void initialize_pool(void* addr, sz_t nSize)
	{

		sz_t mem = ((sz_t ) addr)+SIZE;
		nd.sz = nSize+1u;
		nd.mem = mem + nSize + SIZE*2;

		nd.next = nd.prev = new node(&nd, &nd, nSize-SIZE, mem);
		set_size(nd.next->mem, nd.next->sz);


		fl.top = &nd;
		fl.lesser = fl.larger = new lst(nd.next, &fl, &fl);
		nd.next->top = fl.lesser;
	}

	inline void* acquire(unsigned long long nSize)
	{
		lst *it = fl.larger;

		while (it->top->sz < nSize)
			it = it->larger;


		if (it == &fl)
			return 0;


		node *bl = pop(it);
		delete_f(bl);

		sz_t mem = bl->mem;

		// Split the memory
		if (bl->sz > nSize+SIZE) {
			node *t = new node(0, 0, bl->sz-nSize-SIZE, mem + nSize + SIZE);
			set_size(t->mem, t->sz);

			t = insert_f(t);
			insert_lst(t);

			bl->sz = nSize;
		}

		set_size(mem, bl->sz);

		delete bl;

		return (void * ) mem;
	}

	



	inline void release(void* addr)
	{
		node *t = new node(0, 0, get_size((sz_t ) addr), (sz_t ) addr);
		t = insert_f(t);
		insert_lst(t);
	}




	struct destructor {
		~destructor()
		{
			node *it = nd.next;
			while (it != &nd) {
				node *t = it;
				it = it->next;
				delete t;
			}

			lst *lit = fl.larger;
			while (lit != &fl) {
				lst *t = lit;
				lit = lit->larger;
				delete t;
			}
		}
	} des;
}

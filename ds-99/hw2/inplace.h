#pragma once


namespace HOMEWORK
{
	typedef unsigned long long sz_t;
	typedef unsigned long long sz_f;
	const size_t SIZE = sizeof(sz_f);

//	const sz_t MAXSZ = 4194304;

	struct node {
		node *next;
		node *prev;

		sz_f sz;
		sz_t mem;


		node(node *next, node *prev, sz_t sz, sz_t mem)
			: next(next), prev(prev), sz(sz), mem(mem)
		{
		}
		node() {}
	};
	node nd;

	struct destructor {
		~destructor()
		{
			node *it = nd.next;
			while (it != &nd) {
				node *t = it;
				it = it->next;
				delete t;
			}
		}
	} des;

	void set_size(sz_t ptr, sz_f sz)
	{
		ptr -= SIZE;
		*((sz_f *) ptr) = sz;

	}
	sz_f get_size(sz_t ptr)
	{
		ptr -= SIZE;
		return *((sz_f *) ptr);
	}

	// Free list
	inline void delete_f(node *u)
	{
		u->prev->next = u->next;
		u->next->prev = u->prev;
		u->prev = u->next = 0;
	}
	inline void insert_f(node *u)
	{
		node *it = nd.next;
		while (it->mem < u->mem)
			it = it->next;

		u->prev = it->prev;
		u->next = it;

		u->prev->next = u;
		it->prev = u;

		if (u->prev->mem + u->prev->sz + SIZE == u->mem) {
			node *t = u->prev;
			u->prev = t->prev;
			t->prev->next = u;

			u->sz += (t->sz + SIZE);
			u->mem = t->mem;
			set_size(u->mem, u->sz);

			delete t;
		}

		if (u->next->mem - SIZE - u->sz == u->mem) {
			node *t = u->next;
			u->next = t->next;

			t->next->prev = u;

			u->sz += (t->sz + SIZE);
			set_size(u->mem, u->sz);

			delete t;
		}




	}


	inline void initialize_pool(void* addr, sz_t nSize)
	{

		sz_t mem = ((sz_t ) addr)+SIZE;
		nd.sz = nSize+1u;
		nd.mem = mem + nSize + SIZE*2;

		nd.next = nd.prev = new node(&nd, &nd, nSize-SIZE, ((sz_t ) addr)+SIZE);
		set_size(nd.next->mem, nd.next->sz);
	}

	inline void* acquire(unsigned long long nSize)
	{
		node *it = nd.next;

		while (it->sz < nSize)
			it = it->next;


		if (it == &nd)
			return 0;

		void *mem = (void *)it->mem;
		delete_f(it);

		if (it->sz > nSize+SIZE) {
			node *t = new node(0, 0, it->sz-nSize-SIZE, it->mem + nSize + SIZE);
			set_size(t->mem, t->sz);

			insert_f(t);

			it->sz = nSize;
		}

		set_size(it->mem, it->sz);

		delete it;

		return mem;
	}

	inline void release(void* addr)
	{
		node *t = new node(0, 0, get_size((sz_t ) addr), (sz_t ) addr);
		insert_f(t);
	}
}

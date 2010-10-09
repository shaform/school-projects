#include <cstdio>
#include <cstring>
#include <vector>
#include <ctime>
#include <cstdlib>
#include "allpool.h"
using namespace std;
using namespace HOMEWORK;

#if DEBUG
#define	CHECK check_list()
#else
#define	CHECK  
#endif

int main()
{

	void *test = operator new(4194308);
	initialize_pool(test, 4194308);

	// Large memory check
	int *t = (int *)acquire(4194000);
	CHECK;
	int *u = (int *)acquire(sizeof(int)*300);
	CHECK;
	if (u==0) {
		release((void *)t);
		CHECK;
		t = (int *)acquire(5*sizeof(int));
		CHECK;
		u = (int *)acquire(sizeof(int)*3);
		CHECK;
	}
	int *v = (int *)acquire(sizeof(int)*4);
	CHECK;

	// Check for memory validity
	for (int i=0; i<5; ++i)
		t[i] = i;
	for (int i=0; i<3; ++i)
		u[i] = i;
	for (int i=0; i<4; ++i)
		v[i] = i;

	release((void *)t);
	CHECK;
	t = (int *)acquire(sizeof(int)*3);
	CHECK;
	int *w = (int *)acquire(sizeof(int)*2);
	CHECK;
	release((void *)w);
	CHECK;
	w = (int *)acquire(sizeof(int)*2);
	CHECK;
	release((void *)w);
	CHECK;
	w = (int *)acquire(sizeof(int)*20);
	CHECK;
	release((void *)t);
	CHECK;
	release((void *)u);
	CHECK;
	release((void *)v);
	CHECK;
	release((void *)w);
	CHECK;


	// random check
	vector<void *> vec;
	int k =0;
	for (int i=0; i<10000; ++i) {
		int r = (rand() % 100000) + 1;
		k+=r;
		void * t = acquire(r);
		if (t) {
			memset(t, 0xff, r);
			vec.push_back(t);
		}
	}
	while (!vec.empty()) {
		release(vec.back());
		vec.pop_back();
	}

	// Reallocate Check
	void *r[100];
	r[0] = acquire(1000);
	CHECK;
	r[1] = acquire(2000);
	CHECK;
	r[2] = acquire(3000);
	CHECK;
	r[3] = acquire(4000);
	CHECK;
	r[4] = acquire(5000);
	CHECK;
	r[5] = acquire(6000);
	CHECK;
	release(r[2]);
	CHECK;
	release(r[4]);
	CHECK;
	r[2] = acquire(500);
	CHECK;
	r[4] = acquire(500);
	CHECK;
	r[6] = acquire(500);
	CHECK;
	r[7] = acquire(500);
	CHECK;
	r[8] = acquire(1500);
	CHECK;
	r[9] = acquire(2500);
	CHECK;
	r[10] = acquire(5500);
	CHECK;
	release(r[0]);
	CHECK;
	release(r[1]);
	CHECK;
	release(r[7]);
	CHECK;
	release(r[8]);
	CHECK;
	r[0] = acquire(1000);
	CHECK;
	r[1] = acquire(2000);
	CHECK;
	r[7] = acquire(500);
	CHECK;
	r[8] = acquire(1500);
	CHECK;

	for (int i=10; i>= 0; --i) {
		release(r[i]);
		CHECK;
	}

	operator delete(test);
}

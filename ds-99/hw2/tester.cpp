#include <cstdio>
#include "inplace.h"
int main()
{
	using namespace HOMEWORK;
	void *test = operator new(4194308);
	initialize_pool(test, 4194308);

	int *t = (int *)acquire(4194300);
	int *u = (int *)acquire(sizeof(int)*3);
	if (u==0) {
		release((void *)t);
		t = (int *)acquire(5*sizeof(int));
		u = (int *)acquire(sizeof(int)*3);
	}
	int *v = (int *)acquire(sizeof(int)*4);

	for (int i=0; i<5; ++i)
		t[i] = i;
	for (int i=0; i<3; ++i)
		u[i] = i;
	for (int i=0; i<4; ++i)
		v[i] = i;

	release((void *)t);
	t = (int *)acquire(sizeof(int)*3);
	int *w = (int *)acquire(sizeof(int)*2);
	release((void *)w);
	w = (int *)acquire(sizeof(int)*2);
	release((void *)w);
	w = (int *)acquire(sizeof(int)*20);
	release((void *)t);
	release((void *)u);
	release((void *)v);
	release((void *)w);



	operator delete(test);
}

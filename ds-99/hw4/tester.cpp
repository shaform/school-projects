#include <cstdio>
#include <cmath>
#include "hw4-v6.h"
/**
  putting your code here or linking it by using g++ code.cpp tester.cpp
 */
namespace HOMEWORK
{
	extern double Eval(const char* expr, bool& SyntaxError);
}

struct Match {
	const char *expr;
	double d;
	bool err;
};

bool eqs(double a, double b)
{  
	return std::abs(a-b) < 0.000001;
}

Match m[] = {
	{"1-----2", -1.0, true},
	{"2-(-1)", 3.0},
	{"2-(--3)", -1.0, true},
	{"-(-5)+3", -1.0, true},
	{"(1+2)*(2*(5-4))", 6.0},
	{"1+2+3+4+5+6+7+8+9+10", 55.0},
	{".1+.2+.3+.4+.5+-.6+.7+.8+.9+.10", -1.0, true},
	{"0.1+0.2+0.3+0.4+0.5+0.6+0.7+0.8+0.9+1.0", 5.5},
	{"(((1)))", 1.0},
	{"(((3)))^(((2)))", 9.0},
	{"-4817.", -4817.0},
	{"3-(5-(7+1))^2*(-5)+13", 61.0},
	{"3.2+(6.34*(1.7))", 3.2+(6.34*(1.7))},
	{"3+4*2/(1-5)^2", 3.5},
	{"1/2/3/4", 1.0/2.0/3.0/4.0},
};
int main()
{   
	using HOMEWORK::Eval;
	size_t sz = sizeof(m)/sizeof(Match);
	bool b;
	double d;
	printf("Checking...\n");
	for (size_t i=0; i!=sz; ++i) {
		d = Eval(m[i].expr, b);
		if (!eqs(d, m[i].d) || b!=m[i].err) {
			printf("------# %zu Checking failed!------\n"
					"With expr:%s\n"
					"Right response: d=%f, b=%d\n"
					"Your response: d=%f, b=%d\n"
					"----------------------------\n",
					i,
					m[i].expr, m[i].d, m[i].err,
					d, b);
		} else
			printf("check #%zu passed...\n", i);
	}
	char str[1000];
	printf("Free-typing tester...\n");
	while (scanf("%s", str) == 1) {
		d = Eval(str, b);
		printf("d=%f, b=%d\n", d, b);
	}


}


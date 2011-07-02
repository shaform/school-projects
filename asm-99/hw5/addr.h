#ifndef ADDR_H
#define ADDR_H
#include <vector>
#include <string>
#include "sicxeasm.h"
using namespace std;

struct AddrRef {
	string symbol;
	bool positive;
	AddrRef(string s, bool p) : symbol(s), positive(p) {}
};

struct Addr {
	int addr;
	int relative;
	vector<AddrRef> ext;
	bool error;
	Addr() : addr(0), relative(0), error(false) {}
	Addr &operator*=(const Addr &opr)
	{
		if (relative || opr.relative || ext.size() || opr.ext.size())
			error = true;
		else
			addr *= opr.addr;

		return *this;
	}
	Addr &operator/=(const Addr &opr)
	{
		if (relative || opr.relative || ext.size() || opr.ext.size())
			error = true;
		else
			addr /= opr.addr;
		return *this;
	}
	Addr &operator+=(const Addr &opr)
	{
		addr += opr.addr;
		relative += opr.relative;
		for (unsigned i=0; i<opr.ext.size(); ++i)
			ext.push_back(opr.ext[i]);
		return *this;
	}
	Addr &operator-=(const Addr &opr)
	{
		addr -= opr.addr;
		relative -= opr.relative;
		for (unsigned i=0; i<opr.ext.size(); ++i) {
			AddrRef t = opr.ext[i];
			t.positive = !t.positive;
			ext.push_back(t);
		}

		return *this;
	}
};
#endif

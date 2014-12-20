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

// Address representation.
struct Addr {
	int addr;
	int relative;
	// External reference.
	vector<AddrRef> ext;
	bool error;
	Addr() : addr(0), relative(0), error(false) {}
	Addr &operator*=(const Addr &opr)
	{
		if (relative || opr.relative || ext.size() || opr.ext.size() || opr.error)
			error = true;
		else
			addr *= opr.addr;

		return *this;
	}
	Addr &operator/=(const Addr &opr)
	{
		if (relative || opr.relative || ext.size() || opr.ext.size() || opr.error)
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
		if (opr.error) error = true;
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
		if (opr.error) error = true;
		return *this;
	}
};
#endif

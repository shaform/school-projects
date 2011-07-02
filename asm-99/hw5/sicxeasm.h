#ifndef SIC_H
#define SIC_H

#include <cstring>
#include <string>
#include <cctype>
#include <set>

const int MAXL = 200;
const int NUM_INC = 59;
const int NUM_DIR = 15;
const int NUM_OP = NUM_INC + NUM_DIR;
typedef unsigned addr_t;
const int MAXPG = 6;
extern addr_t REG[256];
extern bool SIC[256];

// ------------------------------------------------------------------ //
//
struct Mod {
	addr_t start_addr;
	addr_t sz;
	bool pos;
	std::string sym;
	Mod(addr_t sa, addr_t s) : start_addr(sa), sz(s) {}
	Mod(addr_t sa, addr_t s, bool p, std::string st) : start_addr(sa), sz(s), pos(p), sym(st) {}
};
struct Opcode {
	char mnem[7];
	unsigned char opcode;
	char format;

	Opcode() {}
	Opcode(const char *s) { strcpy(mnem, s); }
	Opcode(const char *s, unsigned char op, char f) : opcode(op), format(f)
	{ strcpy(mnem, s); }
	bool operator<(const Opcode &rhs) const { return strcmp(mnem, rhs.mnem) < 0; }
};

class Optab : public std::set<Opcode> {
	public:
		Optab();
};

extern Optab OPTAB;

// ------------------------------------------------------------------ //
bool store_x(const char *from, char *to, int n);
bool store_c(const char *from, char *to, int n);
#endif

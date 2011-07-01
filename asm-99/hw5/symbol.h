#ifndef SYM_H
#define SYM_H
const int MAXSYM = 6;
struct Symbol {
	addr_t value;
	string def;
	bool absolute;
	bool external;

	bool is_absolute() { return absolute; }
	bool is_external() { return external; }
	bool is_recursive() { return def.size(); }

	Symbol() : value(0), absolute(true), external(false) {}
	Symbol(addr_t ad) : value(ad), absolute(false), external(false) {}
	Symbol(string d) : value(0), def(d), absolute(false), external(false) {}
	Symbol(addr_t ad, bool ab) : value(ad), absolute(ab), external(false) {}
	Symbol(addr_t ad, bool ab, bool ext) : value(ad), absolute(ab), external(ext) {}
};
struct Literal {
	addr_t addr;
	string bytes;
	Literal(addr_t ad, string b) : addr(ad), bytes(b) {}
	Literal() : addr(0) {}
};
#endif

#ifndef SYM_H
#define SYM_H
const int MAXSYM = 6;
struct Symbol {
	addr_t value;
	string def;
	bool absolute;
	bool external;
	// block number
	unsigned bn;

	bool is_absolute() { return absolute; }
	bool is_external() { return external; }
	bool is_recursive() { return def.size(); }

	Symbol() : value(0), absolute(true), external(false), bn(0) {}
	Symbol(addr_t ad, unsigned b) : value(ad), absolute(false), external(false), bn(b) {}
	Symbol(addr_t ad, string d, unsigned b) : value(ad), def(d), absolute(false), external(false), bn(b) {}
	Symbol(addr_t ad, bool ab, unsigned b) : value(ad), absolute(ab), external(false), bn(b) {}
	Symbol(addr_t ad, bool ab, bool ext, unsigned b) : value(ad), absolute(ab), external(ext), bn(b) {}
};
struct Literal {
	addr_t addr;
	string bytes;
	unsigned bn;
	Literal(addr_t ad, string b) : addr(ad), bytes(b), bn(0) {}
	Literal() : addr(0), bn(0) {}
};
#endif

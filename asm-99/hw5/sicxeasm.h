#include <cstring>
#include <set>

const int MAXL = 200;
const int NUM_INC = 59;
const int NUM_DIR = 15;
const int NUM_OP = NUM_INC + NUM_DIR;
typedef unsigned addr_t;

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

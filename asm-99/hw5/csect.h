#ifndef SIC_CSECT_H
#define SIC_CSECT_H
#include <cctype>
#include <map>
#include <vector>
#include "file.h"
#include "sicxeasm.h"

typedef map<string, Symbol>::iterator SYMIt;

// Program block
struct Block {
	addr_t start_addr;
	addr_t LOCCTR;
	addr_t length;
	string name;
	Block() : start_addr(0), LOCCTR(0), length(0) {}
	Block(string s) : start_addr(0), LOCCTR(0), length(0), name(s) {}
};
// Control Section
struct CSection {
	FileHandler *fh;
	char sec_name[MAXPG+1];
	addr_t start_addr, LOCCTR;
	addr_t sec_length;
	addr_t base;
	bool nobase;
	map<string, Symbol> SYMTAB;
	vector<Literal> LITTAB;
	unsigned lt;
	vector<Mod> M;
	vector<string> extref;
	vector<string> extdef;
	bool main_sect;
	bool more_sect;
	Line *l;
	string end_str;
	vector<Block> bl;
	unsigned bt;

	void clear_name()
	{
		memset(sec_name, ' ', sizeof(sec_name));
		sec_name[MAXPG] = '\0';
	}
	CSection(FileHandler *fh)
		: fh(fh), start_addr(0u), LOCCTR(0u), sec_length(0u), nobase(true), lt(0),
		main_sect(false), more_sect(false), l(fh->get_line()), bl(1), bt(0)
	{
		clear_name();
	}
	CSection(FileHandler *fh, const char *name, const char *addr)
		: fh(fh), LOCCTR(0u), sec_length(0u), nobase(true), lt(0),
		main_sect(false), more_sect(false), l(fh->get_line()), bl(1), bt(0)
	{
		clear_name();
		sscanf(addr, "%d", &start_addr);
		for (int i=0; i<MAXPG && name[i]; ++i)
			sec_name[i] = name[i];
		// Initialize LOCCTR to be always zero.
		//LOCCTR = start_addr;
	}

	// Generates a appropriate address syntax.
	addr_t calc_addr(int dest, bool ext, bool relative = true, bool enable = true)
	{
		if (ext)
			return (1 << 20) | dest;
		else if (!relative && dest <= 2047 && dest >= -2048)
			return (3 << 13) | dest;
		else if (enable && dest <= LOCCTR + 2047 && dest + 2048 >= LOCCTR)
			return (1 << 13) | ((dest-LOCCTR) & ((1u<<12)-1u));
		else if (enable && !nobase && dest >= base && dest <= base + 4095)
			return (1 << 14) | (dest-base);
		else {
			fprintf(stderr, "## Address out of range.\n");
			return 1 << 13;
		}
	}
	bool pass1()
	{
		while (!(l->op("END") || l->op("CSECT") || fh->p1_eof())) {
			// Special processing for literals.
			if (l->oper[0][0] == '=') {

				bool l_error = true;
				l->lit = true;
				if (store_c(&l->oper[0][1], l->oper[1], MAXOPER))
					l_error = false;
				else if (store_x(&l->oper[0][1], l->oper[1], MAXOPER))
					l_error = false;
				else if (l_error && l->oper[0][1] == '*' && l->oper[0][2] == '\0') {
					sprintf(l->oper[1], "%06X", LOCCTR);
					l_error = false;
				}
				if (l_error) {
					fh->print_line();
					fprintf(stderr, "## Invalid literal.\n");
				} else {
					unsigned i;
					for (i=0; i<LITTAB.size(); ++i) {
						if (LITTAB[i].bytes == l->oper[1])
							break;
					}
					// Record the literal.
					if (i==LITTAB.size())
						LITTAB.push_back(Literal(0, l->oper[1]));
					sprintf(&l->oper[0][1],
							"%06X", i);
					l->oper[0][7] = '\0';
				}
			}
			// Stores label.
			if (l->has_label()) {
				if (SYMTAB.find(l->label) != SYMTAB.end()) {
					fh->print_line();
					fprintf(stderr, "## Duplicate symbol %s.\n", l->label);
				} else if (l->op("EQU")) {
					SYMTAB.insert(make_pair(l->label, Symbol(LOCCTR, string(l->oper[0]), bt)));
					fh->p1_read_line();
					continue;
				} else {
					SYMTAB.insert(make_pair(l->label, Symbol(LOCCTR, bt)));
				}
			}

			// Adds the appropriate offset.
			LOCCTR += l->format;
			if (l->format == 0) {
				if (l->op("WORD"))
					LOCCTR += 3;
				else if (l->op("BYTE")) {
					bool error = true;
					addr_t size = 1u;
					if (store_x(l->oper[0], l->oper[1], MAXOPER/2)) error = false;
					else if (store_c(l->oper[0], l->oper[1], MAXOPER/2)) error = false;

					if (error) {
						fh->print_line();
						fprintf(stderr, "## Invalid BYTE definition.\n");
						strcpy(l->oper[0], "X'00'");
					} else
						size = strlen(l->oper[1])/2;
					LOCCTR += size;
				} else if (l->op("RESB")) {
					int t;
					sscanf(l->oper[0], "%d", &t);
					LOCCTR += t;
				} else if (l->op("RESW")) {
					int t;
					sscanf(l->oper[0], "%d", &t);
					LOCCTR += t*3;
				} else if (l->op("LTORG")) {
					p1_put_lt();
				} else if (l->op("EXTDEF")) {
					for (unsigned i=0; i<fh->hack.size(); ++i) {
						extdef.push_back(fh->hack[i]);
					}
					fprintf(fh->mid, "0 EXTDEF\n");
				} else if (l->op("EXTREF")) {
					for (unsigned i=0; i<fh->hack.size(); ++i) {
						if (SYMTAB.find(fh->hack[i]) != SYMTAB.end()) {
							fprintf(stderr, "## Duplicate symbol %s.\n", fh->hack[i].c_str());
							continue;
						} else {
							SYMTAB.insert(make_pair(fh->hack[i], Symbol(0, true, true, 0)));
							extref.push_back(fh->hack[i]);
						}
					}
					fprintf(fh->mid, "0 EXTREF\n");
				} else if (l->op("USE")) {
					// Switchs block.
					bl[bt].LOCCTR = LOCCTR;
					if (l->oper[0][0] == '\0') {
						bt = 0;
						LOCCTR = bl[0].LOCCTR;
					} else for (bt=1; bt<bl.size(); ++bt) {
						if (bl[bt].name == l->oper[0]) {
							LOCCTR = bl[bt].LOCCTR;
							break;
						}
					}
					// Creates a new block.
					if (bt==bl.size()) {
						bl.push_back(Block(l->oper[0]));
						LOCCTR = 0;
					}
				} else {
					//fprintf(stderr, "FIXME!!\n");
					//fh->print_line();
				}
			}
			fh->p1_write_line();
			fh->p1_read_line();
		}
		p1_put_lt();

		fh->p1_write_line();
		bl[bt].LOCCTR = LOCCTR;
		sec_length = 0;


		// Hack to accomplish program blocks.
		// Calculates correct offsets.
		for (unsigned i=0; i<bl.size(); ++i) {
			bl[i].length = bl[i].LOCCTR;
			bl[i].LOCCTR = bl[i].start_addr = start_addr + sec_length;
			sec_length += bl[i].length;
		}
		// Calculates correct addresses.
		for (SYMIt it=SYMTAB.begin(); it!=SYMTAB.end(); ++it) {
			it->second.value += bl[it->second.bn].start_addr;
		}
		for (unsigned i=0; i<LITTAB.size(); ++i) {
			LITTAB[i].addr += bl[LITTAB[i].bn].start_addr;
		}
		return true;
	}
	bool pass2()
	{
		LOCCTR = start_addr;
		bt = 0;
		fprintf(fh->out, "H%s%06X%06X\n", sec_name, start_addr, sec_length);
		Recoder recoder(start_addr, fh->out);
		while (!(l->op("END") || (l->op("CSECT")) ||  fh->p2_eof())) {
			char f = l->format;
			LOCCTR += f;
			if (f == 0) {
				int t;
				sscanf(l->oper[0], "%d", &t);
				if (l->op("RESW")) {
					recoder.res(t*3);
					LOCCTR += t*3;
				} else if (l->op("RESB")) {
					recoder.res(t);
					LOCCTR += t;
				} else if (l->op("WORD")) {
					const char *s = l->oper[0];
					Addr ad = resolve(s, LOCCTR);
					if (ad.error) {
						fprintf(stderr, "##Wrong WORD def %s\n", l->oper[0]);
					} else {
						recoder.insert(ad.addr, 3);
						if (ad.relative) M.push_back(Mod(LOCCTR, 6));
						for (unsigned i=0; i<ad.ext.size(); ++i)
							M.push_back(Mod(LOCCTR, 6,
										ad.ext[i].positive,
										ad.ext[i].symbol));
					}
					LOCCTR += 3;
				} else if (l->op("NOBASE")) {
					nobase = true;
				} else if (l->op("BASE")) {
					SYMIt it = SYMTAB.find(l->oper[0]);
					if (it!=SYMTAB.end()) {
						nobase = false;
						base = it->second.value;
					} else 
						fprintf(stderr, "##Undefined symbol: %s\n", l->oper[0]);
				} else if (l->op("EXTDEF")) {
					fprintf(fh->out, "%c", 'D');
					for (unsigned i=0; i<extdef.size(); ++i) {
						if (SYMTAB.find(extdef[i]) == SYMTAB.end()) {
							fprintf(stderr, "##Undefined symbol: %s\n", fh->hack[i].c_str());
							continue;
						} else {
							char s[7];
							memset(s, ' ', sizeof(s));
							s[6] = '\0';
							for (int j=0; extdef[i][j]; ++j)
								s[j] = extdef[i][j];
							fprintf(fh->out, "%s%06X", s, SYMTAB.find(extdef[i])->second.value);
						}
					}
					fprintf(fh->out, "\n");
				} else if (l->op("EXTREF")) {
					fprintf(fh->out, "%c", 'R');
					for (unsigned i=0; i<extref.size(); ++i) {
						char s[7];
						memset(s, ' ', sizeof(s));
						s[6] = '\0';
						for (int j=0; extref[i][j]; ++j)
							s[j] = extref[i][j];
						fprintf(fh->out, "%s", s);
					}
					fprintf(fh->out, "\n");
				} else if (l->op("-")) {
					LOCCTR += strlen(l->oper[0])/2;
					recoder.insert(l->oper[0]);
				} else if (l->op("USE")) {
					// Program block.
					unsigned i=0;
					if (l->oper[0][0] == '\0') {
						i = 0;
					} else for (i=1; i<bl.size(); ++i) {
						if (bl[i].name == l->oper[0]) {
							break;
						}
					}
					if (i!=bt) {
						recoder.flush();
						bl[bt].LOCCTR = LOCCTR;
						recoder.tr.start_addr = LOCCTR = bl[i].LOCCTR;
						bt = i;
					}
				}
			} else if (f == 1) {
				recoder.insert(l->mnem);
			} else if (f == 2) {
				addr_t r1, r2;
				sscanf(l->oper[0], "%X", &r1);
				sscanf(l->oper[1], "%X", &r2);
				recoder.insert((l->opcode << 8) | (r1 << 4) | r2, 2);
			} else if (f == 3) {
				const char *s = l->oper[0];
				Addr ad = resolve(s, LOCCTR-3);
				if (*s != '\0' || ad.error) fprintf(stderr, "## Invalid operand: %s\n", l->oper[0]);
				addr_t op = l->opcode, n = l->idi, i = l->imm, x = l->idx;
				if (!(n|i)) op |= 3;
				addr_t ins = (op << 16) | (n << 17) | (i << 16) | (x << 15);
				ins |= calc_addr(ad.addr, false, ad.relative, ad.ext.empty());
				if (SIC[op & 0xfc] && ((ins >> 13) & 3) == 3) ins ^= 3 << 13;
				if (ad.relative && ((((ins >> 13) & 1u) ^ ((ins >> 14) & 1u)) == 0))
					M.push_back(Mod(LOCCTR - 2, 3));
				for (unsigned i=0; i<ad.ext.size(); ++i)
					M.push_back(Mod(LOCCTR -2, 3,
								ad.ext[i].positive,
								ad.ext[i].symbol));
				recoder.insert(ins, 3);
			} else if (f == 4) {
				const char *s = l->oper[0];
				Addr ad = resolve(s, LOCCTR-4);
				if (*s != '\0' || ad.error) fprintf(stderr, "## Invalid operand: %s\n", l->oper[0]);
				addr_t op = l->opcode, n = l->idi, i = l->imm, x = l->idx;
				if (!(n|i)) op |= 3;
				addr_t ins = (op << 24) | (n << 25) | (i << 24) | (x << 23);
				ins |= calc_addr(ad.addr, true);
				if (ad.relative && ((((ins >> 21) & 1u) ^ ((ins >> 22) & 1u)) == 0))
					M.push_back(Mod(LOCCTR - 3, 5));
				for (unsigned i=0; i<ad.ext.size(); ++i)
					M.push_back(Mod(LOCCTR -3, 5,
								ad.ext[i].positive,
								ad.ext[i].symbol));
				recoder.insert(ins, 4);
			} else {
				fprintf(stderr, "Invaild immediate file syntax, aborting...\n%c\n", f);
				return false;
			}
			fh->p2_read_line();
		}
		recoder.flush();
		for (unsigned i=0; i<M.size(); ++i) {
			if (main_sect && !more_sect)
				fprintf(fh->out, "M%06X%02X\n", M[i].start_addr, M[i].sz);
			else {
				char sign = M[i].pos ? '+' : '-';
				// Control section syntax, default section name.
				const char *s = sec_name;
				if (M[i].sym.size()) s = M[i].sym.c_str();
				fprintf(fh->out, "M%06X%02X%c%s\n", M[i].start_addr, M[i].sz, sign, s);
			}
		}
		// Switch between basic obj & control section obj.
		if (main_sect) {
			const char *s = end_str.c_str();
			Addr ad = resolve(s, LOCCTR);
			if (!ad.error && ad.ext.empty())
				fprintf(fh->out, "E%06X\n", ad.addr);
			else 
				fprintf(stderr, "##Invalid end\n");
		} else
			fprintf(fh->out, "E\n");
		if (more_sect)
			fprintf(fh->out, "\n\n");
		return true;
	}
	// Puts literals in here.
	void p1_put_lt()
	{
		while (lt < LITTAB.size()) {
			LITTAB[lt].addr = LOCCTR;
			LITTAB[lt].bn = bt;
			LOCCTR += LITTAB[lt].bytes.length()/2;
			fh->p1_write_byte(LITTAB[lt].bytes.c_str());
			++lt;
		}
	}

	// Operand Resolver, can handle expressions.
	Addr resolve(const char *&str, addr_t ad)
	{
		Addr operl = resolveT(str, ad);
		while (*str == '+' || *str == '-') {
			char op = *(str++);
			Addr operr = resolveT(str, ad);
			if (op == '+')
				operl += operr;
			else
				operl -= operr;
		}
		return operl;
	}
	Addr resolveT(const char *&str, addr_t ad)
	{
		Addr operl = resolveF(str, ad);
		while (*str == '*' || *str == '/') {
			char op = *(str++);
			Addr operr = resolveF(str, ad);
			if (op == '*')
				operl *= operr;
			else
				operl /= operr;
		}
		return operl;
	}
	Addr resolveF(const char *&str, addr_t ad)
	{
		if (*str == '(') {
			++str;
			Addr res = resolve(str, ad);
			if (*str != ')') res.error = true;
			else ++str;
			return res;
		} else
			return resolveN(str, ad);
	}
	Addr resolveN(const char *&str, addr_t ad)
	{
		Addr res;
		if (isdigit(*str)) {
			while (isdigit(*str)) {
				res.addr *= 10;
				res.addr += *str - '0';
				++str;
			}
		} else if (isalpha(*str)) {
			char t[MAXSYM+1];
			for (int i=0; i<MAXSYM; ++i)
				if (isalnum(*str)) t[i] = *(str++);
				else {
					t[i] = '\0';
					break;
				}
			t[MAXSYM] = '\0';
			SYMIt it = SYMTAB.find(t);
			if (it == SYMTAB.end())
				res.error = true;
			else if (it->second.is_recursive()) {
				const char *i = it->second.def.c_str();
				res = resolve(i, it->second.value);
			} else {
				res.relative = !it->second.is_absolute();
				res.addr = it->second.value;
				if (it->second.is_external()) {
					res.ext.push_back(AddrRef(t, true));
				}
			}
		} else if (*str == '=') {
			unsigned t;
			sscanf(++str, "%X", &t);
			str += 6;
			res.relative = 1;
			res.addr = LITTAB[t].addr;
		} else if (*str == '*') {
			res.relative = 1;
			res.addr = ad;
			++str;
		} else if (*str != '\0')
			res.error = true;
		return res;
	}
};
#endif

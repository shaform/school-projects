#ifndef SIC_CSECT_H
#define SIC_CSECT_H
#include <cctype>
#include <map>
#include <vector>
#include "file.h"
#include "sicxeasm.h"

typedef map<string, Symbol>::iterator SYMIt;

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

	void clear_name()
	{
		memset(sec_name, ' ', sizeof(sec_name));
		sec_name[MAXPG] = '\0';
	}
	CSection(FileHandler *fh)
		: fh(fh), start_addr(0u), LOCCTR(0u), sec_length(0u), nobase(true), lt(0),
		main_sect(false), more_sect(false), l(fh->get_line())
	{
		clear_name();
	}
	CSection(FileHandler *fh, const char *name, const char *addr)
		: fh(fh), sec_length(0u), nobase(true), lt(0), main_sect(false), more_sect(false), l(fh->get_line())
	{
		clear_name();
		sscanf(addr, "%d", &start_addr);
		for (int i=0; i<MAXPG && name[i]; ++i)
			sec_name[i] = name[i];
		LOCCTR = start_addr;
	}

	// Generates a appropriate address syntax.
	addr_t calc_addr(int dest, bool ext, bool relative = true)
	{
		if (ext)
			return (1 << 20) | dest;
		else if (!relative && dest <= 2047 && dest >= -2048)
			return (3 << 13) | dest;
		else if (dest <= LOCCTR + 2047 && dest + 2048 >= LOCCTR)
			return (1 << 13) | ((dest-LOCCTR) & ((1u<<12)-1u));
		else if (!nobase && dest >= base && dest <= base + 4095)
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
					if (i==LITTAB.size())
						LITTAB.push_back(Literal(0, l->oper[1]));
					sprintf(&l->oper[0][1],
							"%06X", i);
					l->oper[0][7] = '\0';
				}
			}
			if (l->has_label()) {
				if (SYMTAB.find(l->label) != SYMTAB.end()) {
					fh->print_line();
					fprintf(stderr, "## Duplicate symbol %s.\n", l->label);
				} else if (l->op("EQU")) {
					SYMTAB.insert(make_pair(l->label, Symbol(LOCCTR, string(l->oper[0]))));
					fh->p1_read_line();
					continue;
				} else {
					SYMTAB.insert(make_pair(l->label, Symbol(LOCCTR)));
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
				} else if (l->op("EXTREF")) {
				} else {
					fprintf(stderr, "FIXME!!\n");
					fh->print_line();
				}
			}
			fh->p1_write_line();
			fh->p1_read_line();
		}
		p1_put_lt();

		fh->p1_write_line();
		sec_length = LOCCTR - start_addr;
		return true;
	}
	bool pass2()
	{
		LOCCTR = start_addr;
		fprintf(fh->out, "H%s%06X%06X\n", sec_name, start_addr, sec_length);
		Recoder recoder(start_addr, fh->out);
		while (!(l->op("END") || fh->p2_eof())) {
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
				} else if (l->op("NOBASE")) {
					nobase = true;
				} else if (l->op("BASE")) {
					SYMIt it = SYMTAB.find(l->oper[0]);
					if (it!=SYMTAB.end()) {
						nobase = false;
						base = it->second.value;
					} else 
						fprintf(stderr, "##Undefined symbol: %s\n", l->oper[0]);
				} else if (l->op("-")) {
					LOCCTR += strlen(l->oper[0])/2;
					recoder.insert(l->oper[0]);
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
				if (*s != '\0') fprintf(stderr, "## Invalid operand: %s\n", l->oper[0]);
				addr_t op = l->opcode, n = l->idi, i = l->imm, x = l->idx;
				if (!(n|i)) op |= 3;
				addr_t ins = (op << 16) | (n << 17) | (i << 16) | (x << 15);
				ins |= calc_addr(ad.addr, false, ad.relative);
				if (SIC[op & 0xfc] && ((ins >> 13) & 3) == 3) ins ^= 3 << 13;
				if (ad.relative && ((((ins >> 13) & 1u) ^ ((ins >> 14) & 1u)) == 0))
					M.push_back(Mod(LOCCTR - 2, 3));
				recoder.insert(ins, 3);
			} else if (f == 4) {
				const char *s = l->oper[0];
				Addr ad = resolve(s, LOCCTR-4);
				if (*s != '\0') fprintf(stderr, "## Invalid operand: %s\n", l->oper[0]);
				addr_t op = l->opcode, n = l->idi, i = l->imm, x = l->idx;
				if (!(n|i)) op |= 3;
				addr_t ins = (op << 24) | (n << 25) | (i << 24) | (x << 23);
				ins |= calc_addr(ad.addr, true);
				if (ad.relative && ((((ins >> 21) & 1u) ^ ((ins >> 22) & 1u)) == 0))
					M.push_back(Mod(LOCCTR - 3, 5));
				recoder.insert(ins, 4);
			} else {
				fprintf(stderr, "Invaild immediate file syntax, aborting...\n%c\n", f);
				return false;
			}
			fh->p2_read_line();
		}
		recoder.flush();
		for (unsigned i=0; i<M.size(); ++i) {
			fprintf(fh->out, "M%06X%02X\n", M[i].start_addr, M[i].sz);
		}
		if (main_sect) {
			SYMIt it = SYMTAB.find(l->oper[0]);
			if (it!=SYMTAB.end())
				fprintf(fh->out, "E%06X\n", it->second.value);
			else 
				fprintf(stderr, "##Undefined symbol: %s\n", l->oper[0]);
		} else
			fprintf(fh->out, "E\n");
		if (more_sect)
			fprintf(fh->out, "\n\n");
		return true;
	}
	void p1_put_lt()
	{
		while (lt < LITTAB.size()) {
			LITTAB[lt].addr = LOCCTR;
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

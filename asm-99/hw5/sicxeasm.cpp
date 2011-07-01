#include <cstdio>
#include <string>
#include <cstring>
#include <set>
#include <map>

#include "sicxeasm.h"
#include "textrecord.h"
#include "addr.h"
#include "symbol.h"
using namespace std;

struct Mod {
	addr_t start_addr;
	addr_t sz;
	Mod(addr_t sa, addr_t s) : start_addr(sa), sz(s) {}
};

struct Line {
	char label[9];
	char mnem[7];
	char operand[2][40];

	unsigned char opcode;
	char format;

	bool indirect;
	bool immediate;
	bool ext;
	bool index;
	bool literal;
};
typedef map<string, Symbol>::iterator SYMIt;
addr_t REG[256];
bool SIC[256];



// Immediate file syntax
//
// *BYTES*
// -[BYTE]+
//
// *Directive*
// 0DIRECTIVE OPERAND
//
// *Instruction*
// f [opcode] {[n][i][x]}? OPERAND
// f: format
//
// *OPERAND*
// [byte]+ [expr]*
//

class Assembler {
	public:
		Assembler(FILE *i, FILE *m, FILE *o)
			: in(i), middle(m), out(o), start_addr(0), LOCCTR(0), nobase(true), lt(0) {}
		void assemble()
		{
			pass1();
			rewind(middle);
			pass2();
		}
	private:
		// Files
		FILE *in, *middle, *out;


		// Temporary variables.
		Line line;
		char buff_i[MAXL], buff[MAXL];
		char buff_t[MAXL];

		// Internal data for assembly
		char prg_name[MAXPG+1], sec_name[MAXPG+1];
		addr_t start_addr, LOCCTR;
		addr_t prg_length;
		addr_t base;
		bool nobase;
		map<string, Symbol> SYMTAB;
		vector<Literal> LITTAB;
		unsigned lt;

		vector<Mod> M;
		void pass1()
		{
			memset(prg_name, ' ', sizeof(prg_name));
			prg_name[MAXPG] = '\0';
			p1_read_line();
			if (line.format == 0 && strcmp(line.mnem, "START") == 0) {
				sscanf(line.operand[0], "%d", &start_addr);
				for (int i=0; line.label[i]; ++i)
					prg_name[i] = line.label[i];
				LOCCTR = start_addr;
				p1_read_line();
			}

			while (!(!line.format && strcmp(line.mnem, "END") == 0)) {
				if (feof(in)) {
					fprintf(stderr, "## No END directive found.\n");
					break;
				}

				if (line.label[0]) {
					if (SYMTAB.find(line.label) != SYMTAB.end()) {
						print_line();
						fprintf(stderr, "## Duplicate symbol %s.\n", line.label);
					} else if (strcmp(line.mnem, "EQU") == 0) {
						SYMTAB.insert(make_pair(line.label, Symbol(line.operand[0])));
						p1_read_line();
						continue;
					} else {
						SYMTAB.insert(make_pair(line.label, Symbol(LOCCTR)));
					}
				}

				// Adds the appropriate offset.
				LOCCTR += line.format;
				if (line.format == 0) {
					if (strcmp(line.mnem, "WORD") == 0)
						LOCCTR += 3;
					else if (strcmp(line.mnem, "BYTE") == 0) {
						bool error = true;
						int size = 1;
						memset(buff_t, 0, sizeof(buff_t));
						if (line.operand[0][0] == 'X') {
							if (store_x(line.operand[0], buff_t, 20)) error = false;
						} else if (line.operand[0][0] == 'C') {
							if (store_c(line.operand[0], buff_t, 20)) error = false;
						}
						if (error) {
							print_line();
							fprintf(stderr, "## Invalid BYTE definition.\n");
							strcpy(line.operand[0], "X'00'");
						} else {
							size = strlen(buff_t)/2;
							print_line();
							printf("owo: %s\n", buff_t);
							strcpy(line.operand[1], buff_t);
						}
						LOCCTR += size;
					} else if (strcmp(line.mnem, "RESB") == 0) {
						int t;
						sscanf(line.operand[0], "%d", &t);
						LOCCTR += t;
					} else if (strcmp(line.mnem, "RESW") == 0) {
						int t;
						sscanf(line.operand[0], "%d", &t);
						LOCCTR += t*3;
					} else if (strcmp(line.mnem, "LTORG") == 0) {
						printf("LTORG !!!!!!!!!!!!!!\n");
						p1_put_lt();
					} else {
						printf("WRONG!! %s\n", line.mnem);
						print_line();
					}
				}
				p1_write_line();
				p1_read_line();
			}
			p1_put_lt();

			p1_write_line();
			prg_length = LOCCTR - start_addr;

		}
		void p1_put_lt()
		{
			while (lt < LITTAB.size()) {
				LITTAB[lt].addr = LOCCTR;
				LOCCTR += LITTAB[lt].bytes.length()/2;
				write_byte(LITTAB[lt].bytes.c_str());
				++lt;
			}
		}
		void pass2()
		{
			LOCCTR = start_addr;
			fprintf(out, "H%s%06X%06X\n", prg_name, start_addr, prg_length);
			Recoder recoder(start_addr, out);
			char f;
			while (fscanf(middle, " %c", &f) == 1) {
				printf("t2: %x\n", LOCCTR);
				if (isdigit(f)) LOCCTR += f-'0';
				if (f == '0') {
					read_line(middle);
					printf("ine: %s\n", buff);
					int t;
					sscanf(buff, "%s %d", buff_t, &t);
					if (strcmp(buff_t, "RESW") == 0) {
						recoder.res(t*3);
						LOCCTR += t*3;
					} else if (strcmp(buff_t, "RESB") == 0) {
						recoder.res(t);
						LOCCTR += t;
					} else if (strcmp(buff_t, "NOBASE") == 0) {
						nobase = true;
					} else if (strcmp(buff_t, "BASE") == 0) {
						sscanf(buff, "%s%s", buff_t, buff_t);
						SYMIt it = SYMTAB.find(buff_t);
						if (it!=SYMTAB.end()) {
							nobase = false;
							base = it->second.value;
						} else 
							fprintf(stderr, "##Undefined symbol: %s\n", buff_t);
					} else if (strcmp(buff_t, "END") == 0) {
						recoder.flush();
						sscanf(buff, "%s%s", buff_t, buff_t);
						for (unsigned i=0; i<M.size(); ++i) {
							fprintf(out, "M%06X%02X\n", M[i].start_addr, M[i].sz);
						}
						SYMIt it = SYMTAB.find(buff_t);
						if (it!=SYMTAB.end())
							fprintf(out, "E%06X\n", it->second.value);
						else 
							fprintf(stderr, "##Undefined symbol: %s\n", buff_t);

					}
					// BASE RESW RESB END
				} else if (f == '1') {
					fscanf(middle, "%s", buff_t);
					recoder.insert(buff_t);
				} else if (f == '2') {
					read_line(middle);
					printf("ine: %s\n", buff);
					addr_t op = 0, r1 = 0, r2 = 0;
					sscanf(buff, "%X%X%X", &op, &r1, &r2);
					recoder.insert((op << 8) | (r1 << 4) | r2, 2);
				} else if (f == '3') {
					read_line(middle);
					printf("ine: %s\n", buff);
					addr_t op = 0, n = 0, i = 0, x = 0;
					memset(buff_t, 0, sizeof(buff_t));
					sscanf(buff, "%X %X%X%X %s", &op, &n, &i, &x, buff_t);
					const char *s = buff_t;
					Addr ad = resolve(s);
					if (!(n|i)) op |= 3;
					addr_t ins = (op << 16) | (n << 17) | (i << 16) | (x << 15);
					ins |= calc_addr(ad.addr, false, ad.relative);
					if (SIC[op & 0xfc] && ((ins >> 13) & 3) == 3) ins ^= 3 << 13;
					if (ad.relative && ((((ins >> 13) & 1u) ^ ((ins >> 14) & 1u)) == 0))
						M.push_back(Mod(LOCCTR - 2, 3));
					recoder.insert(ins, 3);
				} else if (f == '4') {
					read_line(middle);
					addr_t op = 0, n = 0, i = 0, x = 0;
					memset(buff_t, 0, sizeof(buff_t));
					sscanf(buff, "%X %X%X%X %s", &op, &n, &i, &x, buff_t);
					const char *s = buff_t;
					Addr ad = resolve(s);
					if (!(n|i)) op |= 3;
					addr_t ins = (op << 24) | (n << 25) | (i << 24) | (x << 23);
					ins |= calc_addr(ad.addr, true);
					if (ad.relative && ((((ins >> 21) & 1u) ^ ((ins >> 22) & 1u)) == 0))
						M.push_back(Mod(LOCCTR - 3, 5));
					recoder.insert(ins, 4);
				} else if (f == '-') {
					fscanf(middle, "%s", buff_t);
					LOCCTR += strlen(buff_t)/2;
					recoder.insert(buff_t);
				} else {
					fprintf(stderr, "Invaild immediate file syntax, aborting...\n%c\n", f);
					return;
				}
			}
			recoder.flush();
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
		bool p1_read_line()
		{
			while (true) {

				read_line(in);

				// Terminates at end.
				if (feof(in)) return false;
				// Skips comment lines & empty lines.
				if (buff[0] == '.' || buff[0] == '\0') continue;
				// Checks fix format.
				if (buff[8] != ' ' || 
						(buff[15] != '\0' && buff[15] != ' ') ||
						(buff[16] != '\0' && buff[16] != ' ')) {
					print_line();
					fprintf(stderr, "## Invalid character (should be space).\n");
					continue;
				}

				// Resets the line.
				memset(&line, 0, sizeof(line));

				// Stores the label if present.
				for (int i=0, j=0; i<8 && buff[i]; ++i) {
					if (buff[i] == ' ') continue;
					if ((j == 0 && !isalpha(buff[i])) || !isalnum(buff[i])) {
						print_line();
						fprintf(stderr, "## Invalid label.\n");
					}
					line.label[j++] = buff[i];
				}

				// Stores the opcode.
				for (int i=9, j=0; i<15 && buff[i]; ++i) {
					if (j == 0 && buff[i] == '+') {
						line.ext = true;
						continue;
					} else if (buff[i] == ' ') continue;
					line.mnem[j++] = buff[i];
				}
				printf("test: %s\n", line.mnem);
				printf("%s\n", buff);

				Optab::iterator it = OPTAB.find(Opcode(line.mnem));
				if (it == OPTAB.end()) {
					print_line();
					fprintf(stderr, "## Invalid OPCODE.\n");
					continue;
				}

				line.format = it->format;
				if (line.ext) {
					if (line.format == 3) line.format = 4;
					else {
						print_line();
						fprintf(stderr, "## Extended format cannot be used.\n");
						line.ext = false;
					}
				}
				line.opcode = it->opcode;


				// Stores the operands.

				bool l_error = false;
				for (int i=17, j=0; i<35 && buff[i]; ++i) {
					if (buff[i] == ' ') continue;
					if (j==0) {
						if (buff[i] == '#') {
							line.immediate = true;
							continue;
						}
						if (buff[i] == '@') {
							line.indirect = true;
							continue;
						}
						// Special processing for literals.
						if (buff[i] == '=') {
							line.literal = true;
							if (buff[++i] == 'C') {
								if (!store_c(buff+i, line.operand[0], 35-i+1-3))
									l_error = true;
							} else if (buff[i] == 'X') {
								if (!store_x(buff+i, line.operand[0], 35-i+1-3))
									l_error = true;
							} else if (buff[i] == '*' && buff[i+1] == ' ') {
								sprintf(line.operand[0], "%06X", LOCCTR);
							} else
								l_error = true;

							if (l_error) {
								print_line();
								fprintf(stderr, "## Invalid literal.\n");
							} else {
								unsigned i;
								for (i=0; i<LITTAB.size(); ++i) {
									if (LITTAB[i].bytes == line.operand[0])
										break;
								}
								if (i==LITTAB.size())
									LITTAB.push_back(Literal(0, line.operand[0]));
								line.operand[0][0] = '=';
								sprintf(&line.operand[0][1],
										"%06X", i);
								line.operand[0][5] = '\0';
							}
							break;
						}
					}
					if (buff[i] == ',') {
						if (line.format == 2) {
							for (++i, j=0; i<35 && buff[i]; ++i, ++j)
								line.operand[1][j] = buff[i];
							if (j==0) {
								print_line();
								fprintf(stderr, "## Syntax error.\n");
							}
						} else if (buff[i+1] != 'X' || buff[i+2] != ' ') {
							print_line();
							fprintf(stderr, "## Syntax error.\n");
							break;
						} else if (line.immediate || line.indirect) {
							print_line();
							fprintf(stderr, "## Index cannot be used.\n");
							break;
						} else {
							line.index = true;
							break;
						}

					}
					line.operand[0][j++] = buff[i];
				}
				if (l_error) continue;
				else break;
			}
			if (line.format == 2) {
				if (strcmp("SW", line.operand[0]) == 0)
					line.operand[0][0] = 'W';
				if (strcmp("SW", line.operand[1]) == 0)
					line.operand[1][0] = 'W';
			}

			return true;
		}
		void p1_write_line()
		{
			if (line.format) {
				if (line.format > 2) fprintf(middle, "%d %02X %1x %1x %1x %s\n",
						line.format, line.opcode, line.indirect, line.immediate,
						line.index, line.operand[0]);
				else if (line.format == 2) fprintf(middle, "2 %02X %x %x\n",
						line.opcode, REG[line.operand[0][0]], REG[line.operand[1][0]]);
				else fprintf(middle, "1 %02X\n", line.opcode);

			} else if (strcmp(line.mnem, "BYTE") == 0) {
				fprintf(middle, "-%s\n", line.operand[1]);
			} else
				fprintf(middle, "0 %s %s\n", line.mnem, line.operand[0]);
		}
		void p1_write_byte()
		{
			write_byte(line.operand[0]);
		}
		void write_byte(const char *s)
		{
			fprintf(middle, "- %s\n", s);
		}
		void read_line(FILE *f)
		{
			memset(buff, 0, sizeof(buff));
			fgets(buff_i, MAXL, f);
			for (int i=0; i<MAXL && buff_i[i]; ++i)
				if (buff_i[i] == '\n' || buff_i[i] == '\r')
					buff_i[i] = '\0';

			for (int i=0; buff_i[i]; ++i)
				if (islower(buff_i[i]))
					buff[i] = toupper(buff_i[i]);
				else
					buff[i] = buff_i[i];
		}
		void print_line()
		{
			fprintf(stderr, "%s\n", buff_i);
		}

		bool store_x(const char *from, char *to, int n)
		{
			if (*from != 'X' || *++from != '\'') return false;
			n/=2;

			int i=0;
			for (++from; i<n && *from != '\'' && *from != '\0'; ++i) {
				if (isdigit(*from))
					*to = *from;
				else if (*from >= 'A' && *from <= 'F')
					*to = *from;
				else
					return false;

				++to;
				++from;

				if (isdigit(*from))
					*to = *from;
				else if (*from >= 'A' && *from <= 'F')
					*to = *from;
				else
					return false;

				++to;
				++from;
			}
			if (i==0 || *from != '\'') return false;
			return true;
		}

		bool store_c(const char *from, char *to, int n)
		{
			if (*from != 'C' || *++from != '\'') return false;

			int i=0;
			for (++from; i<n && *from != '\'' && *from != '\0'; ++i) {
				unsigned char t = *from;
				t >>= 4;
				if (t<10) *to = t+'0';
				else *to = t-10+'A';
				++to;

				t = *from & 0xf;

				if (t<10) *to = t+'0';
				else *to = t-10+'A';

				++to;
				++from;
			}
			if (i==0 || *from != '\'') return false;
			return true;
		}
		Addr resolve(const char *&str)
		{
			Addr operl = resolveT(str);
			while (*str == '+' || *str == '-') {
				char op = *(str++);
				Addr operr = resolveT(str);
				if (op == '+')
					operl += operr;
				else
					operl -= operr;
			}
			return operl;
		}
		Addr resolveT(const char *&str)
		{
			Addr operl = resolveF(str);
			while (*str == '*' || *str == '/') {
				char op = *(str++);
				Addr operr = resolveF(str);
				if (op == '*')
					operl *= operr;
				else
					operl /= operr;
			}
			return operl;
		}
		Addr resolveF(const char *&str)
		{
			if (*str == '(') {
				++str;
				Addr res = resolve(str);
				if (*str != ')') res.error = true;
				else ++str;
				return res;
			} else
				return resolveN(str);
		}
		Addr resolveN(const char *&str)
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
					res = resolve(i);
				} else {
					res.relative = !it->second.is_absolute();
					res.addr = it->second.value;
					if (it->second.is_external()) {
						res.ext.push_back(AddrRef(t, true));
					}
				}
			} else if (*str != '\0')
				res.error = true;
			return res;
		}
};

// ------------------------------------------------------------------ //
int main(int argc, char *argv[])
{
	FILE *in, *middle, *out;
	REG['A'] = 0;
	REG['X'] = 1;
	REG['L'] = 2;
	REG['B'] = 3;
	REG['S'] = 4;
	REG['T'] = 5;
	REG['F'] = 6;
	REG['P'] = 8;
	REG['W'] = 9;
	SIC[0x18] = true;
	SIC[0x40] = true;
	SIC[0x28] = true;
	SIC[0x24] = true;
	SIC[0x3c] = true;
	SIC[0x30] = true;
	SIC[0x34] = true;
	SIC[0x38] = true;
	SIC[0x48] = true;
	SIC[0x00] = true;
	SIC[0x50] = true;
	SIC[0x08] = true;
	SIC[0x04] = true;
	SIC[0x20] = true;
	SIC[0x44] = true;
	SIC[0xd8] = true;
	SIC[0x4c] = true;
	SIC[0x0c] = true;
	SIC[0x54] = true;
	SIC[0x14] = true;
	SIC[0xe8] = true;
	SIC[0x10] = true;
	SIC[0x1c] = true;
	SIC[0xe0] = true;
	SIC[0x2c] = true;
	SIC[0xdc] = true;


	if (argc != 2) {
		fprintf(stderr, "usage: sicxeasm INPUTFILE\n");
		return 1;
	}

	// Opens the files.
	in = fopen(argv[1], "r");
	if (in == 0) {
		fprintf(stderr, "Error: Cannot open file \"%s\"\n", argv[1]);
		return 1;
	}

	middle = fopen("middle.out", "w+");
	if (middle == 0) {
		fprintf(stderr, "Error: Cannot open file \"middle.out\"\n");
		return 1;
	}

	out = fopen("output.obj", "w");
	if (middle == 0) {
		fprintf(stderr, "Error: Cannot open file \"output.obj\"\n");
		return 1;
	}

	Assembler sicasm(in, middle, out);
	sicasm.assemble();

	fclose(in);
	fclose(middle);
	fclose(out);
	return 0;
}
// ------------------------------------------------------------------ //

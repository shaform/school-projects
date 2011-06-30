#include <cstdio>
#include <string>
#include <cstring>
#include <set>
#include <map>

#include "sicxeasm.h"
#include "textrecord.h"
using namespace std;

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
const int MAXPG = 6;

class Assembler {
	public:
		Assembler(FILE *i, FILE *m, FILE *o) : in(i), middle(m), out(o), nobase(true) {}
		void assemble()
		{
			pass1();
			rewind(middle);
			pass2();
		}

	private:
		FILE *in, *middle, *out;
		Line line;
		map<string, addr_t> SYMTAB;
		char prg_name[MAXPG+1], sec_name[MAXPG+1];
		char buff_t[MAXL];

		addr_t start_addr, LOCCTR;
		addr_t prg_length;
		addr_t base;
		bool nobase;

		// pass1
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
			} else
				start_addr = LOCCTR = 0;

			while (!(!line.format && strcmp(line.mnem, "END") == 0)) {
				if (line.label[0]) {
					if (SYMTAB.find(line.label) != SYMTAB.end()) {
						print_line();
						fprintf(stderr, "## Duplicate symbol %s.\n", line.label);
					} else {
						SYMTAB.insert(make_pair(line.label, LOCCTR));
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
					}
				}

				p1_write_line();
				p1_read_line();
			}

			p1_write_line();
			prg_length = LOCCTR - start_addr;

		}
		void pass2()
		{
			LOCCTR = start_addr;
			fprintf(out, "H%s%06X%06X\n", prg_name, start_addr, prg_length);
			Recoder recoder(start_addr, out);
			char f;
			while (fscanf(middle, " %c", &f) == 1) {
				LOCCTR += f-'0';
				if (f == '0') {
					read_line(middle);
					int t;
					sscanf(buff, "%s%d", buff_t, &t);
					if (strcmp(buff_t, "RESW") == 0) {
						recoder.res(t*3);
					} else if (strcmp(buff_t, "RESB") == 0) {
						recoder.res(t);
					} else if (strcmp(buff_t, "NOBASE") == 0) {
						nobase = true;
					} else if (strcmp(buff_t, "BASE") == 0) {
						sscanf(buff, "%s%s", &buff_t, &buff_t);
						map<string, addr_t>::iterator it = SYMTAB.find(buff_t);
						if (it!=SYMTAB.end()) {
							nobase = false;
							base = it->second;
						} else 
							fprintf(stderr, "##Undefined symbol: %s\n", buff_t);
					} else if (strcmp(buff_t, "END") == 0) {
						recoder.flush();
						sscanf(buff, "%s%s", &buff_t, &buff_t);
						map<string, addr_t>::iterator it = SYMTAB.find(buff_t);
						if (it!=SYMTAB.end())
							fprintf(out, "E%06X\n", it->second);
						else 
							fprintf(stderr, "##Undefined symbol: %s\n", buff_t);

					}
					// BASE RESW RESB END
				} else if (f == '1') {
					fscanf(middle, "%s", buff_t);
					recoder.insert(buff_t);
				} else if (f == '2') {
					read_line(middle);
					addr_t op = 0, r1 = 0, r2 = 0;
					sscanf(buff, "%X%X%X", &op, &r1, &r2);
					recoder.insert((op << 14) | (r1 << 8) | r2, 2);
				} else if (f == '3') {
					read_line(middle);
					addr_t op = 0, n = 0, i = 0, x = 0;
					sscanf(buff, "%X %X%X%X %s", &op, &n, &i, &x, buff_t);
					map<string, addr_t>::iterator it = SYMTAB.find(buff_t);
					if (!(n|i)) op |= 3;
					printf("insert started.\n");
					recoder.insert((op << 16) | (n << 17) | (i << 16) | (x << 15) | calc_addr(it->second, false), 3);
					printf("insert ended.\n");
				} else if (f == '4') {
					read_line(middle);
					addr_t op = 0, n = 0, i = 0, x = 0;
					sscanf(buff, "%X%X%X%X %s", &op, &n, &i, &x, buff_t);
					map<string, addr_t>::iterator it = SYMTAB.find(buff_t);
					if (!(n|i)) op |= 3;
					if (!(n|i)) op |= 3;
					printf("4op :%02x\n", op);
					recoder.insert((op << 24) | (n << 25) | (i << 24) | (x << 23) | calc_addr(it->second, true), 4);
				} else if (f == '-') {
					fscanf(middle, "%s", buff_t);
					recoder.insert(buff_t);
				} else {
					fprintf(stderr, "Invaild immediate file syntax, aborting...\n%c\n", f);
					return;
				}
			}
			recoder.flush();
		}

		addr_t calc_addr(addr_t dest, bool ext)
		{
			if (ext) {
				return (1 << 20) | dest;
			} else if (dest <= LOCCTR + 2047 && dest + 2048 >= LOCCTR) {
				printf("wow %x\n", dest-LOCCTR);
				return (1 << 13) | ((dest-LOCCTR) & ((1u<<12)-1u));
			} else if (!nobase && dest >= base && dest <= base + 4095) {
				return (1 << 14) | (dest-base);
			} else {
				fprintf(stderr, "## Address out of range.\n");
				return 1 << 13;
			}
		}
		char buff_i[MAXL], buff[MAXL];
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
							} else
								l_error = true;

							if (l_error) {
								print_line();
								fprintf(stderr, "## Invalid literal.\n");
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

			return true;
		}
		void p1_write_line()
		{
			if (line.format) {
				if (line.format > 2) fprintf(middle, "%d %02X %1x %1x %1x %s\n",
						line.format, line.opcode, line.indirect, line.immediate,
						line.index, line.operand[0]);
				else if (line.format == 2) fprintf(middle, "2 %02X %s %s\n",
						line.opcode, line.operand[0], line.operand[1]);
				else fprintf(middle, "1 %02X\n", line.opcode);

			} else if (strcmp(line.mnem, "BYTE") == 0) {
				fprintf(middle, "-%s\n", line.operand[1]);
			} else
				fprintf(middle, "0 %s %s\n", line.mnem, line.operand[0]);
		}
		void p1_write_byte()
		{
			fprintf(middle, "- %s\n", line.operand[0]);
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
};


// ------------------------------------------------------------------ //
int main(int argc, char *argv[])
{
	FILE *in, *middle, *out;

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

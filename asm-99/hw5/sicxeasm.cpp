#include <cstdio>
#include <string>
#include <cstring>
#include <set>
#include <map>

#include "sicxeasm.h"
using namespace std;

struct Line {
	char label[9];
	char mnem[7];
	char operand[2][19];

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

class Assembler {
	public:
		Assembler(FILE *i, FILE *m, FILE *o) : in(i), middle(m), out(o) {}
		void assemble()
		{
			pass1();
			//pass2();
		}

	private:
		FILE *in, *middle, *out;
		Line line;
		map<string, addr_t> SYMTAB;

		addr_t start_addr, LOCCTR;
		addr_t prg_length;
		// pass1
		void pass1()
		{
			p1_read_line();
			if (line.format == 0 && strcmp(line.mnem, "START") == 0) {
				sscanf(line.operand[0], "%d", &start_addr);
				LOCCTR = start_addr;
				p1_write_line();
			} else
				start_addr = LOCCTR = 0;

			while (!(!line.format && strcmp(line.mnem, "END") == 0)) {
				if (line.label[0]) {
					if (SYMTAB.find(line.label) != SYMTAB.end()) {
						fprintf(stderr, "Error: duplicate symbol %s.", line.label);
					} else {
						SYMTAB.insert(make_pair(line.label, LOCCTR));
					}
				}
				//FIXME
				LOCCTR += line.format;

				p1_write_line();
				p1_read_line();
			}

			p1_write_line();
			prg_length = LOCCTR - start_addr;
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
								if (buff[++i] != '\'') l_error = true;
								while (buff[++i] != '\'' && i<35)
									line.operand[0][j++] = buff[i];
								if (i >= 35 || j==0) l_error = true;
							} else if (buff[i] == 'X') {
								if (buff[++i] != '\'') l_error = true;
								++i;
								while (buff[i] != '\'' && i<35) {
									if (isdigit(buff[i]))
										line.operand[0][j] = (buff[i]-'0') << 4;
									else if (buff[i] >= 'A' && buff[i] <= 'F')
										line.operand[0][j] = (buff[i]-'A') << 4;
									else
										l_error = true;

									if (isdigit(buff[i+1]))
										line.operand[0][j] |= (buff[i]-'0');
									else if (buff[i+1] >= 'A' && buff[i+1] <= 'F')
										line.operand[0][j] |= (buff[i]-'A');
									else
										l_error = true;
									++j;
									i+=2;
								}
								if (i >= 35 || j == 0) l_error = true;
							} else
								l_error = true;

							if (l_error) {
								print_line();
								fprintf(stderr, "## Invalid literal.");
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
								fprintf(stderr, "## Syntax error.");
							}
						} else if (buff[i+1] != 'X' || buff[i+2] != ' ') {
							print_line();
							fprintf(stderr, "## Syntax error.");
						} else if (line.immediate || line.indirect) {
							print_line();
							fprintf(stderr, "## Index cannot be used.");
						} else
							line.index = true;

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

			} else
				fprintf(middle, "0 %s %s\n", line.mnem, line.operand[0]);
		}
		void p1_write_byte()
		{
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

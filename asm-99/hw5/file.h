#ifndef SIC_FILE_H
#define SIC_FILE_H
#include <cstdio>
#include "sicxeasm.h"
const int MAXOPER = 40;

struct Line {
	char label[9];
	char mnem[7];
	char oper[2][MAXOPER+1];

	unsigned opcode;
	char format;

	bool idi;
	bool imm;
	bool ext;
	bool idx;
	bool lit;
	bool op(const char *s) { return strcmp(mnem, s) == 0; }
	bool has_label() { return label[0]; }
};
// The file handling class.
class FileHandler {
	public:

		FileHandler() : in(0), mid(0), out(0) { }
		~FileHandler() { close(); }
		bool open(const char *fi, const char *fm = "imme.out", const char *fo = "output.obj")
		{
			in = fopen(fi, "r");
			if (in == 0) {
				fprintf(stderr, "Error: Cannot open file \"%s\"\n", fi);
				return false;
			}
			mid = fopen(fm, "w+");
			if (mid == 0) {
				fprintf(stderr, "Error: Cannot open file \"%s\"\n", fm);
				close();
				return false;
			}

			out = fopen(fo, "w");
			if (out == 0) {
				fprintf(stderr, "Error: Cannot open file \"%s\"\n", fo);
				close();
				return false;
			}
			return true;
		}
		void close()
		{
			printf("closing\n");
			if (in) { fclose(in); in = 0; }
			if (mid) { fclose(mid); mid = 0; }
			if (out) { fclose(out); out = 0; }
		}
		void rwmid()
		{
			rewind(mid);
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

				for (int i=17, j=0; i<35 && buff[i]; ++i) {
					if (buff[i] == ' ') continue;
					if (j==0) {
						if (buff[i] == '#') {
							line.imm = true;
							continue;
						}
						if (buff[i] == '@') {
							line.idi = true;
							continue;
						}
					}
					if (buff[i] == ',') {
						if (line.format == 2) {
							for (++i, j=0; i<35 && buff[i]; ++i, ++j)
								line.oper[1][j] = buff[i];
							if (j==0) {
								print_line();
								fprintf(stderr, "## Syntax error.\n");
							}
						} else if (buff[i+1] != 'X' || buff[i+2] != ' ') {
							print_line();
							fprintf(stderr, "## Syntax error.\n");
							break;
						} else if (line.imm || line.idi) {
							print_line();
							fprintf(stderr, "## Index cannot be used.\n");
							break;
						} else {
							line.idx = true;
							break;
						}

					}
					line.oper[0][j++] = buff[i];
				}
				break;
			}
			// FIXME: a hack for SW register.
			if (line.format == 2) {
				if (strcmp("SW", line.oper[0]) == 0)
					line.oper[0][0] = 'W';
				if (strcmp("SW", line.oper[1]) == 0)
					line.oper[1][0] = 'W';
			}
			return true;
		}
		void p1_write_byte()
		{
			p1_write_byte(line.oper[0]);
		}
		void p1_write_byte(const char *s)
		{
			fprintf(mid, "- %s\n", s);
		}
		void p1_write_line()
		{
			if (line.format) {
				if (line.format > 2) fprintf(mid, "%d %02X %1x %1x %1x %s\n",
						line.format, line.opcode, line.idi, line.imm,
						line.idx, line.oper[0]);
				else if (line.format == 2) fprintf(mid, "2 %02X %x %x\n",
						line.opcode, REG[line.oper[0][0]], REG[line.oper[1][0]]);
				else fprintf(mid, "1 %02X\n", line.opcode);

			} else if (strcmp(line.mnem, "BYTE") == 0) {
				fprintf(mid, "- %s\n", line.oper[1]);
			} else
				fprintf(mid, "0 %s %s\n", line.mnem, line.oper[0]);
		}
		bool p2_read_line()
		{
			read_line(mid);
			// Resets the line.
			memset(&line, 0, sizeof(line));
			if (buff[0] == '-') {
				line.format = 0;
				strcpy(line.mnem, "-");
				sscanf(&buff[2], "%s", line.oper[0]);
			} else {
				line.format = buff[0] - '0';
				if (line.format == 0) {
					sscanf(&buff[2], "%s %s", line.mnem, line.oper[0]);
				} else if (line.format == 1) {
					sscanf(&buff[2], "%s", line.mnem);
				} else if (line.format == 2) {
					sscanf(&buff[2], "%s %s %s", line.mnem, line.oper[0], line.oper[1]);
				} else {
					int n, i, x;
					sscanf(&buff[2], "%s %d %d %d %s", line.mnem, &n, &i, &x, line.oper[0]);
					line.idi = n;
					line.imm = i;
					line.idx = x;
				}
				sscanf(line.mnem, "%X", &line.opcode);
			}
			return true;
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
		bool p1_eof()
		{
			return feof(in);
		}
		bool p2_eof()
		{
			return feof(mid);
		}
		Line *next_line();
		Line *get_line() { return &line; }

		FILE *in, *mid, *out;
	private:
		Line line;
		// Temporary variables.
		char buff_i[MAXL], buff[MAXL];
		char buff_t[MAXL];
};
#endif

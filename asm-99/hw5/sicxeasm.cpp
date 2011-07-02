#include <cstdio>
#include <cstring>
#include <string>
#include <algorithm>
#include <set>
#include <map>

#include "sicxeasm.h"
#include "textrecord.h"
#include "addr.h"
#include "symbol.h"
#include "file.h"
#include "csect.h"

using namespace std;


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
		Assembler(FileHandler &fh) : fh(fh) { l = fh.get_line(); }
		void assemble()
		{
			pass1();
			fh.rwmid();
			pass2();
		}
	private:
		friend class CSection;
		FileHandler &fh;
		Line *l;

		// Internal data for assembly
		vector<CSection> css;
		void pass1()
		{
			fh.p1_read_line();
			if (l->op("START")) {
				css.push_back(CSection(&fh, l->label, l->oper[0]));
				fh.p1_read_line();
			} else
				css.push_back(CSection(&fh));
			css.back().main_sect = true;

			while (css.back().pass1()) {
				if (l->op("CSECT")) {
					if (css[0].start_addr == 0u) {
						css.back().more_sect = true;
						css.push_back(CSection(&fh, l->label, "0"));
						fh.p1_read_line();
						continue;
					}
					fprintf(stderr, "## Control section supported only in relocatable program.\n");
				} else if (!l->op("END")) {
					fprintf(stderr, "## File ended unexpectedly, no END directive found.\n");
				}
				break;
			}
			css[0].end_str = l->oper[0];

		}
		void pass2()
		{
			for (vector<CSection>::iterator it = css.begin(); it != css.end(); ++it) {
				fh.p2_read_line();
				it->pass2();
			}
		}
};

// ------------------------------------------------------------------ //
int main(int argc, char *argv[])
{
	// FIXME: this is a hack of register translation & SIC compatibility.
	// It's ugly and should be changed.
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

	// Open the files.
	FileHandler fh;
	if (!fh.open(argv[1]))
		return 1;

	// Assemble it.
	Assembler(fh).assemble();

	return 0;
}
// ------------------------------------------------------------------ //
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

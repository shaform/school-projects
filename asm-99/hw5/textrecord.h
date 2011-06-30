#ifndef TR_H
#define TR_H
#include <cstdio>
#include "sicxeasm.h"

const int MAXTR = 30;
struct TextRecord {
	addr_t start_addr;
	int cursor;
	char obj[MAXTR*2+1];
	void insert();
	bool full() { return cursor == MAXTR*2; }
	bool full(addr_t sz) { return cursor+sz*2 > MAXTR*2; }
};
class Recoder {
	public:
		TextRecord tr;
		FILE *output;
		void res(addr_t sz) {
			flush();
			tr.start_addr += sz;
		}
		void insert(addr_t d, addr_t len)
		{
			if (tr.full(len)) flush();
			while (len--) {
				if (tr.full()) flush();
				sprintf(&tr.obj[tr.cursor], "%02X", (d >> 8*len) & 0xff);
				printf("output: %02X\n", (d >> 8*len) & 0xff);
				tr.cursor += 2;
				tr.obj[tr.cursor] = '\0';
			}
		}
		void insert(char *s)
		{
			insert(s, 0);
		}
		void insert(char *s, addr_t len)
		{
			if (tr.full(len)) flush();
			for (int i=0; s[i]; ++i) {
				if (tr.full()) flush();
				tr.obj[tr.cursor++] = s[i];
				tr.obj[tr.cursor] = '\0';
			}
		}
		void flush()
		{
			if (tr.cursor) {
				print();
				advance();
			}
		}
		Recoder(addr_t s, FILE *out)
			: output(out)
		{
			tr.cursor = 0;
			tr.start_addr = s;
			tr.obj[0] = '\0';
		}
	private:
		void print()
		{
			fprintf(output, "T%06X%02X%s\n", tr.start_addr, tr.cursor/2, tr.obj);
		}
		void advance()
		{
			tr.start_addr += tr.cursor/2;
			tr.cursor = 0;
			tr.obj[0] = '\0';
		}
};
#endif

#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>

using namespace std;
const size_t SZ = 1024;

char dt[SZ];
const char ct[SZ] = "an const string.";
char it[SZ] = "an initialized string.";

void func()
{
	printf("This is a function.\n");
}

typedef unsigned long long int addr_t;
int main()
{
	char st[SZ];
	char *hp = static_cast<char *>(malloc(SZ*sizeof(char)));

	FILE *pmap = fopen("/proc/self/maps", "r");

	extern char etext, edata, end;
	void *eheap = sbrk(0);

	memset(dt, 0xff, sizeof(dt));
	memset(st, 0xff, sizeof(st));
	memset(hp, 0xff, SZ*sizeof(char));

	printf("---------Virtual addresses of variables---------\n");
	printf("&ct[0] = %012llx\n", (addr_t)&ct[0]);
	printf("&it[0] = %012llx\n", (addr_t)&it[0]);
	printf("&dt[0] = %012llx\n", (addr_t)&dt[0]);
	printf("&st[0] = %012llx\n", (addr_t)&st[0]);
	printf("&hp[0] = %012llx\n", (addr_t)&hp[0]);
	printf("\n\n---------Virtual addresses of functions---------\n");
	printf("&main = %012llx\n", (addr_t)&main);
	printf("&func = %012llx\n", (addr_t)&func);
	printf("&printf = %012llx\n", (addr_t)&printf);
	printf("&scanf = %012llx\n", (addr_t)&scanf);
	printf("\n\n---------Virtual addresses of C++ iostreams---------\n");
	printf("&cin = %012llx\n", (addr_t)&cin);
	printf("&cout = %012llx\n", (addr_t)&cout);
	printf("\n\n---------Addresses of program segments---------\n");
	printf("First address past:\n");
	printf("program text -> %012llx\n", (addr_t)&etext);
	printf("initialized data -> %012llx\n", (addr_t)&edata);
	printf("uninitialized data -> %012llx\n", (addr_t)&end);
	printf("heap -> %012llx\n", (addr_t)eheap);
	printf("\n\n---------Memory map---------\n");
	char c;
	while ((c = getc(pmap)) != EOF)
		putchar(c);

	fclose(pmap);
	free(hp);
	return 0;
}


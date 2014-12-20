// Assignment 4 : Implementation of hash function SHA-1
//
// Environment: linux 2.6.34-gentoo-r6 x86_64
//              with UTF-8 locale
//
// Editor: vim
// Compiler: g++ version 4.4.3 (Gentoo 4.4.3-r2 p1.2)
//
// Compile and Execute:
// # g++ hw4.cpp
// # ./a.out
//
#include <cstdio>
#include <cstring>
#include <iostream>
using namespace std;

const unsigned int MASK = 0xffffffff;
const unsigned int H_i[5] = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0};

unsigned int H[5], W[80], A, B, C, D, E;
bool next, ending;
struct Block {
	unsigned char mem[64*2];
	unsigned long long len;
	int it;
} bk;


unsigned int rotl(unsigned int n, int s)
{
	n &= MASK;
	return (((n << s) & MASK) | (n >> (32-s))) & MASK;
}
unsigned int f(unsigned int B, unsigned int C, unsigned int D, int t)
{
	B &= MASK, C &= MASK, D &= MASK;
	if (t <= 19)
		return (B & C) | (((~B) & MASK) & D);
	else if (t <= 39)
		return B^C^D;
	else if (t <= 59)
		return (B & C) | (B & D) | (C & D);
	else
		return B^C^D;
}
unsigned int k(int t)
{
	if (t <= 19)
		return 0x5A827999;
	else if (t <= 39)
		return 0x6ED9EBA1;
	else if (t <= 59)
		return 0x8F1BBCDC;
	else
		return 0xCA62C1D6;
}
void get_block()
{
	if (ending) {
		for (int i=0; i<64; ++i)
			bk.mem[i] = bk.mem[i+64];
		next = false;
		return;
	}
	int c;
	for (bk.it = 0; bk.it < 64; ++bk.it) {
		c = getchar();
		if (c == '\n' || c == EOF) {
			ending = true;
			break;
		}
		bk.mem[bk.it] = c;
		bk.len += 8;
	}
	if (ending) {
		// Input ended, do sha-1-pad
		int d = (447 + 512 - (bk.len % 512)) % 512;
		int n = (d + 1)/8 - 1;
		bk.mem[bk.it++] = 0x80;
		while (n--)
			bk.mem[bk.it++] = 0u;
		for (n = 7; n >= 0; --n)
			bk.mem[bk.it++] = (bk.len >> (n*8)) & 0xFF;
		// if (bk.it % 64 != 0) printf("error");
		if (bk.it == 64) next = false;
	}
}
void solve()
{
	bk.len = 0;  // Reset message length
	next = true;
	ending = false;
	for (int i=0; i<5; ++i)
		H[i] = H_i[i];

	do {
		get_block();
		// SHA-1 algorithm
		for (int i=0; i<16; ++i) {
			W[i] = 0u;
			for (int j=0; j<4; ++j)
				W[i] |= bk.mem[i*4+j] << (8*(3-j));
		}
		for (int i=16; i<80; ++i)
			W[i] = rotl(W[i-3]^W[i-8]^W[i-14]^W[i-16], 1);
		A = H[0], B = H[1], C = H[2], D = H[3], E = H[4];
		for (int i=0; i<80; ++i) {
			unsigned int temp = (rotl(A, 5) + f(B, C, D, i) + E + W[i] + k(i)) & MASK;
			E = D, D = C, C = rotl(B, 30), B = A, A = temp;
		}
		H[0] += A, H[1] += B, H[2] += C, H[3] += D, H[4] += E;
		H[0] &= MASK, H[1] &= MASK, H[2] &= MASK, H[3] &= MASK, H[4] &= MASK;
	} while (next);
}
void print_answer()
{
	printf("%08x %08x %08x %08x %08x\n", H[0], H[1], H[2], H[3], H[4]);
}

int main()
{
	do {
		printf("Enter message: ");
		solve();
		print_answer();
		printf("\n");
	} while (!feof(stdin));
}

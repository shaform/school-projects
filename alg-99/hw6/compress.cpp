#include <cstdio>
#include <functional>
using namespace std;
// ------------------------------------------------------------------ //

/** File structure
 *
 * CMA --- Type A compression (normal)
 * |--C--|--M--|--A--|-NUM-|-char-|--- freq ---| --- TOTAL LENGTH --- |
 *                         |<-   NUM items   ->|

 * |---------------Encoded Data-----------------|
 *
 *
 *
 * CMB --- Type B compression (empty file or only 1 kind of byte)
 *
 * |--C--|--M--|--B--|-------length-------|char-|
 *
**/

// ------------------------------------------------------------------ //

const int MAX = 300;
const int MAXT = 600;

// ------------------------------------------------------------------ //
template <typename T, typename CMP = less<T> >
struct Heap {
	int size() { return sz; }

	Heap() : sz(0), cmp(CMP()) {}
	Heap(CMP c) : sz(0), cmp(c) {}


	void insert(T u)
	{
		arr[sz] = u;
		heapify_up(sz);
		++sz;
	}
	int pop()
	{
		T u = arr[0];
		arr[0] = arr[--sz];
		heapify_down(0);
		return u;
	}
private:
	CMP cmp;
	T arr[MAX];
	int sz;

	void heapify_down(int u)
	{
		int l = u*2+1, r = u*2+2;

		int maxi = u;

		if (l < sz && cmp(arr[l], arr[maxi]))
			maxi = l;
		if (r < sz && cmp(arr[r], arr[maxi]))
			maxi = r;

		if (maxi != u) {
			T t = arr[u];
			arr[u] = arr[maxi];
			arr[maxi] = t;
			heapify_down(maxi);
		}
	}
	void heapify_up(int u)
	{
		int p = (u-1)/2;


		if (p >= 0 && cmp(arr[u], arr[p])) {
			T t = arr[p];
			arr[p] = arr[u];
			arr[u] = t;
			heapify_up(p);
		}
	}

};
// ------------------------------------------------------------------ //

unsigned freq[MAX];
unsigned code[MAX];
unsigned len[MAX];
unsigned dic_size, max_freq, total_freq;

struct Node {
	unsigned freq;
	int left;
	int right;
	unsigned char ch;

	Node() {}
	Node(unsigned fr, int c) : freq(fr), left(0), right(0), ch(c) {}
	Node(unsigned fr, int l, int r) : freq(fr), left(l), right(r) {}
	bool is_leaf() { return left == right; }
};
union NUM {
	unsigned num;
	unsigned char ch[4];
};

struct Huffman {
	Node nd[MAXT];
	int sz;
	void insert(const Node &u) { nd[sz++] = u; }
	int size() { return sz; }
	Node &operator[](int i) { return nd[i]; }
} huff;


struct Cmp {
	bool operator()(int a, int b) const { return huff[a].freq < huff[b].freq; }
};

Heap<int, Cmp> heap;



// ------------------------------------------------------------------ //

static void init_freq(FILE *in)
{
	int c;
	while ((c = fgetc(in)) != EOF)
		++freq[c];
}
static void sub_gen(int u, unsigned c, unsigned n)
{
	if (huff[u].is_leaf()) {
		unsigned char ch = huff[u].ch;
		code[ch] = c;
		len[ch] = n;
	} else {
		sub_gen(huff[u].left, c << 1, n+1);
		sub_gen(huff[u].right, (c << 1) | 1u, n+1);
	}
}
static void gentree()
{
	// Initializes frequency nodes.
	for (int i=0; i<MAX; ++i) {
		if (freq[i]) {
			huff.insert(Node(freq[i], i));
			heap.insert(huff.size()-1);

			++dic_size;
			total_freq += freq[i];
			if (max_freq < freq[i]) max_freq = freq[i];
		}
	}
	// Builds Huffman tree.
	while (heap.size() > 1)
	{
		int l = heap.pop(), r = heap.pop();
		huff.insert(Node(huff[l].freq + huff[r].freq, l, r));
		heap.insert(huff.size()-1);
	}
}
static void gencode()
{

	// Generates code.
	if (heap.size() == 1)
		sub_gen(heap.pop(), 0, 0);
}
static void writeheader(FILE *out)
{
	fprintf(out, "CM");
	if (dic_size > 1) {
		fputc('A', out);
		fputc(dic_size-1, out);
		NUM u;
		for (int i=0; i<MAX; ++i) {
			if (freq[i]) {
				fputc(i, out);
				u.num = freq[i];
				for (int j=0; j<4; ++j)
					fputc(u.ch[j], out);
			}
		}
		u.num = total_freq;
		for (int i=0; i<4; ++i)
			fputc(u.ch[i], out);
	} else {
		fputc('B', out);
		NUM u;
		u.num = max_freq;
		// Writes the length of the file.
		for (int i=0; i<4; ++i)
			fputc(u.ch[i], out);
		// Writes the only character.
		if (max_freq)
			fputc(huff[0].ch, out);
	}
}
static void writebody(FILE *in, FILE *out)
{
	if (dic_size <= 1) return;

	int c;
	unsigned w = 0, sz = 0;
	while ((c = fgetc(in)) != EOF) {
		sz += len[c];
		w <<= len[c];
		w |= code[c];


		while (sz >= 8) {
			fputc((w >> (sz-8)) & 0xff, out);
			sz -= 8;
		}
	}
	// Writes the end.
	if (sz) {
		fputc((w << (8-sz)) & 0xff, out);
	}
}


// ------------------------------------------------------------------ //




// ------------------------------------------------------------------ //


static void compress(FILE *in, FILE *out)
{
	// Initializes frequency table.
	init_freq(in);

	// Generates Huffman tree.
	gentree();

	// Generates Huffman code.
	gencode();

	// Writes the compress header.
	writeheader(out);

	// Starts compressing file.
	fseek(in, 0, SEEK_SET);
	writebody(in, out);
}

static bool extract(FILE *in, FILE *out)
{
	// Checks file syntax.
	if (fgetc(in) != 'C' || fgetc(in) != 'M')
		return false;
	int c = fgetc(in), d, e;
	NUM u;
	if (c == 'A') {
		// Type A archive.
		c = fgetc(in);
		if (c == EOF) return false;
		for (int i=0; i<=c; ++i) {
			d = fgetc(in);
			if (d == EOF) return false;
			for (int j=0; j<4; ++j) {
				e = fgetc(in);
				if (e == EOF) return false;
				u.ch[j] = e;
			}
			freq[d] = u.num;
		}
		for (int i=0; i<4; ++i) {
			d = fgetc(in);
			if (d == EOF) return false;
			u.ch[i] = d;
		}
		gentree();
		if (heap.size() == 0) return false;
		int root = heap.pop(), v = root;
		unsigned total = u.num, now = 0;
		while (now < total) {
			d = fgetc(in);
			if (d == EOF) return false;
			for (int i=7; i>=0 && now < total; --i) {
				if ((d >> i) & 1u)
					v = huff[v].right;
				else
					v = huff[v].left;
				if (huff[v].is_leaf()) {
					fputc(huff[v].ch, out);
					v = root;
					++now;
				}
			}
		}

		return true;
	} else if (c == 'B') {
		// Type B archive.
		for (int i=0; i<4; ++i) {
			c = fgetc(in);
			if (c == EOF) return false;
			u.ch[i] = c;
		}
		c = fgetc(in);
		if (c == EOF) return false;
		for (int i=0; i<u.num; ++i)
			fputc(c, out);

		return true;
	} else
		return false;
}
// ------------------------------------------------------------------ //
int main(int argc, char *argv[])
{
	FILE *in, *out;

	if (argc != 4) {
		fprintf(stderr, "usage: compress COMMAND INPUTFILE OUTPUTFILE\n"
				"commands:\n"
				"x   extract\n"
				"c   compress\n");
		return 1;
	}
	if (!(argv[1][0] == 'x' || argv[1][0] == 'c') || argv[1][1] != '\0') {
		fprintf(stderr, "Unknown command: %s.\n", argv[1]);
		return 1;
	}

	// Opens the files.
	in = fopen(argv[2], "rb");
	if (in == 0) {
		fprintf(stderr, "Error: Cannot open file \"%s\" for input.\n", argv[2]);
		return 1;
	}
	out = fopen(argv[3], "wb");
	if (out == 0) {
		fprintf(stderr, "Error: Cannot open file \"%s\" for output.\n", argv[3]);
		fclose(in);
		return 1;
	}

	// Starts operation.
	if (argv[1][0] == 'x') {
		if (!extract(in, out)) {
			fprintf(stderr, "Error: Corrupted archive.\n");
			fclose(in);
			fclose(out);
			return 1;
		}
	} else
		compress(in, out);

	fclose(in);
	fclose(out);
	return 0;
}
// ------------------------------------------------------------------ //

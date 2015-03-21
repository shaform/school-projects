// Assignment 2 : Implementation of Stream Cipher RC4
//
// Environment: linux1 2.6.34-gentoo-r6 x86_64
//              with UTF-8 locale
//
// Editor: vim
//
// Compile and Execute:
// # g++ hw2.cpp
// # ./a.out
//
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <iostream>
using namespace std;

const int MAX = 256;
const int BUFL = 16384;

unsigned char S[MAX];
int rci, rcj;
char buffer[BUFL];

// -- The RC4 algorithm -- //

void schedule(const char *str, int len)
{
	for (int i=0; i<256; ++i) {
		S[i] = i;
	}
	for (int i=0, j=0; i<256; ++i) {
		j = (j + S[i] + str[i%len]) % MAX;
		swap(S[i], S[j]);
	}
	rci = rcj = 0;
}

unsigned char rc4()
{
	rci = (rci + 1) % MAX;
	rcj = (rcj + S[rci]) % MAX;
	swap(S[rci], S[rcj]);
	int t = (S[rci] + S[rcj]) % MAX;
	return S[t];
}

// -- ----------------- -- //

bool line_end(int c)
{
	// Standard requires that in text mode, only '\n' would be read as line end.
	return c == '\n'/* || c == '\r' */;
}
void discard_line()
{
	int t;
	do {
		t = getchar();
	} while (line_end(t));
	ungetc(t, stdin);
}

// Test cases
struct Case {
	char func;
	const char *key;
	const char *input;
} cases[] = {
	{'E', "NiceNCTU", "PEACE!"},
	{'D', "Into the Woods.", "401f4d6e"},
	{'D', "I eat too much because I'm depressed, "
		"and I'm depressed because I eat too much. "
		"It's a vicious circle... "
		"that took years to perfect!",
	"26d058dc52f945e01b5da91c"},
	{'D', "What a computer is to me is the most remarkable "
		"tool that we have ever come up with. It's the "
		"equivalent of a bicycle for our minds. ~Steve "
		"Jobs", "3578cfbf5cff8548f88346a783"
		"863b57c91e0e58c1b09c09df7c63eef364b7dbe6911c"
		"bd29e64b8916e9e13ebbd73fece29fbd0b066ea490e6"
		"be54f8696eb66749fa961de51646ab8f908e7838df92"
		"fe8a160bfeeb10e15979a10589184b252f1698ce429a"
		"d2db31b1ed8e5919dfe7df4b8dce450b52332b46b421"
		"642306949aafa250dc99ea9e8f5aac98bbee22e2e9eb"
		"32c4c339a9d81fda924c57290b4a3c9ed0b6d1417bc8"
		"1e45f6af1905793ac13aabe05d7641c6f03dd0c11dcf"
		"633fc57a3b2cf8082fb35140a828d3b29e08e803dd41"
		"900fe33350910cf515054da6ec6674f7e46e0fa3cccd"
		"e52e0f4b389073c982df8cb2d2c2eaee608fc974bf68"
		"977f025e69d22aa2ec8c93da91cfd5332762114f387f"
		"7a3dde160b78a3ebebceba8277bab173258c04c57b8f"
		"9e5c6fcc6f886f4817889497be48b4048e496790d155"
		"0dd3ab5f6aba8a1f571e52f3dea266305d2f328837d3"
		"b38065d80de55d2b00074523456a119b365a1634c6f6"
		"8b46491b349a16650a82c2b9548c4c3f63d78dd88fed"
		"160a460148a0ff9513151b3c41801c3b4e7af6127b8a"
		"fd20a33d2aa353962bf6b220370b96cff34174d72793"
		"2fb69e839c0f6a99ab55110ed98f5ef12b110a5659de"
		"61834d50900147ab24c03c0aa1120ec3f5786f8e8e29"
		"81ce0140fb8d76dbf868f55a5b048ea7c6c20773aaca"
		"b5b427689603dbbd4f5698752750c77bf4a7e201c312"
		"7078268dac74c4c779875e7787"},
};

int main()
{
	// Use test cases first
	for (int i=0; i<sizeof(cases)/sizeof(Case); ++i) {
		schedule(cases[i].key, strlen(cases[i].key));
		if (cases[i].func == 'E') {
			// Encrypt
			for (int j=0; cases[i].input[j]; ++j)
				printf("%02x", cases[i].input[j] ^ rc4());
		} else {
			// Decrypt
			for (int j=0; cases[i].input[j]; j+=2) {
				unsigned ihex;
				sscanf(&cases[i].input[j], "%2x", &ihex);
				printf("%c", ihex ^ rc4());
			}
		}
		printf("\n");
	}
	printf("\n");
	while (true) {
		char act;
		do {
			printf("Encryption / Decryption (E/D)? ");
			discard_line();
			cin.getline(buffer, BUFL);
			act = buffer[0];
			// Loop if input in wrong
		} while (act != 'E' && act != 'D' && !feof(stdin));

		printf("Secret Key: ");
		discard_line();
		// Assume key is not too long.
		cin.getline(buffer, BUFL);

		schedule(buffer, strlen(buffer));

		int key;
		if (act == 'E') {
			printf("Plaintext: ");
			bool first_line = true;
			// Use getchar to handle large input
			while ((key = getchar()) != EOF) {
				if (line_end(key)) break;
				if (first_line) {
					printf("\n= = = = = = =\nChiphertext: ");
					first_line = false;
				}
				printf("%02x", key ^ rc4());
			}
		} else {
			printf("Chiphertext: ");
			bool first_line = true;
			unsigned ihex;
			// Use scanf to handle large input
			while (scanf("%2x", &ihex) == 1) {
				if (first_line) {
					printf("\n= = = = = = =\nPlaintext: ");
					first_line = false;
				}
				printf("%c", ihex ^ rc4());

				// check for line-end
				int t = getchar();
				if (line_end(t)) break;
				else ungetc(t, stdin);
			}
		}
		printf("\n= = = = = = =\n\n");
	}
}

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <string>
#include <cassert>

using namespace std;

namespace HOMEWORK {
	struct tKeyValuePack
	{
		char key[128];
		char value[128];
	};
	bool Insert(const char* key, const char* value);
	bool Find(const char* key, char* value);
	bool Erase(const char* key);
	int Enumerate(tKeyValuePack* key_value_in_order, int N_Max, const char* szLowKey =0, const char* szHighKey =0);
	int GetSize();
	void Clear();
}
namespace CHECK {
	using HOMEWORK::tKeyValuePack;
	bool Insert(const char* key, const char* value);
	bool Find(const char* key, char* value);
	bool Erase(const char* key);
	int Enumerate(tKeyValuePack* key_value_in_order, int N_Max, const char* szLowKey =0, const char* szHighKey =0);
	int GetSize();
	void Clear();
}

using HOMEWORK::tKeyValuePack;
const int BUF_SZ = 50000;
tKeyValuePack buff_h[BUF_SZ];
tKeyValuePack buff_c[BUF_SZ];
const char *BSTR[] = { "false", "true" };

// ----- Checks ----- //
void check_size()
{
	int csz = CHECK::GetSize();
	int hsz = HOMEWORK::GetSize();
	if (csz != hsz) {
		printf("---Check size failed---\ncorrect: %d, yours: %d\n", csz, hsz);
	}
}
void check_insert(const char *key, const char *value)
{
	int bh = HOMEWORK::Insert(key, value) ? 1 : 0;
	int bc = CHECK::Insert(key, value) ? 1 : 0;
	if (bh != bc)
		printf("---Check Insert failed---\n"
				"key: %s\nvalue: %s\n"
				"correct: %s, yours: %s\n",
				key, value, BSTR[bc], BSTR[bh]);
}
void check_erase(const char *key)
{

	int bh = HOMEWORK::Erase(key);
	int bc = CHECK::Erase(key);
	if (bh != bc)
		printf("---Check Erase failed---\n"
				"key: %s\n"
				"correct: %s, yours: %s\n",
				key, BSTR[bc], BSTR[bh]);
}
void check_enum(const char *from, const char *to)
{
	int csz = CHECK::Enumerate(buff_c, BUF_SZ, from, to);
	int hsz = HOMEWORK::Enumerate(buff_h, BUF_SZ, from, to);

	if (from == 0) from = "first";
	if (to == 0) to = "last";

	if (csz != hsz) {
		printf("---Check Enum failed---\n"
				"from: %s\nto: %s\n"
				"correct size: %d, yours: %d\n", from, to, csz, hsz);
	}

	if (hsz < csz) csz = hsz;
	for (int i=0; i<csz; ++i) {
		if (strcmp(buff_h[i].key, buff_c[i].key) != 0)
			printf("---Check Enum key failed at #%d item---\ncorrect: %s\nyours: %s\n",
					i, buff_c[i].key, buff_h[i].key);
		if (strcmp(buff_h[i].value, buff_c[i].value) != 0)
			printf("---Check Enum value failed at #%d item---\n"
					"correct key: %s\n"
					"correct: %s\nyours: %s\n",
					i, buff_c[i].key, buff_c[i].value, buff_h[i].value);
	}
}
void check_find(const char *key)
{
	tKeyValuePack kv;
	int bh = HOMEWORK::Find(key, kv.key);
	int bc = CHECK::Find(key, kv.value);
	if (bh != bc)
		printf("---Check Find failed---\n"
				"key: %s\n"
				"correct: %s, yours: %s\n",
				key, BSTR[bc], BSTR[bh]);
	if (bh && bc && strcmp(kv.key, kv.value) != 0)
		printf("---Check Find failed---\n"
				"key: %s\n"
				"correct value: %s\nyours: %s\n",
				key, kv.value, kv.key);
}
void check_all()
{
	printf("---Check all---\n");
	check_enum(0, 0);
	check_size();
}
int main()
{
	tKeyValuePack kv;
	char ins[100];
	printf("---Check started---\n");
	unsigned int counter = 0;
	while (scanf("%s", ins) == 1) {
		++counter;
		if (strcmp(ins, "INSERT") == 0) {
			scanf(" key:%s value:%s", kv.key, kv.value);
			check_insert(kv.key, kv.value);
		} else if (strcmp(ins, "ERASE") == 0) {
			scanf(" key:%s", kv.key);
			check_erase(kv.key);
		} else if (strcmp(ins, "ENUM") == 0) {
			scanf(" key:%s key:%s", kv.key, kv.value);
			check_enum(kv.key, kv.value);
		} else if (strcmp(ins, "FIND") == 0) {
			scanf(" key:%s", kv.key);
			check_find(kv.key);

		}

		if (counter % 10000 == 0)
			check_all();
	}


	check_all();

	printf("---Check clear---\n");
	CHECK::Clear();
	HOMEWORK::Clear();
	int sz = HOMEWORK::GetSize();
	if (sz) printf("Check clear failed with size %d\n", sz);
	printf("---Check #2 clear---\n");
	HOMEWORK::Clear();
	sz = HOMEWORK::GetSize();
	if (sz) printf("Check #2 clear failed with size %d\n", sz);
	return 0;
}

namespace CHECK {
	typedef map<string,string> tStorage;
	map<string,string> storage;
	bool Insert(const char* key, const char* value)
	{
		pair<tStorage::iterator,bool> retV;
		retV = storage.insert(tStorage::value_type(key, value));
		return retV.second;
	}
	bool Find(const char* key, char* value)
	{
		tStorage::iterator it;
		it = storage.find( key);
		if ( it == storage.end())
			return false;
		strcpy(value,  it->second.c_str());
		return true;
	}
	bool Erase(const char* key)
	{
		return storage.erase(key)==1;
	}
	int GetSize()
	{
		return storage.size();
	}
	void Clear()
	{
		storage.clear();
	}
	int Enumerate(tKeyValuePack* key_value_in_order, int N_Max, const char* szLowKey, const char* szHighKey )
	{
		tStorage::iterator itBegin, itEnd;
		tStorage::iterator it;
		int k;
		if ( szLowKey) {
			itBegin = storage.find(szLowKey);
			assert(itBegin != storage.end());
		}
		else
			itBegin = storage.begin();
		if (szHighKey) {
			itEnd = storage.find(szHighKey);
			assert(itEnd != storage.end());
			++itEnd;
		}
		else {
			itEnd = storage.end();
		}
		for ( k=0, it = itBegin; it != itEnd && k <N_Max; ++it) {
			strcpy(key_value_in_order[k].key, it->first.c_str());
			strcpy(key_value_in_order[k].value, it->second.c_str());
			k++;
		}
		return k;
	}
}

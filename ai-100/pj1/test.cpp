#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

using namespace std;

// ---------- header ---------- //

void init();
bool check_input(const char*);

int heuristic_test(const int[]);
int heuristic_misplace(const int[]);
int heuristic_manhattan(const int[]);
int heuristic_database(const int[]);

void proj1(const char *, int, int, std::vector<int> *);
vector<int> A_star_search(const char *, int (*)(const int[]));
vector<int> IDS(const char *);

// ---------- help functions ---------- //

int compare (const void *a, const void *b)
{
      return (*(char*)a - *(char*)b);
}

// ---------- main ---------- //

int main()
{
    vector<int> sol;
    bool debug = true;

    printf("-- Initializing --\n");

    printf("-- Testing heuristic functions --\n");

#if FORCE_CHECK
    // ---------- check passed at 3/31 9:00 AM ---------- //
    printf("-- Testing invalid inputs --\n");
    for (int i=0; i<1000000000; ++i) {
        // print progress
        if (i%100000000 == 0) {
            printf("check #%d\n", i/100000000);
        }
        // generate input
        char str[] = "0000000000", str2[] = "0000000000";
        for (int j=0, k=i; k; ++j) {
            str2[j] = (str[j] += k%10);
            k /= 10;
        }
        // brute force check
        const char *str_ckt[] = {
            "000000001",
            "000000012",
            "000000123",
            "000001234",
            "000012345",
            "000123456",
            "001234567",
            "012345678",
        };
        bool ck = check_input(str), ck_ans;
        qsort(str, 10, sizeof(char), compare);
        ck_ans = !(strcmp(str, "012345678") != 0 &&
                strcmp(str, "001234567") != 0 &&
                strcmp(str, "000123456") != 0 &&
                strcmp(str, "000012345") != 0 &&
                strcmp(str, "000001234") != 0 &&
                strcmp(str, "000000123") != 0 &&
                strcmp(str, "000000012") != 0 &&
                strcmp(str, "000000001") != 0);
        if (ck_ans != ck) {
                printf("check #%d failed: %s\n", i, str2);
        }
    }
#endif


    printf("Easy input...\n");
    printf("---\n125\n034\n678\n---\n");
    proj1("125034678", 0, 0, &sol);
    for (vector<int>::iterator it = sol.begin();
            it != sol.end(); ++it) {
        int step = *it;
        char di;
        switch (step % 10) {
            case 1:
                di = '<';
                break;
            case 2:
                di = 'v';
                break;
            case 3:
                di = '>';
                break;
            case 4:
                di = '^';
                break;
        }
        printf("step: %d -> %c\n", step / 10, di);
    }

    printf("Invalid input...\n");

    proj1("021345678", 0, 0, &sol);
    if (sol.empty())
        printf("check passed\n");
    else
        printf("check failed 021345678\n");

    proj1("011145678", 0, 0, &sol);
    if (sol.empty())
        printf("check passed\n");
    else
        printf("check failed 011145678\n");

    return 0;
}

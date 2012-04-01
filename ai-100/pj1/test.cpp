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
vector<int> A_star_search(const char *, int (*)(const int[]), bool);
vector<int> IDS(const char *);

struct Info {
    unsigned node_expanded;
    unsigned space_complexity;
};

extern Info info;

// ---------- constants ---------- //
const int MAX_CONFS = 1000000000;
const int MAX_PUZZLES = 8;
const char *PUZZLES[MAX_PUZZLES] = {
    "000000001",
    "000000012",
    "000000123",
    "000001234",
    "000012345",
    "000123456",
    "001234567",
    "012345678",
};

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
    init();

    printf("-- Checking heuristic functions --\n");


#if FORCE_CHECK
    // ---------- check passed at 4/1 2:30 PM ---------- //
    printf("-- Checking invalid inputs --\n");
    for (int i=0; i<MAX_CONFS; ++i) {
        // print progress
        if (i%(MAX_CONFS/10) == 0) {
            printf("check #%d\n", i/(MAX_CONFS/10));
        }
        // generate input
        char str[] = "000000000", str2[] = "000000000";
        for (int j=0, k=i; k; ++j) {
            str2[j] = (str[j] += k%10);
            k /= 10;
        }

        // skip unreachable states
        if (!ck_ans && strcmp(str, PUZZLES[MAX_PUZZLES-1]) == 0) {
            continue;
        }
        // brute force check
        bool ck = check_input(str), ck_ans = false;
        qsort(str, 9, sizeof(char), compare);
        for (int j=0; j<MAX_PUZZLES-1; ++j) {
            if (strcmp(str, PUZZLES[j]) == 0) {
                ck_ans = true;
                break;
            }
        }

        if (ck_ans != ck) {
                printf("check #%d failed: %s\n", i, str2);
        }
    }
#endif

#if 0
    printf("-- Checking correctness of algorithms --\n");
    for (int i=0; i<MAX_CONFS; ++i) {
        // print progress
        if (i%(MAX_CONFS/10) == 0) {
            printf("check #%d\n", i/(MAX_CONFS/10));
        }
        // generate input
        char str[] = "0000000000", str2[] = "0000000000";
        for (int j=0, k=i; k; ++j) {
            str2[j] = (str[j] += k%10);
            k /= 10;
        }
        // brute force check
        /*
        bool ck = check_input(str), ck_ans = false;
        qsort(str, 10, sizeof(char), compare);
        for (int j=0; j<MAX_PUZZLES; ++j) {
            if (strcmp(str, PUZZLES[j]) == 0) {
                ck_ans = true;
                break;
            }
        }
        if (ck_ans != ck) {
                printf("check #%d failed: %s\n", i, str2);
        }
        */
    }
#endif

    printf("-- Checking unreachable states --\n");

    printf("-- Generating statistics --\n");





    // test
    printf("Easy input...\n");
    printf("---\n125\n034\n678\n---\n");
    for (int i=1; i<=3; ++i)
        for (int j=1; j<=2; ++j) {
            proj1("125304678", i, j, &sol);
            printf("------------------\n"
                    "Path length: %zu\n"
                    "-----------------\n", sol.size());
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
        }

    return 0;
}

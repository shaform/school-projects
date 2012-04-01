#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <algorithm>

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

const int MAX_PUZZLES = 8;
extern const char *PUZZLES[MAX_PUZZLES];

const int MAX_DATABASE = 6;

// ---------- constants ---------- //

const int MAX_CONFS = 1000000000;

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

#if FORCE_CHECK
    // ---------- check passed at 4/1 5:10 PM ---------- //
    printf("-- Checking heuristic functions --\n");
    for (int i=0; i<MAX_PUZZLES; ++i) {
        printf("check #%d\n", i+1);
        char str[10];
        memcpy(str, PUZZLES[i], sizeof(str));
        do {
            // generate integer representation
            int st[9];
            for (int j=0; j<9; ++j) {
                st[j] = str[j] - '0';
            }

            int act;

            // check h1
            int mis = 0;
            for (int j=0; j<9; ++j) {
                if (str[j] != '0' && str[j] != PUZZLES[i][j]) {
                    ++mis;
                }
            }

            act = heuristic_misplace(st);
            if (mis != act) {
                printf("check failed: h1() for %s, expect: %d, actual: %d\n", str, mis, act);
            }

            // check h2
            int d = 0;
            for (int j='1'; j<='9'; ++j) {
                int x = 9, y = 9;
                for (int k=0; k<9; ++k) {
                    if (PUZZLES[i][k] == j) {
                        x = k;
                    }
                    if (str[k] == j) {
                        y = k;
                    }
                }
                if (x == 9) {
                    break;
                } else {
                    d += abs((x%3)-(y%3)) + abs((x/3)-(y/3));
                }
            }

            act = heuristic_manhattan(st);
            if (d != act) {
                printf("check failed: h2() for %s, expect: %d, actual: %d\n", str, d, act);
            }

            // check h3
            int dis;
            act = heuristic_database(st);
            if (i < MAX_DATABASE) {
                dis =  A_star_search(str, heuristic_manhattan, false).size();
            } else {
                dis = act;
            }
            if (dis != act) {
                printf("check failed: h3() for %s, expect: %d, actual: %d\n", str, dis, act);
            }
        } while (next_permutation(str, str+9));
    }
#endif

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

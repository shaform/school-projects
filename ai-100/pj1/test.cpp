#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
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

int compare(const void *a, const void *b)
{
      return (*(char*)a - *(char*)b);
}

void shuffle(char str[], int sz)
{
    for (int i=sz-1; i>0; --i) {
        int j = rand() % (i+1);
        swap(str[i], str[j]);
    }
}

bool check_solution(const char source[], vector<int> steps, const char dest[])
{
    char str[10];
    memcpy(str, source, sizeof(str));
    for (vector<int>::iterator it=steps.begin();
            it != steps.end(); ++it) {
        int u = *it / 10;
        int di = *it % 10;
        // L-D-R-U 1-2-3-4
        // assuming no errors, or memory error would happen
        for (int i=0; i<9; ++i) {
            if (str[i] == u+'0') {
                if (di == 1) {
                    swap(str[i], str[i-1]);
                } else if (di == 2) {
                    swap(str[i], str[i+3]);
                } else if (di == 3) {
                    swap(str[i], str[i+1]);
                } else {
                    swap(str[i], str[i-3]);
                }
                break;
            }
        }
    }
    return strcmp(str, dest) == 0;
}

// ---------- main ---------- //

int main()
{
    vector<int> sol;
    bool debug = true;

    printf("-- Initializing --\n");
    init();

#ifdef FORCE_CHECK
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

#ifdef FORCE_CHECK
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

        bool ck = check_input(str), ck_ans = false;
        qsort(str, 9, sizeof(char), compare);

        // skip unreachable states
        if (strcmp(str, PUZZLES[MAX_PUZZLES-1]) == 0) {
            continue;
        }

        // brute force check
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

#ifdef FORCE_CHECK
    // ---------- check passed at 4/1 7:33 PM ---------- //
    printf("-- Checking unreachable states --\n");
    {
        char str[10];
        memcpy(str, PUZZLES[7], sizeof(str));

        // brute force check
        int count = 0;
        int reachable = 0, unreachable = 0;
        while (next_permutation(str, str+9)) {
            // print progress
            if (++count % 1000 == 0) {
                printf("check #%d / %d\n", count / 1000, 362880 / 1000);
            }
            if (check_input(str)) {
                ++reachable;
                if (A_star_search(str, heuristic_manhattan, false).size() == 0) {
                    printf("check failed: %s, expect: unreachable, actual: reachable\n", str);
                }
            } else {
                ++unreachable;
                // it exhausts all memory, cannot actually test
                /*
                if (A_star_search(str, heuristic_database, false).size() != 0) {
                    printf("check failed: %s, expect: reachable, actual: unreachable\n", str);
                }
                */
            }
        }
        if (reachable+1 != unreachable) {
            printf("check failed, number should equal: unreachable: %d, reachable: %d\n",
                    unreachable, reachable+1);
        }
    }
#endif

#if 1
    printf("-- Checking correctness of algorithms --\n");
    {
        const int RANDOM_LIMIT = 4;
        const int CHECK_NUM = 500;
        for (int i=0; i<RANDOM_LIMIT; ++i) {
            // print progress
            printf("check #%d\n", i+1);
            char str[10];
            memcpy(str, PUZZLES[i], sizeof(str));
            do {
                int curr;
                vector<int> vec;
                proj1(str, 1, 0, &vec);
                if (vec.size() == 0) {
                    if ((check_input(str) && strcmp(str, PUZZLES[i]) != 0)) {
                        printf("check failed at IDS: %s\n", str);
                    }
                } else if (!check_solution(str, vec, PUZZLES[i])) {
                    printf("check failed at IDS: %s\n", str);
                }
                curr = vec.size();
                for (int j=1; j<=3; ++j) {
                    for (int k=2; k<=3; ++k) {
                        proj1(str, k, j, &vec);
                        if (!check_solution(str, vec, PUZZLES[i]) ||
                                vec.size() != curr) {
                            printf("check failed at A*(%d), h%d(): %s\n", k, j, str);
                        }
                    }
                }
            } while (next_permutation(str, str+9));
        }

        // initialize random seed
        srand(time(NULL));
        for (int i=RANDOM_LIMIT; i<MAX_CONFS; ++i) {
            char str[10];
            memcpy(str, PUZZLES[i], sizeof(str));
            int count = 0;
            while (++count <= CHECK_NUM) {
                // print progress
                if (count % (CHECK_NUM / 10) == 0) {
                    printf("check #%d-%d\n", i+1, count / (CHECK_NUM / 10));
                }
                int curr;
                vector<int> vec;
                proj1(str, 1, 0, &vec);
                if (vec.size() == 0) {
                    if ((check_input(str) && strcmp(str, PUZZLES[i]) != 0)) {
                        printf("check failed at IDS: %s\n", str);
                    }
                } else if (!check_solution(str, vec, PUZZLES[i])) {
                    printf("check failed at IDS: %s\n", str);
                }
                curr = vec.size();
                for (int j=1; j<=3; ++j) {
                    for (int k=2; k<=3; ++k) {
                        proj1(str, k, j, &vec);
                        if (!check_solution(str, vec, PUZZLES[i]) ||
                                vec.size() != curr) {
                            printf("check failed at A*(%d), h%d(): %s\n", k, j, str);
                        }
                    }
                }
                shuffle(str, 9);
            }
        }
    }
#endif
    printf("-- Generating statistics --\n");
    return 0;
}

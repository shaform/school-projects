// --------------------------------------------------------- //
// ** 1~8 puzzles testing and experimentation program **
//
// !!
//    Notice this program takes a long time to run and
//    uses a lot of memory. Some machines may not be
//    able to run it.
// !!
//
// This program contains two parts:
//
// 1) Testing routines used to test the correctness of
//    the solving program.
//
//    These tests are commented out since they have been
//    performed during development.
//
//    To force the checks, compile the program with
//    `FORCE_CHECK' defined.
//
//    Notice some checks may take extremely long time to run,
//    since all checks are done by brute force.
//
// 2) Statistics generating routines.
//    
//    Files contianing raw data would be generated under
//    the directory.
//
//    sol1.txt ~ sol8.txt contains the optimal depth of
//    configurations in n-puzzle game.
//
//    algi_hj_k.txt contains the time / space complexity
//    of configurations for algorithm i with heuristic j
//    in k-puzzle game. (j has no meaning for IDS)
//
//    Notice this still takes a long time to run, and
//    A* tree search uses a lot of memory.
//
//    Some machines may not be able to run A* tree search.
//
// --------------------------------------------------------- //

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <vector>
#include <algorithm>

using namespace std;

// ---------- header ---------- //
// Because no header file is allowed, we put the header here
// If you change anything here, you should change
// the header section in another file also.

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
    unsigned long long node_expanded;
    unsigned long long space_complexity;
};

extern Info info;

const int MAX_PUZZLES = 8;
extern const char *PUZZLES[MAX_PUZZLES];

const int MAX_DATABASE = 6;

const int MAX_MOVES = 24;
extern const int NEXT_MOVE[MAX_MOVES][2];

// ---------- constants ---------- //

const int MAX_CONFS = 1000000000;

// ---------- help functions ---------- //

int compare(const void *a, const void *b)
{
    return (*(char*)a - *(char*)b);
}

// randomly shuffle the string
void shuffle(char str[], int sz)
{
    for (int i=sz-1; i>0; --i) {
        int j = rand() % (i+1);
        swap(str[i], str[j]);
    }
}

// check if the solution is correct
bool check_solution(const char source[], vector<int> steps, const char dest[])
{
    char str[10];
    memcpy(str, source, sizeof(str));
    for (vector<int>::iterator it=steps.begin();
            it != steps.end(); ++it) {
        int u = *it / 10;
        int di = *it % 10;
        // L-D-R-U 1-2-3-4
        for (int i=0; i<9; ++i) {
            if (str[i] == u+'0') {
                if (di == 1) {
                    if (i % 3 == 0) return false;  // check if possible to move
                    swap(str[i], str[i-1]);
                } else if (di == 2) {
                    if (i >= 6) return false;  // check if possible to move
                    swap(str[i], str[i+3]);
                } else if (di == 3) {
                    if (i % 3 == 2) return false;  // check if possible to move
                    swap(str[i], str[i+1]);
                } else {
                    if (i <= 2) return false;  // check if possible to move
                    swap(str[i], str[i-3]);
                }
                break;
            }
        }
    }
    return strcmp(str, dest) == 0;
}

// generat raw statistics
// if sol_stat is true, generate optimal search depth of puzzles
double generate_output(int alg, int h, int sz, bool sol_stat, bool rd)
{
    unsigned opt_depth = 0, opt_num = 0;
    // unsigned max_depth = 0;
    char fname[50];
    if (sol_stat) {
        sprintf(fname, "sol%d.txt", sz);
    } else {
        sprintf(fname, "alg%d_h%d_%d.txt", alg, h, sz);
    }
    FILE *out = fopen(fname, "w+");
    if (sol_stat) {
        fprintf(out, "--- statistics for solution of size: %d ---\n", sz);
    } else {
        fprintf(out, "--- statistics for alg: %d h: %d size: %d ---\n"
                "/--node expanded--/--space complexity--/\n", alg, h, sz);
    }

    vector<int> vec;
    char str[10];
    int count = 100;
    memcpy(str, PUZZLES[sz-1], sizeof(str));
    if (rd) {
        shuffle(str, 9);
    }
    while (true) {
        if (check_input(str)) {
            proj1(str, alg, h, &vec);
            if (sol_stat) {
                fprintf(out, "%u\n", (unsigned) vec.size());
                opt_depth += vec.size();
                /*
                if (vec.size() > max_depth) {
                    printf("%lu depth: %s\n", vec.size(), str);
                    max_depth = vec.size();
                }
                */
                ++opt_num;
            } else {
                fprintf(out, "%llu      %llu\n", info.node_expanded, info.space_complexity);
            }
        }
        if (rd) {
            if (--count < 0) {
                break;
            } else {
                shuffle(str, 9);
            }
        } else {
            if (!next_permutation(str, str+9)) {
                break;
            }
        }
    }

    fclose(out);

    if (opt_num) {
        return ((double) opt_depth)/opt_num;  // return average of depth
    } else {
        return 0;
    }
}

// ---------- main ---------- //

int main()
{
    printf("-- Initializing --\n");
    // If we omit this, it still gets called when
    // proj1 is called for the first time.
    init();

    // ------------------------------ //
    // Part I: Correctness Checking   //
    // ------------------------------ //

#ifdef FORCE_CHECK
    // ---------- check passed at 4/2 1:40 PM ---------- //
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

#if FORCE_DEBUG
    // ---------- check passed at 4/4 10:34 PM ---------- //
    // notice: this takes hours to run!!
    printf("-- Checking correctness of algorithms --\n");
    {
        // It seems impossible to check all algorithms
        // so we only check some algorithms
        const int RANDOM_LIMIT = 2;
        const int CHECK_NUM = 50;
        for (int i=0; i<MAX_PUZZLES; ++i) {
            // print progress
            printf("check #%d\n", i+1);
            char str[10];
            memcpy(str, PUZZLES[i], sizeof(str));
            do {
                vector<int> vec;
                proj1(str, 2, 2, &vec);
                if (vec.size() == 0) {
                    if ((check_input(str) && strcmp(str, PUZZLES[i]) != 0)) {
                        printf("check failed at A* h2: %s\n", str);
                    }
                } else if (!check_solution(str, vec, PUZZLES[i])) {
                    printf("check failed at A* h2: %s\n", str);
                }
                int curr = vec.size();
                proj1(str, 2, 3, &vec);
                if (!(check_solution(str, vec, PUZZLES[i]) || curr == 0) ||
                        vec.size() != curr) {
                    printf("check failed at A* h3: %s\n", str);
                }

                if (i < 2 || vec.size() < 3) {
                    proj1(str, 1, 1, &vec);
                    if (!(check_solution(str, vec, PUZZLES[i]) || curr == 0) ||
                            vec.size() != curr) {
                        printf("check failed at IDS: %s\n", str);
                    }

                    proj1(str, 3, 1, &vec);
                    if (!(check_solution(str, vec, PUZZLES[i]) || curr == 0) ||
                            vec.size() != curr) {
                        printf("check failed at A* tree h1: %s\n", str);
                    }
                }

                if (i < 4 || vec.size() < 4) {
                    proj1(str, 3, 2, &vec);
                    if (!(check_solution(str, vec, PUZZLES[i]) || curr == 0) ||
                            vec.size() != curr) {
                        printf("check failed at A* tree h2: %s\n", str);
                    }
                }

                if (i < 5 || vec.size() < 4) {
                    proj1(str, 3, 3, &vec);
                    if (!(check_solution(str, vec, PUZZLES[i]) || curr == 0) ||
                            vec.size() != curr) {
                        printf("check failed at A* tree h3: %s\n", str);
                    }
                }
            } while (next_permutation(str, str+9));
        }
    }
#endif

    // ------------------------------ //
    // Part II: Generating Statistics //
    // ------------------------------ //

    printf("-- Generating branching factors --\n");
    printf("do you want to generate? (y/n)\n");
    if (getchar() == 'y') {
        for (int i=1; i<=8; ++i) {
            char str[10];
            unsigned count = 0, num = 0, maxb = 0;
            printf("-- Generating for size %d --\n", i);
            memcpy(str, PUZZLES[i-1], sizeof(str));
            do {
                if (check_input(str)) {
                    unsigned bf = 0;
                    for (int j=0; j<MAX_MOVES; ++j) {
                        int u = NEXT_MOVE[j][0], v = NEXT_MOVE[j][1];
                        if (str[u] == '0' && str[v] != '0') {
                            bf++;
                        }
                    }
                    maxb = max(maxb, bf);
                    count += bf;
                    num++;
                }
            } while (next_permutation(str, str+9));
            printf("average: %f, maximum: %u\n", ((double) count)/num, maxb);
        }
    }
    getchar();  // discard '\n'
    printf("-- Generating solution statistics --\n");
    printf("***************************************************\n"
            "[!!Warning!!] this may take hours!!\n"
            "***************************************************\n\n");
    printf("do you want to generate? (y/n)\n");
    if (getchar() == 'y') {
        for (int i=1; i<=8; ++i) {
            printf("-- Generating for size %d --\n", i);
            double avg = generate_output(2, 2, i, true, false);
            printf("average optimal depth: %f\n", avg);
        }
    }
    getchar();  // discard '\n'
    printf("-- Generating statistics --\n");
    printf("***************************************************\n"
            "[!!Warning!!] this may take hours or even days!!\n"
            "[!!Warning!!] A* tree may exhaust all your memory!\n"
            "[!!Warning!!] Check your memory limit first!!\n"
            "[!!Warning!!] By default, 0 doesn't include A* tree\n"
            "[!!Warning!!] IDS take so much time with more than\n"
            "              3 tiles, so by default it halts at\n"
            "              2-puzzle.\n"
            "***************************************************\n\n");
    printf("enter 4 integers to choose what to generate:\n"
            "(1) algorithm: 0 - all, 1 - IDS, 2 - A* graph, 3 - A* tree\n"
            "(2) heuristic: 0 - all, 1 - misplace, 2 - manhattan, 3 - database\n"
            "(3) size: 0 - all, 1~8\n"
            "(4) random?: 1 - yes, 0 no\n");
    {
        int alg = 2, h = 2, sz = 1, rd = 1;
        scanf("%d %d %d %d", &alg, &h, &sz, &rd);
        if (sz < 0 || sz > 8) {
            sz = 1;
        }
        for (int i=1; i<=2; ++i) {
            if (alg) i = alg;
            for (int j=1; j<=3; ++j) {
                if (h) j = h;
                for (int k=1; k<=8; ++k) {
                    if (sz) k = sz;
                    printf("-- Generating for %d/%d/%d %s--\n",
                            i, j, k, rd ? "random" : "all perms");
                    generate_output(i, j, k, false, rd);
                    if (sz) break;
                    if (i==1 && k >= 2) break;  // break for IDS
                }
                if (i==1) break;  // no heuristic for IDS
                if (h) break;
            }
            if (alg) break;
        }
    }


    return 0;
}

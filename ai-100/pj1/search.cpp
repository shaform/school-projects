// --------------------------------------------------------- //
// ** 1~8 puzzles solving program **
// --------------------------------------------------------- //

#include <cstdio>
#include <cstring>
#include <cmath>
#include <queue>
#include <stack>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <functional>

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
int heuristic_max(const int[]);

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

// the positions to be exchanged
const int NEXT_MOVE[MAX_MOVES][2] = {
    {0, 1}, {1, 2},
    {3, 4}, {4, 5},
    {6, 7}, {7, 8},
    {0, 3}, {3, 6},
    {1, 4}, {4, 7},
    {2, 5}, {5, 8},

    {1, 0}, {2, 1},
    {4, 3}, {5, 4},
    {7, 6}, {8, 7},
    {3, 0}, {6, 3},
    {4, 1}, {7, 4},
    {5, 2}, {8, 5},
};
// the directions of each exchange:
// L-D-R-U 1-2-3-4
const int NEXT_DI[MAX_MOVES] = {
    3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1,
    4, 4, 4, 4, 4, 4,
};

// ---------- declarations ---------- //

struct Move {
    int prev;  // hash of previous state
    int curr;  // hash of current state
    int move;  // move to get to current state
    int g;

    Move(int n) : prev(0), curr(n), move(0), g(0) {}
    Move() : prev(0), curr(0), move(0), g(0) {}

    bool operator<(const Move &rhs) const
    {
        return curr < rhs.curr;
    }
};

struct Node {
    int f;
    int board[9];  // puzzle configuration
    Move move;

    Node() : f(0) {}
    Node(const char *str) : f(0)
    {
        for (int i=0; i<9; ++i) {
            board[i] = str[i]-'0';
        }
        move.curr = hash();
    }
    Node(const int *st) : f(0)
    {
        for (int i=0; i<9; ++i) {
            board[i] = st[i];
        }
        move.curr = hash();
    }

    bool operator<(const Node &rhs) const
    {
        return f < rhs.f;
    }
    bool operator>(const Node &rhs) const
    {
        return f > rhs.f;
    }
    int hash() const
    {
        int h= 0, mul = 1;
        for (int i=0; i<9; ++i) {
            h+= board[i]*mul;
            mul *= 10;
        }
        return h;
    }
    void to_string(char *str) const
    {
        for (int i=0; i<9; ++i)
            str[i] = board[i] + '0';
        str[9] = '\0';
    }
    bool goal_test() const
    {
        int nx = 0;
        for (int i=0; i<9; ++i) {
            if (nx) {
                if (board[i] != nx) {
                    return false;
                } else {
                    ++nx;
                }
            } else if (board[i] == 1) {
                nx = 2;
            } else if (board[i] != 0) {
                return false;
            }
        }
        return true;
    }
    vector<Node> moves() const
    {
        vector<Node> succs;
        Node nd;
        for (int i=0; i<MAX_MOVES; ++i) {
            int u = NEXT_MOVE[i][0], v = NEXT_MOVE[i][1];
            if (board[u] != 0 && board[v] == 0) {
                nd = *this;
                swap(nd.board[u], nd.board[v]);

                nd.move.prev = move.curr;
                nd.move.curr = nd.hash();
                nd.move.move = board[u]*10 + NEXT_DI[i];

                succs.push_back(nd);
            }
        }
        return succs;
    }
};

struct Frontier {
    vector<Node> heap;
    set<int> inserted;

    void push(const Node &u)
    {
        inserted.insert(u.hash());
        heap.push_back(u);
        decrease_key(heap.size()-1);
    }
    void replace_or_push(const Node &u)
    {
        int h = u.hash();
        if (inserted.find(h) == inserted.end()) {
            push(u);
            return;
        }
        for (unsigned i=0; i<heap.size(); ++i) {
            if (heap[i].hash() == h) {
                if (heap[i] > u) {
                    heap[i] = u;
                    decrease_key(i);
                }
                return;
            }
        }
    }
    void decrease_key(int u)
    {
        while (u>=1 && heap[(u-1)/2] > heap[u]) {
            swap(heap[u], heap[(u-1)/2]);
            u = (u-1)/2;
        }
    }
    void heapify(int u, int size)
    {
        while (true) {
            int l = u*2+1, r = u*2+2;
            int mini = u;

            if (l < size && heap[l] < heap[mini])
                mini = l;
            if (r < size && heap[r] < heap[mini])
                mini = r;

            if (mini != u) {
                swap(heap[u], heap[mini]);
                u = mini;
            } else {
                break;
            }
        }
    }
    unsigned long long size() const
    {
        return heap.size();
    }
    Node top() const
    {
        return heap[0];
    }
    void pop()
    {
        inserted.erase(heap[0].hash());
        heap[0] = heap[heap.size()-1];
        heap.pop_back();
        heapify(0, heap.size());
    }
    bool empty() const
    {
        return heap.empty();
    }
};

// ---------- global variables ---------- //

bool initialized = false;
Info info;
map<int, int> db;

// ---------- helper functions ---------- //

void gen_db()
{
    db.clear();

    for (int i=0; i<MAX_DATABASE; ++i) {
        // generate database using BFS
        queue<Node> frontier;
        set<Move> explored;

        Node s(PUZZLES[i]);

        frontier.push(s);

        while (!frontier.empty()) {
            Node nd = frontier.front();
            frontier.pop();

            // generate successors
            if (explored.find(nd.move) == explored.end()) {
                explored.insert(nd.move);
                db[nd.move.curr] = nd.move.g;
                vector<Node> moves = nd.moves();
                for (vector<Node>::iterator it = moves.begin();
                        it != moves.end(); ++it) {
                    if (explored.find(it->move) == explored.end()) {
                        it->move.g = nd.move.g + 1;
                        frontier.push(*it);
                    }
                }
            }
        }
    }
}

void init()
{
    initialized = true;
    gen_db();
}

bool check_input(const char *str)
{
    if (strlen(str) != 9)
        return false;

    bool nums[9];
    memset(nums, 0, sizeof(nums));

    for (int i=0; i<9; ++i) {
        // check invalid character
        if (str[i] < '0' || str[i] > '8')
            return false;

        int n = str[i]-'0';
        // check duplicate character
        if (n != 0 && nums[n])
            return false;

        nums[n] = true;
    }

    // handle all zeros                                                         
                                                                                
    if (!nums[1]) {                                                             
        return false;                                                           
    }

    // check numbers are consecutive
    bool revs = false;
    for (int i=0; i<9; ++i) {
        if (revs) {
            if (nums[i])
                return false;
        } else {
            if (!nums[i])
                revs = true;
        }
    }

    // check the goal is reachable
    // only 8-puzzle
    if (nums[8]) {
        int parity = 0;
        for (int i=0; i<9; ++i)
            for (int j=i+1; j<9; ++j) {
                if (str[i] != '0' && str[j] != '0' && str[j] < str[i])
                    parity++;
            }

        if (parity % 2 == 1)
            return false;
    }


    return true;
}


// ---------- heuristic functions ---------- //


// test, always 0
int heuristic_test(const int[])
{ return 0; }


// total number of misplaced tiles
int heuristic_misplace(const int st[])
{
    // get the number of tiles
    int num = 9;
    for (int i=0; i<9; ++i) {
        if (st[i] == 0) {
            --num;
        }
    }

    // check misplacements
    int h = 0;
    for (int i=1; i<=num; ++i) {
        if (st[9-num+i-1] != i) {
            h++;
        }
    }

    return h;
}

// Manhattan distance
int heuristic_manhattan(const int st[])
{
    // get the number of tiles
    int num = 9;
    for (int i=0; i<9; ++i) {
        if (st[i] == 0) {
            --num;
        }
    }

    // check distances
    int h = 0;
    for (int i=1; i<=num; ++i) {
        int c = 9-num+i-1;
        for (int j=0; j<9; ++j) {
            if (st[j] == i) {
                int dx = abs((c%3)-(j%3));
                int dy = abs((c/3)-(j/3));
                h += (dx+dy);
            }
        }
    }

    return h;
}

// store exact distances up to MAX_DATABASE
int heuristic_database(const int st[])
{
    // get maximum num
    int maxn = 0;
    for (int i=0; i<9; ++i) {
        if (st[i] > maxn) {
            maxn = st[i];
        }
    }
    int diff = max(maxn - MAX_DATABASE, 0);
    int part[9];
    for (int i=0; i<9; ++i) {
        if (st[i] > diff) {
            part[i] = st[i]-diff;
        } else {
            part[i] = 0;
        }
    }
    Node t(part);
    map<int, int>::iterator it = db.find(t.move.curr);

#ifndef FORCE_CHECK
    // this should never happen
    if (it == db.end()) {
        char str[10];
        for (int i=0; i<9; ++i) {
            str[i] = part[i]-'0';
        }
        str[9] = '\0';
        return IDS(str).size();
    }
#endif

    return it->second;
}

int heuristic_max(const int st[])
{
    return max(heuristic_manhattan(st), heuristic_database(st));
}

// ---------- search functions ---------- //
// algo: 1 - IDS, 2 - A* graph, 3 - A* tree
// heuristic: 1 - misplacement, 2 - manhattan, 3 - pattern database
void proj1(const char *source, int algo, int heuristic, std::vector<int> *sol)
{
    if (!initialized)
        init();

    if (sol == 0) {
        return;
    }

    if (check_input(source)) {
        int (*h)(const int[]);
        if (heuristic == 1) {
            h = heuristic_misplace;
        } else if (heuristic == 2) {
            h = heuristic_manhattan;
        } else if (heuristic == 3) {
            h = heuristic_database;
        } else if (heuristic == 4) {
            h = heuristic_max;
        } else {
            h = heuristic_test;
        }

        info.node_expanded = 0;
        info.space_complexity = 0;
        if (algo == 1) {
            *sol = IDS(source);
        } else if (algo == 2) {
            *sol = A_star_search(source, h, false);
        } else if (algo == 3) {
            *sol = A_star_search(source, h, true);
        } else {
            sol-> clear();
        }
    } else {
        sol->clear();
    }
}


vector<int> IDS(const char *source)
{
    Node s(source);
    vector<int> path;

    int max_depth = 0;
    while (true) {
        stack<Node> stk;
        stk.push(s);

        while (!stk.empty()) {
            info.space_complexity = max(info.space_complexity, (unsigned long long) stk.size());
            Node nd = stk.top();
            stk.pop();
            ++info.node_expanded;

            // contruct path
            if (nd.move.g) {
                while (nd.move.g <= path.size()) {
                    path.pop_back();
                }
                path.push_back(nd.move.move);
            }

            if (nd.goal_test()) {
                return path;
            }

            if (nd.move.g >= max_depth) {
                continue;
            }
            // generate successors
            vector<Node> moves = nd.moves();
            for (vector<Node>::iterator it = moves.begin();
                    it != moves.end(); ++it) {
                it->move.g = nd.move.g + 1;
                stk.push(*it);
            }

        }

        while (!stk.empty())
            stk.pop();
        ++max_depth;

    }
}

vector<int> A_star_search(const char *source, int (*h)(const int[]), bool tree)
{
    Frontier frontier;
    set<Move> explored;

    Node s(source);

    frontier.push(s);

    while (!frontier.empty()) {
        info.space_complexity = max(info.space_complexity, frontier.size());
        // extract a node with minimal f value
        Node nd = frontier.top();
        frontier.pop();
        ++info.node_expanded;

        if (nd.goal_test()) {
            // construct the path
            vector<int> path;
            Move mv = nd.move;
            while (mv.move) {
                path.push_back(mv.move);
                mv.curr = mv.prev;
                // find the parent
                mv = *explored.find(mv);
            }

            reverse(path.begin(), path.end());
            return path;
        }

        // generate successors
        set<Move>::iterator it = explored.find(nd.move);
        if (tree || it == explored.end()) {
            if (it == explored.end()) {
                explored.insert(nd.move);
            } else {
                // A* tree search
                if (it->g > nd.move.g) {
                    explored.erase(it);
                    explored.insert(nd.move);
                }
            }
            vector<Node> moves = nd.moves();
            for (vector<Node>::iterator it = moves.begin();
                    it != moves.end(); ++it) {
                if (tree || explored.find(it->move) == explored.end()) {
                    it->move.g = nd.move.g + 1;
                    it->f = it->move.g + h(it->board);
                    if (tree) {
                        frontier.push(*it);
                    } else {
                        frontier.replace_or_push(*it);
                    }
                }
            }
        }
    }

    // no solution
    return vector<int>();
}

#include <cstdio>
#include <cstring>
#include <queue>
#include <set>
#include <vector>
#include <algorithm>

using namespace std;


vector<int> A_star_search(const char *, int (*)(const int[]));

static bool initialized = false;

// The positions to be exchanged
const int MAX_MOVES = 24;
static int NEXT_MOVE[MAX_MOVES][2] = {
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
// L-D-R-U 1-2-3-4
static int NEXT_DI[MAX_MOVES] = {
    3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1,
    4, 4, 4, 4, 4, 4,
};

static void init()
{
    initialized = true;
}

struct Move {
    int prev;
    int curr;
    int move;
    bool operator<(const Move &rhs) const
    {
        return curr < rhs.curr;
    }
    Move(int n) : curr(n) {}
    Move() {}
};

struct Node {
    int f, g;
    int board[9];
    Move move;

    bool operator<(const Node &rhs) const
    {
        return f < rhs.f;
    }

    int hash()
    {
        int h= 0, mul = 1;
        for (int i=0; i<9; ++i) {
            h+= board[i]*mul;
            mul *= 10;
        }
        return h;
    }

    bool goal_test()
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

    vector<Node> moves()
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

static bool check_input(const char *str)
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
    int parity = 0;
    // only 8-puzzle
    if (nums[8]) {
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

int test_h(const int[])
{ return 0; }
void proj1(const char *source, int algo, int heuristic, std::vector<int> *sol)
{
    if (!initialized)
        init();

    if (check_input(source)) {
        *sol = A_star_search(source, test_h);
    } else if (sol != 0) {
        sol->clear();
    }
}


vector<int> A_star_search(const char *source, int (*h)(const int[]))
{
    priority_queue<Node> frontier;
    set<Move> explored;

    Node s;
    s.f = 0;
    s.g = 0;
    for (int i=0; i<9; ++i)
        s.board[i] = source[i]-'0';
    s.move.move = 0;
    s.move.curr = s.hash();
    s.move.prev = 0;

    frontier.push(s);

    while (!frontier.empty()) {
        Node nd = frontier.top();

        if (nd.goal_test()) {
            vector<int> path;
            Move mv = nd.move;
            while (mv.move) {
                path.push_back(mv.move);
                mv.curr = mv.prev;
                mv = *explored.find(mv);
            }

            reverse(path.begin(), path.end());
            return path;
        }

        frontier.pop();
        if (explored.find(nd.move) == explored.end()) {
            explored.insert(nd.move);
            vector<Node> moves = nd.moves();
            for (vector<Node>::iterator it = moves.begin();
                    it != moves.end(); ++it) {
                if (explored.find(it->move) == explored.end()) {
                    it->g = nd.g + 1;
                    it->f = nd.g + h(it->board);
                    frontier.push(*it);
                }
            }
        }
    }
}

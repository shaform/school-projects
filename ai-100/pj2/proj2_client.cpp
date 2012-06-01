#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <climits>
#include <ctime>
#include <algorithm>

#define ENABLE_INPUT 1

#if ENABLE_DEF
#include "def.h"
#include "defb.h"
#elif ENABLE_INPUT
int DEPTH_INC;
int V_LOST, V_TWO, V_MOVE, V_MOVEO, V_REDU, V_DEPTH, V_LINE;
int V_LOSTB, V_TWOB, V_MOVEB, V_MOVEOB, V_REDUB, V_DEPTHB, V_LINEB;
#else
const int DEPTH_INC = 4;
const int V_LOST = 477885;
const int V_TWO = 41575;
const int V_MOVE = 828891;
const int V_MOVEO = 599221;
const int V_REDU = 10;
const int V_LINE = 0;
const int V_LOSTB = 622079;
const int V_TWOB =  873744;
const int V_MOVEB = 806641;
const int V_MOVEOB = 630353;
const int V_REDUB = 10;
const int V_LINEB = 0;
#endif

#define ENABLE_PRUN 1
using namespace std;

const char *D_OUT[] = {"up", "right", "down", "left", ""};
const int D_UP = 0, D_RIGHT = 1, D_DOWN = 2, D_LEFT = 3, D_NO = 4;
const int D_INF = INT_MAX, D_NEGINF = INT_MIN;
const struct { int x, y; } D_MOVES[] = {
    {-1, 0}, {0, 1}, {1, 0}, {0, -1},
};

struct Action {
    int x, y, direction;
};
struct iPair {
    int x, y, z;
};
enum ActionCode { DROP, CAPTURE, MOVE };

// -- internal states -- //

struct State {
    int chessboard[6][6];
    int remains[2];
    int lost[2];
    int depth;
} g_currentState;

int g_player = 221;
int g_enemy = 2;
int g_sndHand = 0;
bool g_captured = false;
time_t g_startTime;
bool g_cutOne = false, g_cutTwo = false;
bool g_action = true;

// -- function prototypes -- //
Action alpha_beta_search(State &, ActionCode);
static int max_value(State, int, int, bool = false);
static int min_value(State, int, int, bool = false);
bool cutoff_test(State &);
int eval(State &);
#if ENABLE_ERRMSG
void print_state(State &s)
{
    for (int i=0; i<6; ++i) {
        for (int j=0; j<6; ++j) {
            printf("%d,", s.chessboard[i][j]);
        }
        printf("\n");
    }
    printf("remains: %d %d\n", s.remains[0], s.remains[1]);
}
#endif

int calc_dist(int cb[6][6], int p = g_player)
{
    int posi[12], posj[12];
    int count = 0, ret = 0;
    for (int i=0; i<6; ++i)
        for (int j=0; j<6; ++j)
            if (cb[i][j] == p) {
                posi[count] = i;
                posj[count] = j;
                count++;
            }
    for (int i=0; i<count; ++i)
        for (int j=i+1; j<count; ++j)
            ret += abs(posi[i]-posi[j]) + abs(posj[i]-posj[j]);

    return ret;
}
int calc_moves_out(int cb[6][6], int p)
{
    int count = 0;
    // move in
    for (int i=0; i<6; ++i) {
        for (int j=0; j<6; ++j) {
            // only player can work
            if (cb[i][j] == p) {
                bool goOut = (i > 0 && cb[i-1][j] == 0)
                    || (i < 5 && cb[i+1][j] == 0);
                // from left
                if (j > 2 && cb[i][j-3] == p
                        && cb[i][j-2] == p && cb[i][j-1] == p
                        && (j == 3 || cb[i][j-4] != p)
                        && (goOut || (j < 5 && cb[i][j+1] == 0))) {
                    count++;
                }

                // from right
                if (j < 3 && cb[i][j+3] == p
                        && cb[i][j+2] == p && cb[i][j+1] == p
                        && (j == 2 || cb[i][j+4] != p)
                        && (goOut || (j > 0 && cb[i][j-1] == 0))) {
                    count++;
                }
            }

            if (cb[j][i] == p) {
                bool goOut = (i > 0 && cb[j][i-1] == 0)
                    || (i < 5 && cb[j][i+1] == 0);
                // from left
                if (j > 2 && cb[j-3][i] == p
                        && cb[j-2][i] == p && cb[j-1][i] == p
                        && (j == 3 || cb[j-4][i] != p)
                        && (goOut || (j < 5 && cb[j+1][i] == 0))) {
                    count++;
                }

                // from right
                if (j < 3 && cb[j+3][i] == p
                        && cb[j+2][i] == p && cb[j+1][i] == p
                        && (j == 2 || cb[j+4][i] != p)
                        && (goOut || (j > 0 && cb[j-1][i] == 0))) {
                    count++;
                }
            }
        }
    }

    return count;
}
iPair calc_twos(int cb[6][6], int p)
{
    iPair count = {0, 0, 0};
    for (int i=0; i<6; ++i) {
        bool line_x = false, line_y = false;
        for (int j=0; j<6; ++j) {
            // only space can work
            int m_count = 0;
            if (i > 0 && cb[i-1][j] == p) m_count++;
            if (i < 5 && cb[i+1][j] == p) m_count++;
            if (cb[i][j] == 0) {

                // from left
                if (j>1 && (j == 2 || cb[i][j-3] != p)
                        && cb[i][j-2] == p && cb[i][j-1] == p
                        && (j == 5 || cb[i][j+1] != p)) {
                    count.x++;
                    count.y += m_count;
                    line_x = true;
                }

                // from middle
                if (j>0 && j<5
                        && (j == 1 || cb[i][j-2] != p)
                        && (j == 4 || cb[i][j+2] != p)
                        && cb[i][j+1] == p && cb[i][j-1] == p) {
                    count.x++;
                    count.y += m_count;
                    line_x = true;
                }

                // from right
                if (j<4 && (j == 3 || cb[i][j+3] != p)
                        && cb[i][j+2] == p && cb[i][j+1] == p
                        && (j == 0 || cb[i][j-1] != p)) {
                    count.x++;
                    count.y += m_count;
                    line_x = true;
                }
            }
            m_count = 0;
            if (i > 0 && cb[j][i-1] == p) m_count++;
            if (i < 5 && cb[j][i+1] == p) m_count++;
            if (cb[j][i] == 0) {
                // from left
                if (j>1 && (j == 2 || cb[j-3][i] != p)
                        && cb[j-2][i] == p && cb[j-1][i] == p
                        && (j == 5 || cb[j+1][i] != p)) {
                    count.x++;
                    count.y += m_count;
                    line_y = true;
                }

                // from middle
                if (j>0 && j<5
                        && (j == 1 || cb[j-2][i] != p)
                        && (j == 4 || cb[j+2][i] != p)
                        && cb[j+1][i] == p && cb[j-1][i] == p) {
                    count.x++;
                    count.y += m_count;
                    line_y = true;
                }

                // from right
                if (j<4 && (j == 3 || cb[j+3][i] != p)
                        && cb[j+2][i] == p && cb[j+1][i] == p
                        && (j == 0 || cb[j-1][i] != p)) {
                    count.x++;
                    count.y += m_count;
                    line_y = true;
                }
            }
        }

        if (line_x) count.z++;
        if (line_y) count.z++;
    }

    return count;
}

bool cutoff_test(State &s)
{

    if (s.lost[0] >= 10 || s.lost[1] >= 10)
        return true;

    if (g_cutOne) {
        if (g_cutTwo) {
            return true;
        }
        return s.depth >= 2;
    }

    if (g_currentState.remains[0] == 0
            && g_currentState.remains[1] == 0) {
        if (s.lost[0] > 5 && s.lost[1] > 5)
            return s.depth >= 2 + DEPTH_INC;
    }
    return s.depth >= DEPTH_INC;
}
int eval(State &s)
{
    if (!g_cutOne) {
        if (time(NULL) - g_startTime > 20) {
            g_cutOne = true;
        }
    } else if (!g_cutTwo) {
        if (time(NULL) - g_startTime > 30) {
            g_cutTwo = true;
        }
    }
    if (s.lost[g_enemy-1] >= 10) {
        return D_INF - s.lost[g_player-1];
    }
    if (s.lost[g_player-1] >= 10) {
        return D_NEGINF + s.lost[g_enemy-1];
    }

    iPair pPair = calc_twos(s.chessboard, g_player);
    iPair ePair = calc_twos(s.chessboard, g_enemy);
    int pmoveo = calc_moves_out(s.chessboard, g_player);
    int emoveo = calc_moves_out(s.chessboard, g_enemy);
    if (s.remains[g_player-1] > 1) {
        pmoveo /= (g_sndHand ? V_REDUB : V_REDU);
        pPair.y /= (g_sndHand ? V_REDUB : V_REDU);
    }
    if (s.remains[g_enemy-1] > 1) {
        emoveo /= (g_sndHand ? V_REDUB : V_REDU);
        ePair.y /= (g_sndHand ? V_REDUB : V_REDU);
    }
    if (s.remains[g_player-1] == 0) {
        pPair.x = 0;
        pPair.z = 0;
    }
    if (s.remains[g_enemy-1] == 0) {
        ePair.x = 0;
        ePair.z = 0;
    }
    int value =
        (s.lost[g_enemy-1]-s.lost[g_player-1]) * (g_sndHand ? V_LOSTB : V_LOST)
        + (pPair.x-ePair.x) * (g_sndHand ? V_TWOB : V_TWO)
        + (pPair.y-ePair.y) * (g_sndHand ? V_MOVEB : V_MOVE)
        + (pmoveo-emoveo) * (g_sndHand ? V_MOVEOB : V_MOVEO)
        + (pPair.z-ePair.z) * (g_sndHand ? V_LINEB : V_LINE);

    return value;
}

bool calc_drop_capture(int cb[6][6], int p, int i, int j);
bool calc_move_capture(int cb[6][6], int p,
        int i, int j, int ni, int nj)
{
    if (calc_drop_capture(cb, p, ni, nj)) {
        return true;
    }

    // from left
    if (j > 2 && cb[i][j-3] == p
            && cb[i][j-2] == p && cb[i][j-1] == p
            && (j == 3 || cb[i][j-4] != p)) {
        return true;
    }

    // from right
    if (j < 3 && cb[i][j+3] == p
            && cb[i][j+2] == p && cb[i][j+1] == p
            && (j == 2 || cb[i][j+4] != p)) {
        return true;
    }

    // from top
    if (i > 2 && cb[i-3][j] == p
            && cb[i-2][j] == p && cb[i-1][j] == p
            && (i == 3 || cb[i-4][j] != p)) {
        return true;
    }

    // from bottom
    if (i < 3 && cb[i+3][j] == p
            && cb[i+2][j] == p && cb[i+1][j] == p
            && (i == 2 || cb[i+4][j] != p)) {
        return true;
    }
    return false;
}
bool calc_drop_capture(int cb[6][6], int p, int i, int j)
{
    // from left
    if (j>1 && (j == 2 || cb[i][j-3] != p)
            && cb[i][j-2] == p && cb[i][j-1] == p
            && (j == 5 || cb[i][j+1] != p)) {
        return true;
    }

    // from middle
    if (j>0 && j<5
            && (j == 1 || cb[i][j-2] != p)
            && (j == 4 || cb[i][j+2] != p)
            && cb[i][j+1] == p && cb[i][j-1] == p) {
        return true;
    }

    // from right
    if (j<4 && (j == 3 || cb[i][j+3] != p)
            && cb[i][j+2] == p && cb[i][j+1] == p
            && (j == 0 || cb[i][j-1] != p)) {
        return true;
    }

    // from top
    if (i>1 && (i == 2 || cb[i-3][j] != p)
            && cb[i-2][j] == p && cb[i-1][j] == p
            && (i == 5 || cb[i+1][j] != p)) {
        return true;
    }

    // from middle
    if (i>0 && i<5
            && (i == 1 || cb[i-2][j] != p)
            && (i == 4 || cb[i+2][j] != p)
            && cb[i+1][j] == p && cb[i-1][j] == p) {
        return true;
    }

    // from bottom
    if (i<4 && (i == 3 || cb[i+3][j] != p)
            && cb[i+2][j] == p && cb[i+1][j] == p
            && (i == 0 || cb[i-1][j] != p)) {
        return true;
    }

    return false;
}

Action alpha_beta_search(State &state, ActionCode acode)
{
    Action act = {0, 0, 0};
    State prev = state;
    state.depth = 1;
    int v = D_NEGINF, A = D_NEGINF;

    g_action = false;
    switch (acode) {
        case DROP:
            --state.remains[g_player-1];
            for (int i=0; i<6; ++i) {
                for (int j=0; j<6; ++j) {
                    if (state.remains[g_player-1] >= 10
                            && ((i != 2 && i != 3) || (j != 2 && j != 3))) {
                        continue;
                    }
                    if (state.chessboard[i][j] == 0) {
                        state.chessboard[i][j] = g_player;
                        int t;
                        bool captured = calc_drop_capture(state.chessboard, g_player, i, j);
                        if (captured) {
                            t = max_value(state, A, D_INF, true);
                        } else {
                            t = min_value(state, A, D_INF);
                        }
                        // choose this action
                        if (t > v || v == D_NEGINF || (v == t && captured)
                                || (v == t && !g_captured && eval(state) > eval(prev))) {
                            act.x = i;
                            act.y = j;
                            g_action = true;
                            v = t;
                            g_captured = captured;
                            prev = state;
                        }
                        A = max(A, v);
                        state.chessboard[i][j] = 0;
                    }
                }
            }
            break;
        case MOVE:
            for (int i=0; i<6; ++i) {
                for (int j=0; j<6; ++j) {
                    if (state.chessboard[i][j] == g_player) {
                        state.chessboard[i][j] = 0;
                        for (int k=0; k<4; ++k) {
                            int new_i = i + D_MOVES[k].x;
                            int new_j = j + D_MOVES[k].y;
                            if (new_i >= 0 && new_j >= 0 && new_i < 6 && new_j < 6
                                    && state.chessboard[new_i][new_j] == 0) {
                                state.chessboard[new_i][new_j] = g_player;
                                int t;
                                bool captured = calc_move_capture(state.chessboard, g_player,
                                        i, j, new_i, new_j);
                                if (captured) {
                                    t = max_value(state, A, D_INF, true);
                                } else {
                                    t = min_value(state, A, D_INF);
                                }

                                // choose this action
                                if (t > v || v == D_NEGINF || (v == t && captured)
                                        || (v == t && !g_captured && eval(state) > eval(prev))
                                        || (v == t && !g_captured &&
                                            state.remains[0] == 0 && state.remains[1] == 0
                                            && calc_dist(prev.chessboard) > calc_dist(state.chessboard))) {
                                    act.x = i;
                                    act.y = j;
                                    act.direction = k;
                                    g_action = true;
                                    v = t;
                                    g_captured = captured;
                                    prev = state;
                                }
                                A = max(A, v);
                                state.chessboard[new_i][new_j] = 0;
                            }
                        }
                        state.chessboard[i][j] = g_player;
                    }
                }
            }
            break;
        case CAPTURE:
            ++state.lost[g_enemy-1];
            for (int i=0; i<6; ++i) {
                for (int j=0; j<6; ++j) {
                    if (state.chessboard[i][j] == g_enemy) {
                        state.chessboard[i][j] = 0;
                        int t = min_value(state, A, D_INF);
                        // choose this action
                        if (t > v || v == D_NEGINF
                                || (v == t && eval(state) > eval(prev))) {
                            act.x = i;
                            act.y = j;
                            g_action = true;
                            v = t;
                            prev = state;
                        }
                        A = max(A, v);
                        state.chessboard[i][j] = g_enemy;
                    }
                }
            }
            break;
    }
    return act;
}


#if ENABLE_ERRMSG
void check_state(State &state)
{
    int onboard_e = 0, onboard_p = 0;
    for (int i=0; i<6; ++i) {
        for (int j=0; j<6; ++j) {
            if (state.chessboard[i][j] == g_player) onboard_p++;
            if (state.chessboard[i][j] == g_enemy) onboard_e++;
        }
    }
    if (state.remains[g_enemy-1]
            + onboard_e + state.lost[g_enemy-1] != 12) {
        printf("Something is totally wrong!! enemy mismatch!\n");
        printf("exp: %d - %d\n", state.remains[g_enemy-1],
                state.lost[g_enemy-1]);
        printf("actual: - %d -\n", onboard_e);
        print_state(state);
        exit(1);
    }
    int t = state.remains[g_player-1]
        + onboard_p + state.lost[g_player-1];
    if (t != 12) {

        printf("Something is totally wrong!! player mismatch!\n");
        printf("exp: %d - %d\n", state.remains[g_player-1],
                state.lost[g_player-1]);
        printf("actual: - %d -\n", onboard_p);
        exit(1);
    }
}
#endif

int max_value(State state, int A, int B, bool isCapture)
{
    if (!isCapture) {
        if (cutoff_test(state)) {
            return eval(state);
        }
        ++state.depth;
    }

    int v = D_NEGINF;
    const int cEnemy = g_enemy;
    const int cPlayer = g_player;

    if (isCapture) {
        ++state.lost[cEnemy-1];
        for (int i=0; i<6; ++i) {
            for (int j=0; j<6; ++j) {
                if (state.chessboard[i][j] == cEnemy) {
                    state.chessboard[i][j] = 0;
                    v = max(v, min_value(state, A, B));
#if ENABLE_PRUN
                    if (v >= B) {
                        return v;
                    }
#endif
                    A = max(A, v);
                    state.chessboard[i][j] = cEnemy;
                }
            }
        }
    } else if (state.remains[cPlayer-1]) {
        // DROP
        --state.remains[cPlayer-1];
        for (int i=0; i<6; ++i) {
            for (int j=0; j<6; ++j) {
                if (state.remains[cPlayer-1] >= 10
                        && ((i != 2 && i != 3) || (j != 2 && j != 3))) {
                    continue;
                }
                if (state.chessboard[i][j] == 0) {
                    state.chessboard[i][j] = cPlayer;
                    if (calc_drop_capture(state.chessboard, cPlayer, i, j)) {
                        int t = max(v, max_value(state, A, B, true));
                        v = max(v, t);
                    } else {
                        v = max(v, min_value(state, A, B));
                    }
#if ENABLE_PRUN
                    if (v >= B) {
                        return v;
                    }
#endif
                    A = max(A, v);
                    state.chessboard[i][j] = 0;
                }
            }
        }
    } else {
        // MOVE
        for (int i=0; i<6; ++i) {
            for (int j=0; j<6; ++j) {
                if (state.chessboard[i][j] == cPlayer) {
                    state.chessboard[i][j] = 0;
                    for (int k=0; k<4; ++k) {
                        int new_i = i + D_MOVES[k].x;
                        int new_j = j + D_MOVES[k].y;
                        if (new_i >= 0 && new_j >= 0 && new_i < 6 && new_j < 6
                                && state.chessboard[new_i][new_j] == 0) {
                            state.chessboard[new_i][new_j] = cPlayer;

                            if (calc_move_capture(state.chessboard, cPlayer, i, j,
                                        new_i, new_j)) {
                                v = max(v, max_value(state, A, B, true));
                            } else {
                                v = max(v, min_value(state, A, B));
                            }
#if ENABLE_PRUN
                            if (v >= B) {
                                return v;
                            }
#endif
                            A = max(A, v);
                            state.chessboard[new_i][new_j] = 0;
                        }
                    }
                    state.chessboard[i][j] = cPlayer;
                }
            }
        }
    }

    if (v == D_NEGINF) {
        return D_NEGINF + state.lost[cEnemy-1];
    } else {
        return v;
    }
}

int min_value(State state, int A, int B, bool isCapture)
{
    if (!isCapture) {
        if (cutoff_test(state)) {
            return eval(state);
        }
        ++state.depth;
    }

    int v = D_INF;
    const int cEnemy = g_player;
    const int cPlayer = g_enemy;

    if (isCapture) {
        ++state.lost[cEnemy-1];
        for (int i=0; i<6; ++i) {
            for (int j=0; j<6; ++j) {
                if (state.chessboard[i][j] == cEnemy) {
                    state.chessboard[i][j] = 0;
                    v = min(v, max_value(state, A, B));
#if ENABLE_PRUN
                    if (v <= A) {
                        return v;
                    }
#endif
                    B = min(B, v);
                    state.chessboard[i][j] = cEnemy;
                }
            }
        }
    } else if (state.remains[cPlayer-1]) {
        // DROP
        --state.remains[cPlayer-1];
        for (int i=0; i<6; ++i) {
            for (int j=0; j<6; ++j) {
                if (state.remains[cPlayer-1] >= 10
                        && ((i != 2 && i != 3) || (j != 2 && j != 3))) {
                    continue;
                }
                if (state.chessboard[i][j] == 0) {
                    state.chessboard[i][j] = cPlayer;
                    if (calc_drop_capture(state.chessboard, cPlayer, i, j)) {
                        v = min(v, min_value(state, A, B, true));
                    } else {
                        v = min(v, max_value(state, A, B));
                    }
#if ENABLE_PRUN
                    if (v <= A) {
                        return v;
                    }
#endif
                    B = min(B, v);
                    state.chessboard[i][j] = 0;
                }
            }
        }
    } else {
        // MOVE
        for (int i=0; i<6; ++i) {
            for (int j=0; j<6; ++j) {
                if (state.chessboard[i][j] == cPlayer) {
                    state.chessboard[i][j] = 0;
                    for (int k=0; k<4; ++k) {
                        int new_i = i + D_MOVES[k].x;
                        int new_j = j + D_MOVES[k].y;
                        if (new_i >= 0 && new_j >= 0 && new_i < 6 && new_j < 6
                                && state.chessboard[new_i][new_j] == 0) {
                            state.chessboard[new_i][new_j] = cPlayer;

                            if (calc_move_capture(state.chessboard, cPlayer, i, j,
                                        new_i, new_j)) {
                                v = min(v, min_value(state, A, B, true));
                            } else {
                                v = min(v, max_value(state, A, B));
                            }
#if ENABLE_PRUN
                            if (v <= A) {
                                return v;
                            }
#endif
                            B = min(B, v);
                            state.chessboard[new_i][new_j] = 0;
                        }
                    }
                    state.chessboard[i][j] = cPlayer;
                }
            }
        }
    }

    if (v == D_INF) {
        return D_INF - state.lost[cEnemy-1];
    } else {
        return v;
    }
}


void load_chessboard(int[6][6]);
Action action(const char *cmd, int err_msg);

int main(int argc, char * argv[])
{
    if (argc > 1) {
        g_player = atoi(argv[1]);
        g_enemy = (g_player % 2) + 1;
    }

#if ENABLE_INPUT
    if (argc > 3) {
        printf("TTTT\n");
        FILE *f = fopen(argv[2], "r");
        fscanf(f, "%d%d%d%d%d%d%d\n",
                &V_LOST, &V_TWO, &V_MOVE, &V_MOVEO, &V_REDU, &V_DEPTH, &V_LINE);
        fclose(f);

        f = fopen(argv[3], "r");
        fscanf(f, "%d%d%d%d%d%d%d\n",
                &V_LOSTB, &V_TWOB, &V_MOVEB, &V_MOVEOB, &V_REDUB, &V_DEPTHB, &V_LINEB);
        fclose(f);
    }
#endif
    int snum = 0;
    while (true) {
        FILE *file = fopen("request.txt", "r");
        if (file == NULL) {
            continue;
        }

        int t;
        fscanf(file, "%d", &t);
        if (t > snum) {
            snum = t;

            int err_msg;
            char cmd[100];
            fscanf(file, "%s %d", cmd, &err_msg);
            printf("Read: %d %s %d\n", snum, cmd, err_msg);

            load_chessboard(g_currentState.chessboard);
            Action output = action(cmd, err_msg);

            FILE *fout = fopen("action.txt", "w");
            snum++;
            if (!g_action) {
                printf("No action is possible!\n");
            } else if (strcmp(cmd, "/move")==0) {
                fprintf(fout, "%d %s %d %d %s", snum, cmd, output.x, output.y, D_OUT[output.direction]);
                printf("Write: %d %s %d %d %s\n", snum, cmd, output.x, output.y, D_OUT[output.direction]);
            } else {
                fprintf(fout, "%d %s %d %d ", snum, cmd, output.x, output.y);
                printf("Write: %d %s %d %d\n", snum, cmd, output.x, output.y);
            }
            fclose(fout);
        }
        fclose(file);
    }
    return 0;
}



bool first_time = true;
Action action(const char *cmd, int err_msg)
{
    g_startTime = time(NULL);
    g_cutOne = false;
    g_cutTwo = false;
    if (first_time) {
        g_currentState.remains[0] = 12;
        g_currentState.remains[1] = 12;
        g_currentState.lost[0] = 0;
        g_currentState.lost[1] = 0;

#if ENABLE_INPUT
        DEPTH_INC = V_DEPTH;
#endif
        for (int i=0; i<6; ++i) {
            for (int j=0; j<6; ++j) {
                if (g_currentState.chessboard[i][j] != 0) {
                    g_sndHand = 1;
#if ENABLE_INPUT
                    DEPTH_INC = V_DEPTHB;
#endif
                    g_currentState.remains[g_enemy-1] = 11;
                    break;
                }
            }
        }
        first_time = false;
    } else {
        if (g_currentState.remains[g_enemy-1] && !g_captured) {
#if ENABLE_ERRMSG
            if (strcmp(cmd, "/capture") == 0) {
                printf("strange!\n");
            }
#endif
            --g_currentState.remains[g_enemy-1];
        }
        int onboard_e = 0, onboard_p = 0;
        for (int i=0; i<6; ++i) {
            for (int j=0; j<6; ++j) {
                if (g_currentState.chessboard[i][j] == g_player) onboard_p++;
                if (g_currentState.chessboard[i][j] == g_enemy) onboard_e++;
            }
        }
#if ENABLE_ERRMSG
        if (g_currentState.remains[g_enemy-1]
                + onboard_e + g_currentState.lost[g_enemy-1] != 12) {
            printf("Something is totally wrong!! enemy mismatch as %d!\n", g_enemy);
            printf("exp: %d - %d\n", g_currentState.remains[g_enemy-1],
                    g_currentState.lost[g_enemy-1]);
            printf("actual: - %d -\n", onboard_e);
        }
#endif
        int t = g_currentState.remains[g_player-1]
            + onboard_p + g_currentState.lost[g_player-1];
        if (t == 11) g_currentState.lost[g_player-1]++;
#if ENABLE_ERRMSG
        else if (t != 12) {

            printf("Something is totally wrong!! player mismatch! as %d!\n", g_player);
            printf("exp: %d - %d\n", g_currentState.remains[g_player-1],
                    g_currentState.lost[g_player-1]);
            printf("actual: - %d -\n", onboard_p);
            exit(1);
        }
#endif
    }

    ActionCode act;
    if (strcmp(cmd, "/drop") == 0) {
        act = DROP;
    } else if (strcmp(cmd, "/move") == 0) {
        act = MOVE;
    } else {
        act = CAPTURE;
    }

    if (g_captured) {
#if ENABLE_ERRMSG
        if (act != CAPTURE) {
            printf("Something is totally wrong!! expect capture!\n");
            for (int i=0; i<6; ++i) {
                for (int j=0; j<6; ++j) {
                    printf("%d,", g_currentState.chessboard[i][j]);
                }
                printf("\n");
            }
            exit(1);
        }
#endif
        g_captured = false;
#if ENABLE_ERRMSG
    } else {
        if (act == CAPTURE) {
            printf("Something is totally wrong!! do not expect capture!\n");
            exit(1);
        }
    }
    if (g_currentState.remains[g_player-1]) {
        if (act == MOVE) {
            printf("Something is totally wrong!! do not expect move!\n");
            exit(1);
        }
    } else {
        if (act == DROP) {
            printf("Something is totally wrong!! do not expect drop!\n");
            exit(1);
        }
#endif
    }

    Action point = alpha_beta_search(g_currentState, act);
    return point;
}

void load_chessboard(int chessboard[6][6])
{
    FILE *file = fopen("chessboard.txt", "r");
    if (file == NULL) {
        for (int i=0; i<6; i++)
            for (int j=0; j<6; j++)
                chessboard[i][j] = 0;
        return;
    }

    for (int i=0; i<6; i++) {
        fscanf(file, "%d%d%d%d%d%d",
                &chessboard[i][0],
                &chessboard[i][1],
                &chessboard[i][2],
                &chessboard[i][3],
                &chessboard[i][4],
                &chessboard[i][5]);
    }
    fclose(file);
}

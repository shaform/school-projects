#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <climits>
#include <algorithm>
using namespace std;

const char *D_OUT[] = {"up", "right", "down", "left", ""};
const int D_UP = 0, D_RIGHT = 1, D_DOWN = 2, D_LEFT = 3, D_NO = 4;
const int D_INF = INT_MAX, D_NEGINF = INT_MIN;
const struct { int x, y; } D_MOVES[] = {
    {-1, 0}, {0, 1}, {1, 0}, {0, -1},
};

struct Action {
    int x, y, direction;
    int utility;
};
enum ActionCode { DROP, CAPTURE, MOVE };

bool operator<(const Action &lhs, const Action &rhs)
{
    return lhs.utility < rhs.utility;
}

// -- internal states -- //

struct State {
    int chessboard[6][6];
    int remains[2];
    int lost[2];
    int depth;
} g_currentState;

int g_player = 1;
int g_enemy = 2;
int g_sndHand = 0;
bool g_captured = false;

// -- function prototypes -- //
Action alpha_beta_search(State &, ActionCode);
static int min_max_value(State &, int, int, bool);
int min_max_value(State &, int, int, bool, bool);
bool cutoff_test(State &);
bool eval(State &);
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


bool cutoff_test(State &s)
{
    return s.depth >= 7;
}
bool eval(State &s)
{
    return s.lost[g_enemy-1] - s.lost[g_player-1];
}

void save_current_capture(int cb[6][6], int p, int capRows[6], int capCols[6])
{
    for (int i=0; i<6; ++i) {
        int m = 0;
        for (int j=0, t=0; j<6; ++j) {
            if (cb[i][j] == p) {
                ++t;
                m = max(m, t);
            } else {
                t = 0;
            }
        }
        capRows[i] = (m == 3);
    }
    for (int i=0; i<6; ++i) {
        int m = 0;
        for (int j=0, t=0; j<6; ++j) {
            if (cb[j][i] == p) {
                ++t;
                m = max(m, t);
            } else {
                t = 0;
            }
        }
        capCols[i] = (m == 3);
    }
}
bool calc_new_capture(int cb[6][6], int p, int capRows[6], int capCols[6])
{
    for (int i=0; i<6; ++i) {
        if (capRows[i]) continue;
        int m = 0;
        for (int j=0, t=0; j<6; ++j) {
            if (cb[i][j] == p) {
                ++t;
                m = max(m, t);
            } else {
                t = 0;
            }
        }
        if (m == 3) {
            return true;
        }
    }
    for (int i=0; i<6; ++i) {
        if (capCols[i]) continue;
        int m = 0;
        for (int j=0, t=0; j<6; ++j) {
            if (cb[j][i] == p) {
                ++t;
                m = max(m, t);
            } else {
                t = 0;
            }
        }
        if (m == 3) {
            return true;
        }
    }
    return false;
}

Action alpha_beta_search(State &state, ActionCode acode)
{
    Action act;
    state.depth = 1;
    int v = D_NEGINF, A = D_NEGINF;

    int capRows[6], capCols[6];
    switch (acode) {
        case DROP:
            --state.remains[g_player-1];
            save_current_capture(state.chessboard, g_player, capRows, capCols);
            for (int i=0; i<6; ++i) {
                for (int j=0; j<6; ++j) {
                    if (state.remains[g_player-1] >= 10
                            && ((i != 2 && i != 3) || (j != 2 && j != 3))) {
                        continue;
                    }
                    if (state.chessboard[i][j] == 0) {
                        state.chessboard[i][j] = g_player;
                        int t;
                        bool captured = calc_new_capture(state.chessboard, g_player, capRows, capCols);
                        if (captured) {
                            t = min_max_value(state, A, D_INF, true, true);
                        } else {
                            t = min_max_value(state, A, D_INF, false);
                        }
                        state.chessboard[i][j] = 0;
                        // choose this action
                        if (t > v || v == D_NEGINF) {
                            act.x = i;
                            act.y = j;
                            v = t;
                            g_captured = captured;
                            printf("test %d %d v: %d\n", i, j, v);
                        }
                        A = max(A, v);
                        state.chessboard[i][j] = 0;
                    }
                }
            }
            break;
        case MOVE:
            save_current_capture(state.chessboard, g_player, capRows, capCols);
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
                                bool captured = calc_new_capture(state.chessboard, g_player,
                                        capRows, capCols);
                                if (captured) {
                                    t = min_max_value(state, A, D_INF, true, true);
                                } else {
                                    t = min_max_value(state, A, D_INF, false);
                                }
                                state.chessboard[new_i][new_j] = 0;

                                // choose this action
                                if (t > v || v == D_NEGINF) {
                                    act.x = i;
                                    act.y = j;
                                    act.direction = k;
                                    v = t;
                                    g_captured = captured;
                                }
                                A = max(A, v);
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
                        int t = min_max_value(state, A, D_INF, false);
                        // choose this action
                        if (t > v || v == D_NEGINF) {
                            act.x = i;
                            act.y = j;
                            v = t;
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

static int min_max_value(State &state, int A, int B, bool isMax)
{
    return min_max_value(state, A, B, isMax, false);
}
int min_max_value(State &state, int A, int B, bool isMax, bool isCapture)
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
        printf("Something is totally wrong!! enemy mismatch! in %d\n", isMax);
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
    if (!isCapture && cutoff_test(state)) {
        return eval(state);
    }

    ++state.depth;

    int v, cPlayer, cEnemy;
    if (isMax) {
        v = D_NEGINF;
        cPlayer = g_player;
        cEnemy = g_enemy;
    } else {
        v = D_INF;
        cPlayer = g_enemy;
        cEnemy = g_player;
    }

    int capRows[6], capCols[6];
    if (isCapture) {
        ++state.lost[cEnemy-1];
        for (int i=0; i<6; ++i) {
            for (int j=0; j<6; ++j) {
                if (state.chessboard[i][j] == cEnemy) {
                    state.chessboard[i][j] = 0;
                    if (isMax) {
                        v = max(v, min_max_value(state, A, B, false));
                        if (v >= B) {
                            --state.depth;
                            --state.lost[cEnemy-1];
                            state.chessboard[i][j] = cEnemy;
                            return v;
                        }
                        A = max(A, v);
                    } else {
                        v = min(v, min_max_value(state, A, B, true));
                        if (v <= A) {
                            --state.depth;
                            --state.lost[cEnemy-1];
                            state.chessboard[i][j] = cEnemy;
                            return v;
                        }
                        B = min(B, v);
                    }
                    state.chessboard[i][j] = cEnemy;
                }
            }
        }
        --state.lost[cEnemy-1];
    } else if (state.remains[cPlayer-1]) {
        // DROP
        --state.remains[cPlayer-1];
        save_current_capture(state.chessboard, cPlayer, capRows, capCols);
        for (int i=0; i<6; ++i) {
            for (int j=0; j<6; ++j) {
                if (state.remains[cPlayer-1] >= 10
                        && ((i != 2 && i != 3) || (j != 2 && j != 3))) {
                    continue;
                }
                if (state.chessboard[i][j] == 0) {
                    state.chessboard[i][j] = cPlayer;
                    if (isMax) {
                        if (calc_new_capture(state.chessboard, cPlayer, capRows, capCols)) {
                            v = max(v, min_max_value(state, A, B, true, true));
                        } else {
                            v = max(v, min_max_value(state, A, B, false));
                        }
                        if (v == D_INF) {
                            printf("too many!\n");
                            print_state(state);
                        }
                        if (v >= B) {
                            ++state.remains[cPlayer-1];
                            state.chessboard[i][j] = 0;
                            --state.depth;
                            return v;
                        }
                        A = max(A, v);
                    } else {
                        if (calc_new_capture(state.chessboard, cPlayer, capRows, capCols)) {
                            v = min(v, min_max_value(state, A, B, false, true));
                        } else {
                            v = min(v, min_max_value(state, A, B, true));
                        }
                        if (v == D_NEGINF) {
                            printf("too little!\n");
                        }
                        if (v <= A) {
                            ++state.remains[cPlayer-1];
                            state.chessboard[i][j] = 0;
                            --state.depth;
                            return v;
                        }
                        B = min(B, v);
                    }
                    state.chessboard[i][j] = 0;
                }
            }
        }
        ++state.remains[cPlayer-1];
    } else {
        // MOVE
        save_current_capture(state.chessboard, cPlayer, capRows, capCols);
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

                            if (isMax) {
                                if (calc_new_capture(state.chessboard, cPlayer, capRows, capCols)) {
                                    v = max(v, min_max_value(state, A, B, true, true));
                                } else {
                                    v = max(v, min_max_value(state, A, B, false));
                                }
                                if (v == D_INF) {
                                    printf("too many!\n");
                                    print_state(state);
                                }
                                if (v >= B) {
                                    --state.depth;
                                    state.chessboard[new_i][new_j] = 0;
                                    state.chessboard[i][j] = cPlayer;
                                    return v;
                                }
                                A = max(A, v);
                            } else {
                                if (calc_new_capture(state.chessboard, cPlayer, capRows, capCols)) {
                                    v = min(v, min_max_value(state, A, B, false, true));
                                } else {
                                    v = min(v, min_max_value(state, A, B, true));
                                }
                                if (v == D_NEGINF) {
                                    printf("too little!\n");
                                }
                                if (v <= A) {
                                    --state.depth;
                                    state.chessboard[new_i][new_j] = 0;
                                    state.chessboard[i][j] = cPlayer;
                                    return v;
                                }
                                B = min(B, v);
                            }


                            state.chessboard[new_i][new_j] = 0;
                        }
                    }
                    state.chessboard[i][j] = cPlayer;
                }
            }
        }
    }

    --state.depth;

    return v;
}

void load_chessboard(int[6][6]);
Action action(const char *cmd, int err_msg);

int main(int argc, char * argv[])
{
    if (argc > 1) {
        g_player = atoi(argv[1]);
        g_enemy = (g_player % 2) + 1;
    }
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
            if (strcmp(cmd, "/move")==0) {
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
    if (first_time) {
        g_currentState.remains[0] = 12;
        g_currentState.remains[1] = 12;
        g_currentState.lost[0] = 0;
        g_currentState.lost[1] = 0;

        for (int i=0; i<6; ++i) {
            for (int j=0; j<6; ++j) {
                if (g_currentState.chessboard[i][j] != 0) {
                    g_sndHand = 1;
                    g_currentState.remains[g_enemy-1] = 11;
                    break;
                }
            }
        }
        first_time = false;
    } else {
        if (g_currentState.remains[g_enemy-1] && !g_captured) {
            if (strcmp(cmd, "/capture") == 0) {
                printf("strange!\n");
            }
            --g_currentState.remains[g_enemy-1];
        }
        int onboard_e = 0, onboard_p = 0;
        for (int i=0; i<6; ++i) {
            for (int j=0; j<6; ++j) {
                if (g_currentState.chessboard[i][j] == g_player) onboard_p++;
                if (g_currentState.chessboard[i][j] == g_enemy) onboard_e++;
            }
        }
        if (g_currentState.remains[g_enemy-1]
                + onboard_e + g_currentState.lost[g_enemy-1] != 12) {
            printf("Something is totally wrong!! enemy mismatch!\n");
            printf("exp: %d - %d\n", g_currentState.remains[g_enemy-1],
                    g_currentState.lost[g_enemy-1]);
            printf("actual: - %d -\n", onboard_e);
            exit(1);
        }
        int t = g_currentState.remains[g_player-1]
            + onboard_p + g_currentState.lost[g_player-1];
        if (t == 11) g_currentState.lost[g_player-1]++;
        else if (t != 12) {

            printf("Something is totally wrong!! player mismatch!\n");
            printf("exp: %d - %d\n", g_currentState.remains[g_player-1],
                    g_currentState.lost[g_player-1]);
            printf("actual: - %d -\n", onboard_p);
            exit(1);
        }
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
        g_captured = false;
    } else {
        if (act == CAPTURE) {
            printf("Something is totally wrong!! do not expect capture!\n");
            //exit(1);
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
    }

    Action point = alpha_beta_search(g_currentState, act);
    printf("my current = %d\n", g_currentState.remains[g_player-1]);
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

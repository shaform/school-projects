#include <cstdio>
#include <cstring>
#include <cstdlib>
using namespace std;

const char *D_OUT[] = {"", "up", "right", "down", "left"};
const int D_NO = 0, D_UP = 1, D_RIGHT = 2, D_DOWN = 3, D_LEFT = 4;


struct Coordinate {
    int x, y, direction;
};

void load_chessboard(int[6][6]);
Coordinate action(int[6][6], const char *cmd, int err_msg);

int main(int argc, char * argv[])
{
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

            int chessboard[6][6];
            load_chessboard(chessboard);
            Coordinate output = action(chessboard, cmd, err_msg);

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


// -- internal states -- //
int sndHand = 0;
int remains[2];
int eaten[2];

Coordinate action(int chessboard[6][6], const char *cmd, int err_msg)
{
    Coordinate point;
    point.x = rand()%6;
    point.y = rand()%6;
    point.direction = rand()%5;
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

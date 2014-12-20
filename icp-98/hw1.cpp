#include <stdio.h>

void farey(int);

int main(void)
{

    int n;
    printf("Enter an integer >= 1: ");
    while (scanf("%d", &n) == 1) {
        farey(n);
        printf("Enter an integer >= 1: ");
    }

    return 0;
}


void farey(int n)
{
    int x0 = 0, x1 = 1, y0 = 1, y1 = n, xn, yn;
    int i=1;
    do {
        xn = ((y0+n)/y1)*x1 -x0;
        yn = ((y0+n)/y1)*y1 -y0;
        printf("%d/%d ", x0, y0);
        x0 = x1;
        y0 = y1;
        x1 = xn;
        y1 = yn;
        ++i;
    } while (x0 != 1 || y0 != 1);
    printf("%d/%d\n", x0, y0);
    printf("The Farey sequence of order %d has %d terms.\n", n, i);
}

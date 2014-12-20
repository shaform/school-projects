#include <stdio.h>

int main(void)
{

    const int MAXD = 21;
    char d[MAXD];
    unsigned int n;
    while (printf("Enter an unsigned integer >= 0: ") && scanf("%u", &n) == 1) {
        for (int i=0; i<MAXD; i++)
            d[i] = 0;
        for (unsigned u=1; u<=n; u++) {
            unsigned int num = u;
            for (int j=1; j < MAXD; num /= ++j) {
                d[j] += num % (j+1);
                if (d[j] > j) {
                    d[j+1] += d[j]/(j+1);
                    d[j] = d[j] % (j+1);
                }
            }
        }

        printf("sum(1..%u) = ", n);
        int i = MAXD-1;
        while (d[i]==0 && i > 0)
            --i;
        for (; i>0; --i)
            printf("%d*%d! + ", d[i], i);

        printf("0*0!\n");

    }

    return 0;
}


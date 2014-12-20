#include <stdio.h>

int a(int);
int b(int);

int main(void)
{
    int n;
    while (printf("Enter an integer >= 0: ") && scanf("%d", &n) == 1) {
        printf("a(%d) = %d\n", n, a(n));
        printf("b(%d) = %d\n", n, b(n));
    }

    return 0;
}


int a(int n)
{
    int i=0;
    while (i*(i+1)/2 < n) {
        ++i;
    }

    n = n - (i-1)*i/2;
    int ans = 1;
    if (n != i) {
        for (int j=2; j<=n; ++j)
            ans*=j;
    }

    return ans;
}


int b(int n)
{
    int f_1=0, f_2=1, f_n=1;
    while (f_2 <= n) {
        f_2 += f_1 + 1;
        f_1 = f_2 - f_1 -1;
        ++f_n;
    }

    return f_n;
}

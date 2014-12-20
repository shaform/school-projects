#include <stdio.h>

int main(void)
{

    int n;
    while (printf("Enter an integer >= 0: ") && scanf("%d", &n) == 1) {
        if (n<=1) {
            printf("Prime factorization of %d! = (0)\n", n); 
            continue;
        }
        printf("Prime factorization of %d! = (", n);

        int first_time = 1;
        for (int i=2; i<=n; ++i) {
            int is_prime = 1;

            for (int j=2; j*j<=i; ++j) {
                if (i%j == 0) {
                    is_prime = 0;
                    break;
                }
            }

            if (!is_prime)
                continue;

            int sum = 0, to_add = n;
            do {
                to_add = to_add/i;
                sum+= to_add;
            } while (to_add>0);

            if (!first_time)
                printf(" ");
            else
                first_time = 0;
            printf("%d", sum);

        }
        printf(")\n"); 
    }

    return 0;
}

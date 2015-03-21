#include <stdio.h>

#define LEN 20

struct stack {
    int top;
    int stk[LEN];
};

struct stack s = { -1 };

/* Functions to compute the ways */
int p(int n);
int q(int n, int k);

/* Functions to handle input/output */
void ui(void);
void show(int stk[], int i);

int main(void)
{
    ui();
    return 0;
}



int p(int n)
{
    return q(n,1);
}


int q(int n, int k)
{
    if (n<k) {
        return 0;
    } else if (n==k) {
        s.stk[++s.top] = k;
        show(s.stk, s.top);
        printf("\n");

        --s.top;
        return 1;
    } else {
        /* Process partitions made of larger numbers first */
        int a = q(n, k+1);

        /* Choose k and add it to the stack */
        s.stk[++s.top] = k;
        int b = q(n-k, k);
        --s.top;

        return a + b;
    }
}

void ui(void)
{
    /* Process one input */
    int n;
    printf("Enter an integer <=20: ");

    // Read an number and proceed if succeeded
    if (scanf("%d", &n) == 1) {
        printf("There are %d ways to partition %d\n", p(n), n);

        /* Process next input */
        ui();
    }
}

void show(int stk[], int i)
{
    // Exit if there is nothing left to print
    if (i<0)
        return;

    // Print the current number
    printf("%d", stk[i]);

    // Print a trailing space if there are still some numbers to print
    if (i!=0)
        printf(" ");

    // Print the next number
    show(stk, i-1);
}

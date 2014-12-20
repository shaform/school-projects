#include <stdio.h>

const int BITS = 32;

void binary_qsort(unsigned *arr, int size);
void res(int bits, unsigned *left, unsigned *right);
unsigned *partition(int bits, unsigned *left, unsigned *right);

/* swap two unsigned integers */
void swapu(unsigned *a, unsigned *b);

int main(void)

{

    printf("Test 1 ...\n");

    unsigned a[9]={0x77777777,0x33333333,0x99999999,0x22222222,0x11111111,0x88888888,0x66666666,0x55555555,0x44444444};

    binary_qsort(a,9);     

    for (int i=0;i<9;i++) printf("%x ",a[i]);

    printf("\n");

    printf("Test 2 ...\n");

    unsigned b[15]={177,129,66,188,166,255,99,128,127,254,77,88,199,55,155};

    binary_qsort(b,15);

    for (int i=0;i<15;i++) printf("%u ",b[i]);

    printf("\n");

}

void binary_qsort(unsigned *arr, int size)
{
    res(BITS, arr, arr+size-1);
}

void res(int bits, unsigned *left, unsigned *right)
{
    if (right>left) {
        unsigned int *m = partition(bits, left, right);
        if (bits > 1) {
            res(bits-1, left, m);
            res(bits-1, m+1, right);
        }
    }
}
unsigned *partition(int bits, unsigned *left, unsigned *right)
{
    unsigned mask = 1u << (bits-1);
    unsigned *i = left, *j = right+1;
    while (i != j) {
        if (((*i) & mask) == 0u) {
            ++i;
        } else {
            swapu(i, --j);
            /*
               swapu(bits, i, --j);
               */
        }
    }
    return i-1;
}

void swapu(int bits, unsigned *a, unsigned *b)
{
    unsigned temp = 0u;
    unsigned mask;
    if (bits >=32)
        mask = 0u - 1u;
    else
        mask = (1u << bits) - 1u;

    temp |= ((*a) & mask);

    *a &= (~mask);
    *a |= ((*b) & mask);

    *b &= (~mask);
    *b |= temp;
}

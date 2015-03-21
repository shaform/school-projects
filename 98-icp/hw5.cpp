#include <stdio.h>

typedef unsigned bcd, decimal;

//A BCD Mask: 1111
bcd BCDM = 0xFu;
int MAXD = 8, BCDD = 4;

// convert a decimal to a BCD
bcd dec2bcd(decimal); 
// convert a BCD to a decimal
decimal bcd2dec(bcd);
// add two BCDs
bcd add(bcd, bcd);
// subtract BCD b from BCD a
bcd sub(bcd a, bcd b);

int main(void)
{

    int a, b;
    char op;
    while (printf("Enter a+b or a-b: ") && scanf("%u %c%u", &a, &op, &b) == 3) {
        int output;
        if (op == '-')
            output = bcd2dec(sub(dec2bcd(a), dec2bcd(b)));
        else if (op == '+')
            output = bcd2dec(add(dec2bcd(a), dec2bcd(b)));
        printf("%u %c %u = %u\n", a, op, b, output);
    }
    return 0;
}

bcd dec2bcd(decimal d)
{
    bcd output = 0;
    for (int i=0; i<MAXD && d!=0; ++i) {
        output |= (d%10u)<<i*BCDD;
        d/=10u;
    }
    return output;
}
decimal bcd2dec(bcd b)
{
    decimal output = 0;
    for (int i=0, insert_p=1; i<MAXD && b!=0; ++i, insert_p*=10) {
        output += (b&BCDM)*insert_p;
        b>>=BCDD;
    }
    return output;
}
bcd add(bcd a, bcd b)
{
    bcd output = a + b;
    //Correct the BCD
    decimal carry = 0;
    for (int i=0; i<MAXD; ++i) {
        if (((a>>i*BCDD)&BCDM) + ((b>>i*BCDD)&BCDM) + carry > 9u) {
            output += 6u<<i*BCDD;
            carry = 1;
        } else carry = 0;
    }
    return output;
}
bcd sub(bcd a, bcd b)
{
    bcd output = a - b;
    //Correct the BCD
    decimal borrow = 0;
    for (int i=0; i<MAXD; ++i) {
        if (((a>>i*BCDD)&BCDM) < ((b>>i*BCDD)&BCDM) + borrow) {
            output -= 6u<<i*BCDD;
            borrow = 1;
        } else borrow = 0;
    }
    return output;
}

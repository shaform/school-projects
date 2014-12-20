#include <iostream>
#include "stack6.h"
using namespace std;   

    template<typename T>
void prints(stack<T> s)
{
    while (s.size()) {
        cout << s.top();
        s.pop();
    }
    cout << endl;
}

    template<typename T>
int part_res(T *a, int i, T j, stack<T> &s)
{
    if (i==1) {
        if (j==0) {
            prints(s);
            return 1;
        } else if (j==a[0]) {
            s.push(a[0]);
            prints(s);
            s.pop();
            return 1;
        } else
            return 0;
    } else {
        int op_l = 0;
        if (j-a[i-1] >= 0) {
            s.push(a[i-1]);
            op_l = part_res(a, i-1, j-a[i-1], s);
            s.pop();
        }
        return op_l + part_res(a, i-1, j, s);
    }
}

    template<typename T>
int partition(T *a,int n)
{
    T sum(0);
    for (int i=0; i<n; ++i)
        sum +=a[i];

    if (sum % 2)
        return 0;

    stack<T> s;
    return part_res(a, n, sum/2, s);
}

int main()
{
    cout << "Test 1 ...\n";
    int a[7]={1,2,3,4,5,6,7}; 
    cout << partition(a,7) << " subset(s) in total.\n\n";

    cout << "Test 2 ...\n";
    unsigned b[8]={1,2,3,4,5,6,7,8}; 
    cout << partition(b,8) << " subset(s) in total.\n\n";

    cout << "Test 3 ...\n";
    int c[6]={7,77,777,7777,77777,777777}; 
    cout << partition(c,6) << " subset(s) in total.\n\n";

    return 0;
}    







#include <iostream>
#include "vector5.h"
using namespace std;



    template<typename T>
int partition(T* a,int n)
{
    if (n==1 || n== 2)
        return 0;

    T sum(0);
    for (int i=0; i<n; ++i)
        sum +=a[i];

    if (sum % 2)
        return 0;

    T half(sum/2);

    vector<vector<T> > vec(n, vector<T>(half+1, 0));
    vec[0][0] = vec[0][a[0]] = 1;
    for (int i=1; i<n; ++i)
        for (T j=0; j<=half; ++j)
            if (j-a[i] < 0)
                vec[i][j] = vec[i-1][j];

            else
                vec[i][j] = vec[i-1][j-a[i]] + vec[i-1][j];
    return vec[n-1][half]/2;
}


int main()
{
    int a[7]={7,6,5,4,3,2,1}; cout << partition(a,7) << endl;
    short b[12]={7,6,5,8,4,9,10,3,11,2,1,12}; cout << partition(b,12) << endl;
    unsigned c[1]={8}; cout << partition(c,1) << endl;
    char d[5]={1,3,5,7,12}; cout << partition(d,5) << endl;
    int e[6]={7,77,777,7777,77777,777777}; cout << partition(e,6) << endl;

    return 0;
}

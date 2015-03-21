#include <iostream>
using namespace std;

inline void mcs(char c,int l,int r)   //output  function 
{
    if (l<=r) cout << " : " << c << "[" << l << ".." << r <<  "] is an mcs\n";
    else cout << " : empty sequence\n";
}

// Version B1 ;V array as pointer
template<typename T>
T mcs(T* a,int n,int& l,int& r)
{
    if (n==1) {
        if (a[0] >= 0) {
            l = r = 0;
            return a[0];
        } else {
            r = 0;
            l = 1;
            return 0;
        }
    } else if (n<1) {
        l = 0;
        r = 1;
        return 0;
    }

    // Compute the halves
    const int mid = n/2;
    int l_a, r_a, l_b, r_b;

    T max_a=mcs(a, mid, l_a, r_a), max_b=mcs(a+mid, n-mid, l_b, r_b);

    // Compute the cross seqs
    l = mid-1;
    r = mid;

    T sum_front_so_far=a[l], sum_front_max=a[l];
    T sum_end_so_far=a[r], sum_end_max=a[r];

    for (int i=l-1; i>=0; --i)
    {
        sum_front_so_far += a[i];
        if (sum_front_so_far > sum_front_max) {
            l=i;
            sum_front_max = sum_front_so_far;
        }
    }
    for (int i=r+1; i<n; ++i)
    {
        sum_end_so_far += a[i];
        if (sum_end_so_far > sum_end_max) {
            r=i;
            sum_end_max = sum_end_so_far;
        }
    }

    T sum_cross_max = sum_front_max + sum_end_max;

    if (sum_cross_max > max_a && sum_cross_max > max_b) {
        return sum_cross_max;
    } else if (max_a > max_b) {
        l = l_a;
        r = r_a;
        return max_a;
    } else {
        l = l_b+mid;
        r = r_b+mid;
        return max_b;
    }
}
// Version B2 ;V array as array
template<typename T,int n>
T mcs(T (&a)[n],int& l,int& r)
{
    // Compute the halves
    const int mid = n/2;
    int l_a, r_a, l_b, r_b;

    T max_a=mcs(reinterpret_cast<T(&)[mid]>(a[0]), l_a, r_a), max_b=mcs(reinterpret_cast<T(&)[n-mid]>(a[mid]), l_b, r_b);

    // Compute the cross seqs
    l=mid-1, r = mid;

    T sum_front_so_far=a[l], sum_front_max=a[l];
    T sum_end_so_far=a[r], sum_end_max=a[r];

    for (int i=l-1; i>=0; --i)
    {
        sum_front_so_far += a[i];
        if (sum_front_so_far > sum_front_max) {
            l=i;
            sum_front_max = sum_front_so_far;
        }
    }
    for (int i=r+1; i<n; ++i)
    {
        sum_end_so_far += a[i];
        if (sum_end_so_far > sum_end_max) {
            r=i;
            sum_end_max = sum_end_so_far;
        }
    }

    T sum_cross_max = sum_front_max + sum_end_max;

    if (sum_cross_max > max_a && sum_cross_max > max_b) {
        return sum_cross_max;
    } else if (max_a > max_b) {
        l = l_a;
        r = r_a;
        return max_a;
    } else {
        l = l_b+mid;
        r = r_b+mid;
        return max_b;
    }
}
template<typename T>
T mcs(T (&a)[1],int& l,int& r)
{
    if (a[0] >= 0) {
        l = r = 0;
        return a[0];
    } else {
        r = 0;
        l = 1;
        return 0;
    }
}

// Version C
template<int n,typename T> 
T mcs(T a[n],int& l,int& r)
{
    T maxsofar, maxinclude;

    if (a[0]>=0) {
        maxsofar = a[0];
        l = r = 0;
    } else {
        maxsofar = 0;
        r = 0;
        l = n;
    }

    maxinclude = a[0];
    int l_in=0, r_in=0;

    for (int i=1; i<n; ++i) {
        if (maxinclude >= 0) {
            maxinclude = maxinclude + a[i];
            r_in = i;
        } else {
            maxinclude = a[i];
            l_in = r_in = i;
        }

        if (maxsofar < maxinclude) {
            maxsofar = maxinclude;
            l = l_in;
            r = r_in;
        }
    }

    return maxsofar;
}


int main()
{
    int a[13]={38,-62,47,-33,28,13,-18,-46,8,21,12,-53,25};  
    double b[10]={3.1,-4.1,5.9,2.6,-5.3,5.8,9.7,-9.3,-2.3,8.4};   
    int c[8]={1,2,3,-100,3,3,-20,6};
    int d[9]={-1,-2,-3,-4,-5,-6,-7,-8,-9};

    cout << "Test 1\n";
    { int l,r; cout << mcs(a,13,l,r); mcs('a',l,r); }
    { int l,r; cout << mcs(b,10,l,r); mcs('b',l,r); }
    { int l,r; cout << mcs(c,8,l,r);  mcs('c',l,r); }
    { int l,r; cout << mcs(d,9,l,r);  mcs('d',l,r); }

    cout << "\nTest 2\n";
    { int l,r; cout << mcs(a,l,r); mcs('a',l,r); }
    { int l,r; cout << mcs(b,l,r); mcs('b',l,r); }
    { int l,r; cout << mcs(c,l,r); mcs('c',l,r); }
    { int l,r; cout << mcs(d,l,r); mcs('d',l,r); }

    cout << "\nTest 3\n";
    { int l,r; cout << mcs<13>(a,l,r); mcs('a',l,r); }
    { int l,r; cout << mcs<10>(b,l,r); mcs('b',l,r); }
    { int l,r; cout << mcs<8>(c,l,r);  mcs('c',l,r); }
    { int l,r; cout << mcs<9>(d,l,r);  mcs('d',l,r); }

}

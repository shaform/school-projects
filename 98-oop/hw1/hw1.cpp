#include <iostream>
#include <limits>
#include <cstring>
using namespace std;

const int M = 16;
const int LEN = 8;
const int AP = 'z'-'a'+1;
const unsigned mask = 0xF;
const int nibble = 4;

template<typename T>
void radixsort(T* a,int n){
    T* b = new T[n];
    int c[M] = {0};

    int comp_pairs = numeric_limits<T>::digits/nibble;
    if (numeric_limits<T>::digits%nibble != 0)
        ++comp_pairs;

    for (int i=0; i<comp_pairs; ++i) {
        memset(c, 0, sizeof(c));
        for (int j=0; j<n; ++j)
            ++c[(a[j] >> (nibble*i)) & mask];
        for (int j=1; j<M; ++j)
            c[j] += c[j-1];

        for (int j=n-1; j>=0; --j)
            b[--c[(a[j] >> (nibble*i)) & mask]] = a[j];

        if (numeric_limits<T>::is_signed && i == comp_pairs-1) {
            for (int j=0; j<n; ++j)
                if (((b[j] >> (nibble*i)) & mask) >> 3) {
                    int k;
                    for (k=0; k<n-j; ++k)
                        a[k] = b[j+k];
                    for (; k<n; ++k)
                        a[k] = b[k-j];
                    break;
                }
        } else
            for (int j=0; j<n; ++j)
                a[j] = b[j];

    }
    delete [] b;
}

template<>
void radixsort<char*>(char** a,int n){
    char** b = new char*[n];
    int c[M] = {0};

    //Step one : length
    for (int i=0; i<2; ++i) {
        memset(c, 0, sizeof(c));
        for (int i=0; i<n; ++i)
            ++c[(strlen(a[i]) >> (nibble*i)) & mask];
        for (int i=1; i<M; ++i)
            c[i] += c[i-1];

        for (int i=n-1; i>=0; --i)
            b[--c[(strlen(a[i])  >> (nibble*i)) & mask]] = a[i];

        for (int j=0; j<n; ++j)
            a[j] = b[j];
    }

    int d[AP+1];

    //Step two:
    for (int i=LEN-1; i>=0; --i) {
        memset(d, 0, sizeof(d));
        for (int j=0; j<n; ++j)
            if (i >= strlen(a[j]))
                ++d[0];
            else
                ++d[a[j][i]-'a'+1];

        for (int j=1; j<=AP; ++j)
            d[j] += d[j-1];

        for (int j=n-1; j>=0; --j)
            b[--d[i >= strlen(a[j]) ? 0 : a[j][i] -'a'+1]] = a[j];

        for (int j=0; j<n; ++j)
            a[j] = b[j];

    }

    delete [] b;
}

int main()

{

    cout << "Test 1\n"; 

    unsigned short a[]={0x3579,0x3597,0x3245,0x3254,0x4235,0x4253,0x5379,0x5397,

        0x3759,0x3795,0x3425,0x3524,0x4325,0x4523,0x5739,0x5793,

        0x3957,0x3975,0x3452,0x3542,0x4352,0x4532,0x5937,0x5973};                           

    radixsort(a,24);

    for (int i=0;i<24;i++) cout << a[i] << ' '; cout << endl;



    cout << "\nTest 2\n";

    signed b[]={0xA345A345,0x23542354,0xA245A245,0x32543254,0xA235A235,0x42534253,0xA243A243,0x52345234,

        0xB435B435,0x24532453,0xB425B425,0x35243524,0xB325B325,0x45234523,0xB423B423,0x53245324,

        0xC543C543,0x25342534,0xC452C452,0x35423542,0xC352C352,0x45324532,0xC432C432,0x53425342};

    radixsort(b,24);

    for (int i=0;i<24;i++) cout << b[i] << ' '; cout << endl;



    cout << "\nTest 3\n";

    char* c[27]={"boggy","snoot","cat","dog","doggy","garage","snooper","plug","bogs",

        "egg","cafe","dig","snoopy","cats","pluto","snooty","dogs","smell",

        "zoo","smoke","bigger","snoop","bog","cab","garfield","small","big"};

    radixsort(c,27);

    for (int i=0;i<27;i++) cout << c[i] << ' '; cout << endl;

}

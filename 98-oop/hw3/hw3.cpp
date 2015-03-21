#include <iostream>
#include <functional>
#include <cstring>

using namespace std;

namespace std {
    template<>
        struct less<const char*> {
            bool operator() (const char *x, const char *y)
            {
                return strcmp(x, y) < 0;
            }
        };

    template<>
        struct greater<const char*> {
            bool operator() (const char *x, const char *y)
            {
                return strcmp(x, y) > 0;
            }
        };
};




    template<typename T>
void swapbp(T* first, T* second)
{
    T temp = *first;
    *first = *second;
    *second = temp;
}

    template<typename T,typename Compare>
void sort(T* first,T* last,Compare comp)
{
    while (first+1 < last) {
        T *i = first, *j = last-2;

        bool swapped;
        swapped = false;
        while (i+1 < last) {
            if (!comp(*i, *(i+1))) {
                swapbp(i, i+1);
                swapped = true;
            }
            ++i;
        }
        if (!swapped)
            break;

        while (first <= j-1) {
            if (!comp(*(j-1), *j))
                swapbp(j-1, j);
            --j;
        }
        ++first;
        --last;
    }

}

    template<typename T,typename Compare>
bool binary_search(const T* first, const T* last, const T& value, Compare comp) 
{
    if (first >= last)
        return false;

    T* it = first + (last-first)/2;

    if (comp(value,*it))
        return binary_search(first, it, value, comp);
    else if (comp(*it,value))
        return binary_search(it+1, last, value, comp);
    else
        return true;
}




int main()
{
    cout << boolalpha;
    cout << "Test 1\n";
    int a[12]={12,11,10,9,8,7,6,5,4,3,2,1};
    sort(a,a+12,less<int>()); 
    for (int i=0;i<12;i++) cout << a[i] << ' '; cout << endl;
    cout << binary_search(a,a+12,12,less<int>()) << ' ' << binary_search(a,a+12,0,less<int>()) << endl;

    cout << "\nTest 2\n";
    char b[20]="snoopyplutogarfield";
    sort(b,b+19,greater<char>());
    for (int i=0;i<19;i++) cout << b[i] << ' '; cout << endl;
    cout << binary_search(b,b+19,'y',greater<char>()) << ' ' << binary_search(b,b+19,'b',greater<char>()) << endl;

    cout << "\nTest 3\n";
    const char* c[11]={"Nationals","Indians","Cubs","Astros","Dodgers","Mets","Twins","Royals","Giants","Braves","Orioles"};
    sort(c,c+11,less<const char*>());
    for (int i=0;i<11;i++) cout << c[i] << ' '; cout << endl;
    cout << binary_search(c,c+11,(const char *)"Nationals",less<const char*>()) << ' ';
    cout << binary_search(c,c+11,(const char *)"Yankees",less<const char*>()) << endl;

    cout << "\nTest 4\n";
    sort(c,c+11,greater<const char*>());
    for (int i=0;i<11;i++) cout << c[i] << ' '; cout << endl;
    cout << binary_search(c,c+11,(const char *)"Nationals",greater<const char*>()) << ' ';
    cout << binary_search(c,c+11,(const char *)"Yankees",greater<const char*>()) << endl;
}

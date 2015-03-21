#include <iostream>
#include <string>
using namespace std;

template<typename T>
struct even {
    bool operator()(const T& n) const { return (n&1)==0; }
};

template<>
struct even<string> {
    bool operator() (const string& s) const { return (s.length()%2)==0; }
};

    template<typename ForwardIterator, typename Predicate>
ForwardIterator satisfy(ForwardIterator begin, ForwardIterator end, Predicate p)
{
    for (ForwardIterator i = begin; i!=end; ++i)
        if (p(*i) && i!=begin) *(begin++) = *i;
    return begin;
}



int main()
{
    cout << "Test 1 ...\n";
    cout << boolalpha << even<string>()("Snoopy") << ' ' << even<string>()("Pluto") << endl;
    cout << "\nTest 2 ...\n";
    int a[15]={1,2,3,4,5,6,7,8,7,6,5,4,3,2,1};
    int* a_end=satisfy(a,a+15,even<int>());
    for (int* it=a;it!=a_end;++it) cout << *it << ' ';
    cout << endl;
    cout << "\nTest 3 ...\n";
    string b[11]={"Nationals","Indians","Cubs","Astros","Dodgers","Mets","Twins","Royals","Giants","Braves","Orioles"};
    string* b_end=satisfy(b,b+11,even<string>());
    for (string* it=b;it!=b_end;++it) cout << *it << ' ';
    cout << endl;
}

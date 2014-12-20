#include <iostream>
#include "list7.h"
using namespace std;


template<class ForwardIterator>
ForwardIterator
unique(ForwardIterator begin, ForwardIterator end)
{
    ForwardIterator it=begin, nit=begin;
    while (++nit!=end)
        if (*nit != *it)
            *++it = *nit;

    return ++it;
}


int main()
{
    cout << "Part 1 ...\n";
    int x[16]={2,2,2,3,4,4,4,5,5,5,5,6,6,5,5,5};
    list<int> a(x,x+16);
    list<int>::iterator it;
    list<int>::const_iterator cit;

    for (it=a.begin();it!=a.end();++it) cout << *it; cout << endl;
    for (cit=a.begin();cit!=a.end();++cit) cout << *cit; cout << endl;

    cout << "\nPart 2 ...\n";
    list<int> b(a);
    a.unique();
    for (it=a.begin();it!=a.end();++it) cout << *it; cout << endl;
    list<int>::iterator logical_end=unique(b.begin(),b.end());
    for (it=b.begin();it!=logical_end;++it) cout << *it; cout << endl;
    for (it=b.begin();it!=b.end();++it) cout << *it; cout << endl;

    cout << "\nPart 3 ...\n";
    a.push_back(6);
    a.push_front(1);
    for (it=a.begin();it!=a.end();++it) cout << *it; cout << endl;
    cout << *a.erase(----a.end()) << endl;
    for (it=a.begin();it!=a.end();++it) cout << *it; cout << endl;
    cout << *a.insert(++++a.begin(),7) << endl;
    for (it=a.begin();it!=a.end();++it) cout << *it; cout << endl;
}

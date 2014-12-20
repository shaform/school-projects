#include <iostream> 
#include "listD.h" 
using namespace std;

ostream& operator<<(ostream& os,const list<int>& v)
{
    for (list<int>::const_iterator it=v.begin();it!=v.end();++it) os << *it; 
    return os << endl;
}

int main() 
{
    int x[]={6,5,6,7,8,8,3};
    int y[]={1,2,6,8,1,3,2,1,3};
    list<int> a(x,x+7),b(y,y+9);
    a-=b; cout << a;
    b-=a; cout << b;
    a-=b; cout << a;
    b-=a; cout << b;
    a-=a; cout << a;
}

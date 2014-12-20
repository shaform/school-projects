#include <iostream>
#include <string>
#include "stackC.h"
using namespace std;

class str {
    public:
        str(const char* s="") : s(s) { cout << s << " constructed" << endl; }
        str(const str& rhs) : s(rhs.s) { cout << s << " copy-constructed" << endl; }
        ~str() { cout << s << " destructed"<< endl; }
    private:
        string s;
};

int main()
{
    cout << "Test 1 ...\n";
    stack<char> s;
    cout << s.size() << endl;
    for (char c='a';c<='z';c++) s.push(c);
    cout << s.top() << ' ' << s.size() << endl;
    s.pop(); 
    cout << s.top() << ' ' << s.size() << endl;
    cout << "\nTest 2 ...\n";
    str a("Snoopy"),b("Pluto");
    cout << "=======================\n";
    {
        stack<str> t;
        t.push(a); t.push(b); t.pop();
    }
    cout << "=======================\n";
}

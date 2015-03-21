#include <iostream>
#include <string>
#include <deque>
#include <vector>
#include "stack4.h"

using namespace std;

    template<typename T>
int stackPerm(deque<T>& in,stack<T>& perm,vector<T>& out)
{
    static int n = 0;
    if (out.empty() && perm.empty())
        n = 0;

    if (in.empty() && perm.empty()) {
        ++n;

        typename vector<T>::size_type i = 0;
        if (i < out.size()) {
            cout << out[i];

            for (++i; i < out.size(); ++i) {
                cout << ' ' << out[i];
            }
            cout << endl;
        }
    } else {

        if (!perm.empty()) {
            typename stack<T>::value_type val = perm.top();

            perm.pop();
            out.push_back(val);

            stackPerm(in, perm, out);

            out.pop_back();
            perm.push(val);
        }

        if (!in.empty()) {
            typename deque<T>::value_type val = in.front();

            in.pop_front();
            perm.push(val);

            stackPerm(in, perm, out);

            perm.pop();
            in.push_front(val);
        }
    }
    return n;
}

int main()
{
    {
        cout << "Test 1 ...\n";
        deque<int> in;
        stack<int> perm;
        vector<int> out;
        for (int i=1;i<=3;i++) in.push_back(i);
        cout << "There are " << stackPerm(in,perm,out) << " stack permutations.\n\n";
    }
    {
        cout << "Test 2 ...\n";
        deque<char> in;
        stack<char> perm(3);
        vector<char> out;
        string s("nctu");
        for (int i=0;i<s.size();i++) in.push_back(s[i]);
        cout << "There are " << stackPerm(in,perm,out) << " stack permutations.\n\n";
    }
    {
        cout << "Test 3 ...\n";
        deque<string> in;
        stack<string> perm(1);
        vector<string> out;
        string s[5]={"ant","bee","cat","dog","elf"};
        for (int i=0;i<5;i++) in.push_back(s[i]);
        cout << "There are " << stackPerm(in,perm,out) << " stack permutations.\n\n";
    }
}   

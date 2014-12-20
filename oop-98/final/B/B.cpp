#include <iostream>
#include <string>
#include <vector>
#include <stack>

using namespace std;

struct state {
    string selected;
    int to_choose;
    state(const string &s, int n) : selected(s), to_choose(n) {}
};

int substring(const string& s)
{

    // res begin
    vector<string> buff;
    stack<state> sts;
    sts.push(state(string(), 0));

    while (!sts.empty()) {
        state st = sts.top();
        sts.pop();

        if (st.to_choose == s.length()) {
            buff.push_back(st.selected);

        } else {
            ++st.to_choose;
            st.selected.insert(st.selected.end(), s[st.to_choose-1]);
            sts.push(st);
            st.selected.erase(--st.selected.end());
            sts.push(st);
        }
    }
    // res end

    // remove dup & print
    int total = 0;
    for (vector<string>::iterator it=buff.begin(); it!=buff.end(); ++it) {
        bool dup = false;
        for (vector<string>::iterator cit=buff.begin(); cit != it; ++cit)
            if (*it==*cit) dup = true;

        if (!dup) {
            cout << *it << endl;
            ++total;
        }
    }

    return total;
}


int main()
{
    cout << "Test 1 ...\n";
    string a("cafe");
    cout << substring(a) << " substrings\n";
    cout << "\nTest 2 ...\n";
    string b("sees");
    cout << substring(b) << " substrings\n";
}

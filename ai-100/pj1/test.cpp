#include <cstdio>
#include <vector>

using namespace std;

void proj1(const char *, int, int, std::vector<int> *);

int main()
{
    vector<int> sol;
    printf("Easy input...\n");
    proj1("125034678", 0, 0, &sol);
    for (vector<int>::iterator it = sol.begin();
            it != sol.end(); ++it) {
        printf("step: %d\n", *it);
    }

    printf("Invalid input...\n");

    proj1("021345678", 0, 0, &sol);
    if (sol.empty())
        printf("check passed\n");
    else
        printf("check failed 021345678\n");

    proj1("011145678", 0, 0, &sol);
    if (sol.empty())
        printf("check passed\n");
    else
        printf("check failed 011145678\n");

    return 0;
}

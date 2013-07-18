#include "counting-sort.hpp"

#include <iostream>
#include <vector>
#include <random>
#include <functional>
#include <algorithm>

using namespace std;

typedef unsigned T;

int main(void)
{
    unsigned const k = 5;
    mt19937 engine;
    uniform_int_distribution<T> dist(0, k);
    auto generator = bind(dist, engine);
    vector<T> A = {2, 5, 3, 0, 2, 3, 0, 3};
    cout << " Input: ";
    for_each(begin(A), end(A), [](T const e){ cout << e << " "; });
    cout << "\n";
    /*
    for(unsigned i = 0; i < 10; ++i)
    {
        A.push_back(generator());
    }
    */
    vector<T> B(A.size());
    counting_sort(A.cbegin(), A.cend(), A.crbegin(), A.crend(), begin(B), k);
    cout << "Output: ";
    for_each(begin(B), end(B), [](T const e){ cout << e << " "; });
    cout << "\n";
    return 0;
}

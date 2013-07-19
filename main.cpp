#include "counting-sort.hpp"

#include <iostream>
#include <vector>
#include <random>
#include <functional>
#include <algorithm>
#include <chrono>
#include <string>

using namespace std;

typedef unsigned long T;

// #define STRINGS

int main(void)
{
#ifndef STRINGS
    T const k = 5, __min = 0;
    unsigned const n = 10000000;
    cout << "k = " << k << ", n = " << n << endl;
    mt19937 engine;
    uniform_int_distribution<T> dist(__min, k);
    auto generator = bind(dist, engine);
    vector<T> A;
    A.reserve(n);
    for(unsigned i = 0; i < n; ++i)
        A.push_back(generator());
    vector<T> B(A.size());
#else
    string A("something wicked this way comes");
    char const __min(*min_element(A.begin(), A.end())), k(*max_element(A.begin(), A.end()));
    cout << "Before: " << A << endl;
#endif
    auto const t0(std::chrono::high_resolution_clock::now());
    stable_counting_sort(A.cbegin(), A.cend(), A.crbegin(), A.crend(), begin(B), k);
    // counting_sort(A.begin(), A.end(), k, __min);
    // sort(begin(A), end(A));
    auto const t1(std::chrono::high_resolution_clock::now());
    auto elapsed(chrono::duration_cast<chrono::milliseconds>(t1 - t0).count());
    // cout << " After: " << A << endl;
    cout << "Elapsed time: " << elapsed << " ms.\n";
    
    return 0;
}

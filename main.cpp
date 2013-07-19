#include "counting-sort.hpp"

#include <iostream>
#include <vector>
#include <random>
#include <functional>
#include <algorithm>
#include <chrono>
#include <string>

using namespace std;

typedef unsigned short T;

int main(void)
{
    T const k = 15, __min = 0;
    unsigned const n = 10000000;
    cout << "k = " << k << ", n = " << n << endl;
    mt19937 engine;
    uniform_int_distribution<T> dist(__min, k);
    auto generator = bind(dist, engine);
    vector<T> A = { 11, 12, 5, 8, 3, 2, 0, 3, 6, 8, 13, 3, 3, 6, 5, 0 };
    /*
    A.reserve(n);
    for(unsigned i = 0; i < n; ++i)
        A.push_back(generator());
    */
    vector<T> B(A.size());

    auto const t0(std::chrono::high_resolution_clock::now());
    stable_counting_sort(A.cbegin(), A.cend(), A.crbegin(), A.crend(), begin(B), k, 0x03);
    // counting_sort(A.begin(), A.end(), k, __min);
    // sort(begin(A), end(A));
    auto const t1(std::chrono::high_resolution_clock::now());
    auto elapsed(chrono::duration_cast<chrono::milliseconds>(t1 - t0).count());
    // cout << " After: " << A << endl;
    cout << "Elapsed time: " << elapsed << " ms.\n";
    
    return 0;
}

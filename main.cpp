//  (C) Copyright Jeremy W. Murphy 2013.
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#include "boost/algorithm/sorting/integer_sort.hpp"
#include "boost/algorithm/sorting/string_sort.hpp"

#include "counting-sort.hpp"
#include "radix-sort.hpp"

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <vector>
#include <random>
#include <functional>
#include <algorithm>
#include <chrono>
#include <string>

using namespace std;
using namespace boost;

typedef unsigned T;

int main(int argc, char **argv)
{
    unsigned const n = lexical_cast<unsigned>(argv[1]);
    T const k = lexical_cast<T>(argv[2]), __min = 0;
    cout << "k = " << k << ", n = " << n << endl;
    mt19937 engine;
    uniform_int_distribution<T> dist(__min, k);
    auto generator = bind(dist, engine);
    cout << "Creating data vectors..." << endl;
    vector<T> A;
    A.reserve(n);
    for(unsigned i = 0; i < n; ++i)
        A.push_back(generator());
    vector<T> B(A);
    cout << "Sorting..." << endl;
    auto const t0(std::chrono::high_resolution_clock::now());
    stable_counting_sort(A.cbegin(), A.cend(), B.begin(), k);
    // radix_sort(A.cbegin(), A.cend(), B.begin(), k);
    // integer_sort(B.begin(), B.end());
    // counting_sort(B.begin(), B.end(), k);
    // sort(A.begin(), A.end());
    auto const t1(std::chrono::high_resolution_clock::now());
    auto elapsed(chrono::duration_cast<chrono::milliseconds>(t1 - t0).count());
    // cout << " After: " << A << endl;
    cout << "Elapsed time: " << elapsed << " ms.\n";
    
    // Test stable_counting_sort().
    cout << "Testing ... ";
    cout.flush();
    vector<T> X(A);
    sort(X.begin(), X.end());
    if(X == B)
        cout << "[OK]" << endl;
    else
        cout << "[FAILED]" << endl;

#ifndef NDEBUG
    cout << "==== A ====" << endl;
    for_each(std::begin(A), std::end(A), [](T const &__e){ cout << __e; });
    cout << endl << "==== B ====" << endl;
    for_each(std::begin(B), std::end(B), [](T const &__e){ cout << __e; });
    cout << endl << "==== X ====" << endl;
    for_each(std::begin(X), std::end(X), [](T const &__e){ cout << __e; });
    cout << endl;
#endif
    
    radix_sort(A.cbegin(), A.cend(), B.begin(), k);
    
    return 0;
}

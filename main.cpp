//  (C) Copyright Jeremy W. Murphy 2013.
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#include "boost/algorithm/sorting/integer_sort.hpp"
#include "boost/algorithm/sorting/string_sort.hpp"

#include "counting-sort.hpp"
#include "radix-sort.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/random.hpp>

#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>
#include <string>
#include <ctime>
#include <locale>
#include <iomanip>

using namespace std;
using namespace boost;

template <template <typename T> class Distribution, typename T>
void test(T const _k, T const _min = 0, unsigned const _max10 = 8, unsigned const _seed = 0)
{
    typedef typename vector<T>::const_iterator value_const_iterator;

    cout << "=== Tests (seed = " << _seed << "). ===" << endl;
    random::mt19937 rng(_seed);

    for(int _p = 1; _p <= _max10; _p++)
    {
        std::size_t const _n(pow10(_p));
        
        Distribution<T> const dist(_min, _k);
        cout << "Creating data vectors, n = " << _n << ", k = " << _k << "..." << endl;
        vector<T> A;
        A.reserve(_n);
        T _actual_min = numeric_limits<T>::max();
        for(unsigned i = 0; i < _n; ++i)
        {
            A.push_back(dist(rng));
            if(A.back() < _actual_min)
                _actual_min = A.back();
        }
        vector<T> B(A);

        cout << "Sorting (min = " << _actual_min << ") ..." << endl;
        struct timespec t0, t1;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t0);
        stable_radix_sort(A.begin(), A.end(), B.begin(), _k, _min);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
        
        time_t _seconds = (t1.tv_sec - t0.tv_sec > 0 ? t1.tv_sec - t0.tv_sec - 1 : 0);
        long double _elapsed = _seconds + abs(t1.tv_nsec - t0.tv_nsec) / 10000000000.0;
        
        cout << "Time taken : " << _elapsed << " s" << endl;
        
        vector<T> X(A);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t0);
        stable_sort(X.begin(), X.end());
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);

        _seconds = (t1.tv_sec - t0.tv_sec > 0 ? t1.tv_sec - t0.tv_sec - 1 : 0);
        _elapsed = _seconds + abs(t1.tv_nsec - t0.tv_nsec) / 10000000000.0;

        cout << "std::sort(): " << _elapsed << " s" << " ... ";

        if(X == B)
            cout << "[OK]" << endl;
        else
        {
            cout << "[FAILED]" << endl;
#ifndef NDEBUG // Print the results in debug mode.
            cout << "==== A ====" << endl;
            for(value_const_iterator _it(A.begin()); _it != A.end(); _it++) { cout << *_it << " "; };
            cout << endl << "==== B ====" << endl;
            for(value_const_iterator _it(B.begin()); _it != B.end(); _it++) { cout << *_it << " "; };
            cout << endl << "==== X ====" << endl;
            for(value_const_iterator _it(X.begin()); _it != X.end(); _it++) { cout << *_it << " "; };
            cout << endl;
#endif
        }
    }
}


int main(int argc, char **argv)
{
    // Configure cout to my liking.
    std::ios_base::sync_with_stdio(false);
    std::locale::global(std::locale(""));
    std::cout.imbue(std::locale());
    
    struct timespec tp;
    clock_getres(CLOCK_PROCESS_CPUTIME_ID, &tp);
    
    // Say something about the clock res?
    /*
    vector<T> _k;
    for(unsigned _i = 0; _i < log2(sizeof(T)); ++_i)
        _k.push_back((T(1) << 8 * (1 << _i)) - T(1));
    _k.push_back(numeric_limits<T>::max()); // Why doesn't this value work in the above formula?
    */
    
    // test<unsigned long>(argc < 2 ? time(NULL) : lexical_cast<unsigned>(argv[1]));
    test<random::uniform_int_distribution>(numeric_limits<unsigned char>::max());
    
    return 0;
}

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

typedef unsigned T; // TODO: Use MPL to test all the types: unsigned char, unsigned short, unsigned, unsigned long.

void test(unsigned const _seed)
{
    typedef typename vector<T>::const_iterator value_const_iterator;
    typedef typename vector<std::size_t>::const_iterator size_t_const_iterator;

    cout << "=== Tests (seed = " << _seed << "). ===" << endl;
    
    /*********************************************************************************************
     * Test a matrix of n and k values ...
     *********************************************************************************************/

    // TODO: Yes, I know this is a mess.
    // TODO: mpl::for_each unsigned type...
    vector<std::size_t> _k;
    _k.push_back((1ul << 8) - 1);
    if(numeric_limits<T>::max() >= (1ul << 16) - 1)
        _k.push_back((1ul << 16) - 1);
    if(numeric_limits<T>::max() >= (1ul << 32) - 1)
        _k.push_back((1ul << 32) - 1);
    if(numeric_limits<T>::max() >= (1ul << 64) - 1)
        _k.push_back((1ul << 64) - 1);
    
    random::mt19937 rng(_seed);

    for(int _p = 1; _p <= 8; _p++) // 10^n.  I think any more than 8 will be trouble.
    {
        std::size_t const _n(pow10(_p));
        
        for(size_t_const_iterator _j(_k.begin()); _j != _k.end(); _j++)
        {
            random::uniform_int_distribution<T> const dist(0, *_j);
            cout << "Creating data vectors, n = " << _n << ", k = " << *_j << "..." << endl;
            vector<T> A;
            A.reserve(_n);
            T _min = numeric_limits<T>::max();
            for(unsigned i = 0; i < _n; ++i)
            {
                A.push_back(dist(rng));
                if(A.back() < _min)
                    _min = A.back();
            }
            vector<T> B(A);

            cout << "Sorting (min = " << _min << ") ..." << endl;
            struct timespec t0, t1;
            clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t0);
            stable_radix_sort(A.begin(), A.end(), B.begin(), *_j, _min);
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

    test(argc < 2 ? time(NULL) : lexical_cast<unsigned>(argv[1]));
    
    return 0;
}

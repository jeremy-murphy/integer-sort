//  (C) Copyright Jeremy W. Murphy 2013.
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#include "boost/algorithm/sorting/integer_sort.hpp"
#include "boost/algorithm/sorting/string_sort.hpp"

#include "counting-sort.hpp"
#include "radix-sort.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/mpl/vector.hpp>

#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <ctime>
#include <locale>
#include <iomanip>

using namespace std;
using namespace boost;

typedef unsigned T; // TODO: Use MPL to test all the types: unsigned char, unsigned short, unsigned, unsigned long.

void test(unsigned const __seed)
{
    typedef typename vector<T>::const_iterator value_const_iterator;
    typedef typename vector<std::size_t>::const_iterator size_t_const_iterator;

    cout << "=== Tests (seed = " << __seed << "). ===" << endl;
    srand(__seed);
    
    /*********************************************************************************************
     * Test a matrix of n and k values ...
     *********************************************************************************************/

    // TODO: Yes, I know this is a mess.
    // mpl::for_each unsigned type...
    vector<std::size_t> __k;
    __k.push_back((1ul << 8) - 1);
    if(numeric_limits<T>::max() >= (1ul << 16) - 1)
        __k.push_back((1ul << 16) - 1);
    if(numeric_limits<T>::max() >= (1ul << 32) - 1)
        __k.push_back((1ul << 32) - 1);
    if(numeric_limits<T>::max() >= (1ul << 64) - 1)
        __k.push_back((1ul << 64) - 1);
    
    for(int __p = 1; __p <= 8; __p++) // 10^n.  I think any more than 8 will be trouble.
    {
        std::size_t const __n(pow10(__p));
        
        for(size_t_const_iterator __j(__k.begin()); __j != __k.end(); __j++)
        {
            cout << "Creating data vectors, n = " << __n << ", k = " << *__j << "..." << endl;
            vector<T> A;
            A.reserve(__n);
            for(unsigned i = 0; i < __n; ++i)
                A.push_back(rand() % *__j);
            vector<T> B(A);

            cout << "Sorting..." << endl;
            struct timespec t0, t1;
            clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t0);
            // stable_counting_sort(A.begin(), A.end(), B.begin(), *__j);
            stable_radix_sort(A.begin(), A.end(), B.begin(), *__j);
            clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
            
            time_t __seconds = (t1.tv_sec - t0.tv_sec > 0 ? t1.tv_sec - t0.tv_sec - 1 : 0);
            long double __elapsed = __seconds + abs(t1.tv_nsec - t0.tv_nsec) / 10000000000.0;
            
            cout << "Time taken : " << __elapsed << " s" << endl;
            
            vector<T> X(A);
            clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t0);
            sort(X.begin(), X.end());
            clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);

            __seconds = (t1.tv_sec - t0.tv_sec > 0 ? t1.tv_sec - t0.tv_sec - 1 : 0);
            __elapsed = __seconds + abs(t1.tv_nsec - t0.tv_nsec) / 10000000000.0;

            cout << "std::sort(): " << __elapsed << " s" << " ... ";

            if(X == B)
                cout << "[OK]" << endl;
            else
            {
                cout << "[FAILED]" << endl;
#ifndef NDEBUG // Print the results in debug mode.
                cout << "==== A ====" << endl;
                for(value_const_iterator __it(A.begin()); __it != A.end(); __it++) { cout << *__it << " "; };
                cout << endl << "==== B ====" << endl;
                for(value_const_iterator __it(B.begin()); __it != B.end(); __it++) { cout << *__it << " "; };
                cout << endl << "==== X ====" << endl;
                for(value_const_iterator __it(X.begin()); __it != X.end(); __it++) { cout << *__it << " "; };
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

    test(argc < 2 ? time(NULL) : boost::lexical_cast<unsigned>(argv[1]));
    
    return 0;
}

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
#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/test_tools.hpp>

#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>
#include <string>
#include <locale>
#include <iomanip>
#include <typeinfo>

using namespace std;
using namespace boost;

template <typename T, class Distribution>
void test(Distribution dist, unsigned const _seed = 0, unsigned const _max10 = 6)
{
    // typedef typename std::vector<T>::iterator iterator;
    typedef typename std::vector<T>::const_iterator const_iterator;

    cout << "=== Test (seed = " << _seed << ", T = " << typeid(T).name() << "). ===" << endl;
    random::mt19937 rng(_seed);

    for(int _p = 1; _p <= _max10; _p++)
    {
        std::size_t const _n(pow10(_p));
        vector<T> A;
        A.reserve(_n);
        for(unsigned i = 0; i < _n; ++i)
            A.push_back(dist(rng));
        vector<T> B(A);
        const_iterator const    _min = std::min_element(A.begin(), A.end()), 
                                _max = std::max_element(A.begin(), A.end());
        cout << "n = " << _n << ", min = " << *_min << ", k = " << *_max << " ..." << endl;
        stable_radix_sort(A.begin(), A.end(), B.begin(), *_max, *_min);
        
        vector<T> X(A);
        stable_sort(X.begin(), X.end());

        BOOST_CHECK(X == B);
    }
}


int test_main(int argc, char **argv)
{
    // Configure cout to my liking.
    std::ios_base::sync_with_stdio(false);
    std::locale::global(std::locale(""));
    std::cout.imbue(std::locale());
    
    unsigned _seed(argc < 2 ? time(NULL) : lexical_cast<unsigned>(argv[1]));
    
    cout << "Testing with a poisson_distribution, mean = numeric_limits<T>::max() / 2" << endl;
    test<unsigned char>(random::poisson_distribution<unsigned char>(numeric_limits<unsigned char>::max() / 2), _seed);
    test<unsigned short>(random::poisson_distribution<unsigned short>(numeric_limits<unsigned short>::max() / 2), _seed);
    test<unsigned int>(random::poisson_distribution<unsigned int>(numeric_limits<unsigned int>::max() / 2), _seed);
    test<unsigned long>(random::poisson_distribution<unsigned long>(numeric_limits<unsigned long>::max() / 2), _seed);

    cout << "Testing with a uniform_int_distribution, min = 0, k = numeric_limits<T>::max()" << endl;
    test<unsigned char>(random::uniform_int_distribution<unsigned char>(0, numeric_limits<unsigned char>::max()), _seed);
    test<unsigned short>(random::uniform_int_distribution<unsigned short>(0, numeric_limits<unsigned short>::max()), _seed);
    test<unsigned int>(random::uniform_int_distribution<unsigned int>(0, numeric_limits<unsigned int>::max()), _seed);
    test<unsigned long>(random::uniform_int_distribution<unsigned long>(0, numeric_limits<unsigned long>::max()), _seed);
    
    return 0;
}

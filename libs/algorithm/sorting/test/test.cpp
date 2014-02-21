//  (C) Copyright Jeremy W. Murphy 2013.
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/algorithm/integer_sort.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/random.hpp>
#include <boost/timer/timer.hpp>
#include <boost/bind.hpp>
#include <boost/test/test_tools.hpp>

#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>
#include <string>
#include <locale>
#include <iomanip>
#include <typeinfo>
#include <cmath>

using boost::lexical_cast;
using namespace boost::random;
using namespace boost::algorithm;

// TODO: Finish this fixture and integrate it with proper fixture test cases.
struct foo
{
    foo()
    {
        BOOST_TEST_MESSAGE( "setup fixture" );
    }
    
    ~foo()
    {
        BOOST_TEST_MESSAGE( "teardown fixture" );
    }
};


template <typename T, class Distribution>
void test(Distribution dist, unsigned const seed = 0, unsigned const max10 = 7)
{
    // typedef typename std::vector<T>::iterator iterator;
    typedef typename std::vector<T>::const_iterator const_iterator;

    std::cout << "=== Test (seed = " << seed << ", T = " << typeid(T).name() << "). ===" << std::endl;
    mt19937 rng(seed);
    

    for(unsigned p = 1; p <= max10; p++)
    {
        std::size_t const n(round(pow10(p)));
        std::vector<T> A;
        A.reserve(n);
        std::generate_n(std::back_inserter(A), n, boost::bind(dist, rng));
        std::vector<T> B(A);
        const_iterator const    _min = std::min_element(A.begin(), A.end()), 
                                _max = std::max_element(A.begin(), A.end());
        std::cout << "n = " << n << ", min = " << (*_min) * 1u << ", k = " << (*_max) * 1u << " ...";
        std::cout.flush();
        std::vector<T> X(A);
        boost::timer::cpu_timer timer;
        stable_radix_sort(A.begin(), A.end(), B.begin(), *_max, *_min);
        boost::timer::cpu_times t1 = timer.elapsed();
        std::stable_sort(X.begin(), X.end());
        boost::timer::cpu_times t2 = timer.elapsed();
        timer.stop();
        if(t1.user + t1.system > 0 && t2.user + t2.system - t1.user - t1.system > 0)
            std::cout << " " << static_cast<double>(t1.user + t1.system) / (static_cast<double>(t2.user + t2.system) - static_cast<double>(t1.user + t1.system));
        std::cout << std::endl;
        BOOST_CHECK(X == B);
    }
}


int test_main(int argc, char **argv)
{
    // Configure cout to my liking.
    std::ios_base::sync_with_stdio(false);
    std::locale::global(std::locale(""));
    std::cout.imbue(std::locale());
    
    unsigned seed(argc < 2 ? std::time(NULL) : lexical_cast<unsigned>(argv[1]));
    
    std::cout << "Testing with a poisson_distribution, mean = numeric_limits<T>::max() / 2" << std::endl;
    test<unsigned char>(poisson_distribution<unsigned char>(std::numeric_limits<unsigned char>::max() / 2), seed);
    test<unsigned short>(poisson_distribution<unsigned short>(std::numeric_limits<unsigned short>::max() / 2), seed);
    test<unsigned int>(poisson_distribution<unsigned int>(std::numeric_limits<unsigned int>::max() / 2), seed);
    test<unsigned long>(poisson_distribution<unsigned long>(std::numeric_limits<unsigned long>::max() / 2), seed);

    std::cout << "Testing with a uniform_int_distribution, min = 0, k = numeric_limits<T>::max()" << std::endl;
    test<unsigned char>(uniform_int_distribution<unsigned char>(0, std::numeric_limits<unsigned char>::max()), seed);
    test<unsigned short>(uniform_int_distribution<unsigned short>(0, std::numeric_limits<unsigned short>::max()), seed);
    test<unsigned int>(uniform_int_distribution<unsigned int>(0, std::numeric_limits<unsigned int>::max()), seed);
    test<unsigned long>(uniform_int_distribution<unsigned long>(0, std::numeric_limits<unsigned long>::max()), seed);
    
    return 0;
}

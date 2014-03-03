#include <iterator>
#include <vector>
#include <list>

#include <boost/algorithm/integer_sort/counting-sort.hpp>
#include <boost/test/test_tools.hpp>
#include <boost/random.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost::algorithm;

template <typename Input>
void counting_things(Input first, Input last)
{
    typedef typename std::iterator_traits<Input>::value_type value_type;
    std::vector<value_type> tmp(first, last);
    counting_sort(first, last);
    std::sort(tmp.begin(), tmp.end());
    std::vector<value_type> const result(first, last);
    BOOST_CHECK(tmp == result);
}


int test_main(int argc, char **argv)
{
    unsigned seed(argc < 2 ? std::time(NULL) : boost::lexical_cast<unsigned>(argv[1]));
    boost::mt19937 rng(seed);
    std::list<unsigned> input;
    for(unsigned i = 0; i < 1000; ++i)
        input.push_back(rng());
    counting_things(input.begin(), input.end());
    return 0;
}

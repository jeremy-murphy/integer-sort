
#define BOOST_TEST_MODULE counting_sanity
#include <boost/test/unit_test.hpp>

#include <iterator>
#include <vector>
#include <list>
#include <deque>

#include <boost/algorithm/integer_sort/counting-sort.hpp>
#include <boost/test/test_tools.hpp>
#include <boost/random.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/mpl/list.hpp>

using namespace boost::algorithm;

typedef boost::mpl::list<unsigned char, unsigned short, unsigned int, unsigned long> all_unsigned_types;
typedef boost::mpl::list<unsigned char, unsigned short> small_unsigned_types;

BOOST_AUTO_TEST_CASE_TEMPLATE(empty, T, all_unsigned_types)
{
    // So empty, it would be a crime to dereference it.
    T *p = NULL;
    counting_sort(p, p);
    // No BOOST_REQUIRE... we just make sure the program does not crash/throw.
}


/* n == 1 is a special case; fundamentally, there is no problem, the data is 
 * already in order.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE(n_equals_one, T, all_unsigned_types)
{
    std::list<T> tmp_list;
    tmp_list.push_back(0);
    counting_sort(tmp_list.begin(), tmp_list.end());
    BOOST_CHECK_EQUAL(tmp_list.back(), 0);

    std::vector<T> tmp_vector;
    tmp_vector.push_back(0);
    counting_sort(tmp_vector.begin(), tmp_vector.end());
    BOOST_CHECK_EQUAL(tmp_vector.back(), 0);

    std::deque<T> tmp_deque;
    tmp_deque.push_back(0);
    counting_sort(tmp_deque.begin(), tmp_deque.end());
    BOOST_CHECK_EQUAL(tmp_deque.back(), 0);
}


BOOST_AUTO_TEST_CASE_TEMPLATE(one_1000, T, all_unsigned_types)
{
    unsigned int const n = 1000;
    
    std::list<T> tmp_list(n, 1);
    counting_sort(tmp_list.begin(), tmp_list.end());
    std::vector<T> tmp_vector(n, 1);
    BOOST_CHECK(std::equal(tmp_list.begin(), tmp_list.end(), tmp_vector.begin()));
    
    counting_sort(tmp_vector.begin(), tmp_vector.end());
    std::deque<T> tmp_deque(n, 1);
    BOOST_CHECK(std::equal(tmp_deque.begin(), tmp_deque.end(), tmp_vector.begin()));
    
    counting_sort(tmp_deque.begin(), tmp_deque.end());
    BOOST_CHECK(std::equal(tmp_list.begin(), tmp_list.end(), tmp_vector.begin()));
}


BOOST_AUTO_TEST_CASE_TEMPLATE(random_small_k, T, all_unsigned_types)
{
    unsigned const k = 255;
    boost::random::mt19937 gen;
    boost::random::uniform_int_distribution<T> dist(0, k);
    std::vector<T> tmp_data;
    for(unsigned i = 0; i < 10; i++)
        tmp_data.push_back(dist(gen));
    std::vector<T> tmp_vector(tmp_data.begin(), tmp_data.end());
    std::list<T> tmp_list(tmp_data.begin(), tmp_data.end());
    std::deque<T> tmp_deque(tmp_data.begin(), tmp_data.end());
    std::sort(tmp_data.begin(), tmp_data.end());
    counting_sort(tmp_vector.begin(), tmp_vector.end());
    BOOST_CHECK(std::equal(tmp_data.begin(), tmp_data.end(), tmp_vector.begin()));
    counting_sort(tmp_list.begin(), tmp_list.end());
    BOOST_CHECK(std::equal(tmp_data.begin(), tmp_data.end(), tmp_list.begin()));
    counting_sort(tmp_deque.begin(), tmp_deque.end());
    BOOST_CHECK(std::equal(tmp_data.begin(), tmp_data.end(), tmp_deque.begin()));
}

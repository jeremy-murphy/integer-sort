
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
#include <boost/mpl/transform.hpp>
#include <boost/mpl/apply_wrap.hpp>
#include <boost/mpl/for_each.hpp>

using namespace boost::algorithm;

typedef boost::mpl::list<unsigned char, unsigned short, unsigned int, unsigned long> all_unsigned_types;
typedef boost::mpl::list<unsigned char, unsigned short> small_unsigned_types;


BOOST_AUTO_TEST_CASE_TEMPLATE(empty, T, all_unsigned_types)
{
    // So empty, it would be a crime to dereference it.
    T *const p = NULL, * const result = NULL;
    stable_counting_sort(p, p, result);
    // No BOOST_REQUIRE... we just make sure the program does not crash/throw.
}


template <typename T>
struct count_one
{
    std::vector<T> result;
    
    count_one()
    {
        result.push_back(9);
    }
    
    template <typename Container>
    void operator()(Container c)
    {
        c.push_back(0);
        stable_counting_sort(c.begin(), c.end(), result.begin());
        BOOST_CHECK_EQUAL(result.back(), 0);
        result[0] = 9;
    }
};


/* n == 1 is a special case; fundamentally, there is no problem, the data is 
 * already in order.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE(n_equals_one, T, all_unsigned_types)
{
    typedef typename boost::mpl::transform<boost::mpl::list< std::vector<boost::mpl::_1>, std::deque<boost::mpl::_1>, std::list<boost::mpl::_1> >, boost::mpl::apply1<boost::mpl::_1, T> >::type sequence_containers;
    boost::mpl::for_each<sequence_containers>(count_one<T>());
}


template <typename T>
class one_1000_foo
{
private:
    std::vector<T> input;
    std::vector<T> result;
    
public:
    one_1000_foo(std::vector<T> const &input) : input(input)
    {
        result.resize(input.size());
    }
    
    template <typename Container>
    void operator()(Container)
    {
        std::fill(result.begin(), result.end(), 9);
        Container const c(input.begin(), input.end());
        stable_counting_sort(c.begin(), c.end(), result.begin());
        BOOST_CHECK(std::equal(c.begin(), c.end(), result.begin()));
    }
};


BOOST_AUTO_TEST_CASE_TEMPLATE(one_1000, T, all_unsigned_types)
{
    typedef typename boost::mpl::transform<boost::mpl::list< std::vector<boost::mpl::_1>, std::deque<boost::mpl::_1>, std::list<boost::mpl::_1> >, boost::mpl::apply1<boost::mpl::_1, T> >::type test_containers;

    unsigned int const n = 1000;
    std::vector<T> const input(n, 1);
    
    boost::mpl::for_each<test_containers>(one_1000_foo<T>(input));
}


/*
BOOST_AUTO_TEST_CASE_TEMPLATE(random_small_k, T, all_unsigned_types)
{
    unsigned const k = 255, n = 1000;
    boost::random::mt19937 gen(0);
    boost::random::uniform_int_distribution<T> dist(0, k);
    std::vector<T> tmp_data;
    for(unsigned i = 0; i < n; i++)
        tmp_data.push_back(dist(gen));
    std::vector<T> tmp_vector(tmp_data.begin(), tmp_data.end());
    std::list<T> tmp_list(tmp_data.begin(), tmp_data.end());
    std::deque<T> tmp_deque(tmp_data.begin(), tmp_data.end());
    std::stable_sort(tmp_data.begin(), tmp_data.end());
    std::vector<T> result(n);
    stable_counting_sort(tmp_vector.begin(), tmp_vector.end(), result.begin(), no_op<T>());
    BOOST_CHECK(std::equal(tmp_data.begin(), tmp_data.end(), tmp_vector.begin()));
    std::fill_n(result.begin(), n, 0);
    stable_counting_sort(tmp_list.begin(), tmp_list.end(), result.begin());
    BOOST_CHECK(std::equal(tmp_data.begin(), tmp_data.end(), tmp_list.begin()));
    std::fill_n(result.begin(), n, 0);
    stable_counting_sort(tmp_deque.begin(), tmp_deque.end(), result.begin());
    BOOST_CHECK(std::equal(tmp_data.begin(), tmp_data.end(), tmp_deque.begin()));
}
*/

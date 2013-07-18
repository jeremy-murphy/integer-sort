#include <iterator>
#include <algorithm>

/**
 * Requires that client allocates space for result beforehand.
 */

template <typename ForwardIterator, typename ReverseIterator, typename OutputIterator>
void counting_sort(ForwardIterator __first, ForwardIterator __last, ReverseIterator __rfirst, ReverseIterator __rlast, OutputIterator __result, unsigned const __k = 9)
{
    typedef typename std::iterator_traits<ForwardIterator>::value_type value_type;
    
    std::vector<value_type> C(__k + 1);
    std::for_each(__first, __last, [&](value_type const e){ ++C[e]; });

    /*
    auto const __e(std::end(C));
    for(auto __it(std::begin(C) + 1); __it != __e; ++__it)
    {
        *__it += *(__it - 1);
    }
    */
    std::transform(std::begin(C) + 1, std::end(C), std::begin(C), std::begin(C) + 1, [](value_type const input1, value_type const input2){ return input1 + input2; });
    std::for_each(__rfirst, __rlast, [&](value_type const e)
    {
        *(__result + --C[e]) = e;
    });
}

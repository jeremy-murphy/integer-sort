#include <iterator>
#include <algorithm>
#include <cassert>
#include <limits>

#include <boost/static_assert.hpp>

/**
 * Requires that client allocates space for result beforehand.
 */

template <typename ForwardIterator, typename ReverseIterator, typename OutputIterator>
void stable_counting_sort(ForwardIterator __first, ForwardIterator __last, 
                          ReverseIterator __rfirst, ReverseIterator __rlast, OutputIterator __result, 
                          typename std::iterator_traits<ForwardIterator>::value_type const __k = 15, 
                          typename std::iterator_traits<ForwardIterator>::value_type const bitmask = std::numeric_limits<typename std::iterator_traits<ForwardIterator>::value_type>::max())
{
    // TODO: Statically check the iterators for type sanity.
    typedef typename std::iterator_traits<ForwardIterator>::value_type value_type;
    BOOST_STATIC_ASSERT(!std::numeric_limits<value_type>::is_signed);

    assert(__first != __last);
    assert(__rfirst != __rlast);
    
    std::vector<value_type> C(__k + 1);
    std::for_each(__first, __last, [&](value_type const e){ ++C[e & bitmask]; });
    std::transform(std::begin(C) + 1, std::end(C), std::begin(C), std::begin(C) + 1, [](value_type const input1, value_type const input2){ return input1 + input2; });
    std::for_each(__rfirst, __rlast, [&](value_type const e)
    {
        *(__result + --C[e & bitmask]) = e;
    });
}


#include <cstring>
#include <climits>


template <typename T>
void counting_sort( T* const arr, std::size_t const len, T const __max, T const __min = 0 )
{
    T z = 0;
    T nlen = ( __max - __min ) + 1;
    T temp[nlen];
    memset( temp, 0, nlen * sizeof( T ) );
    
    for( std::size_t i = 0; i < len; i++ )
        temp[arr[i] - __min]++;
    
    for( T i = __min; i <= __max; i++ )
        while( temp[i - __min] )
        {
            arr[z++] = i;
            temp[i - __min]--;
        }
}

/**
 * In-place but not stable.
 */
template <typename InputIterator>
void counting_sort( InputIterator __first, InputIterator __last,
                typename std::iterator_traits<InputIterator>::value_type const __max,
                typename std::iterator_traits<InputIterator>::value_type const __min = 0) 
{
    typedef typename std::iterator_traits<InputIterator>::value_type value_type;
    
    assert(__first != __last);
    assert(__max > __min);
    
    value_type const nlen = ( __max - __min ) + 1;
    value_type temp[nlen];
    std::fill_n((value_type*)(temp), nlen, value_type());
    std::for_each(__first, __last, [&](value_type const e){ assert(__min <= e && e <= __max); temp[e - __min]++; });
    
    auto it(__first);
    for( value_type i = __min; i <= __max; i++ )
        while( temp[i - __min]-- )
        {
            *(it++) = i;
        }
}

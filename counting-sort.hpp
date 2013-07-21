#ifndef COUNTING_SORT
#define COUNTING_SORT

#include <iterator>
#include <algorithm>
#include <cassert>
#include <limits>

#include <boost/static_assert.hpp>

/**
 * Requires that client allocates space for result beforehand.
 */

namespace boost
{
    template <typename ForwardIterator, typename ReverseIterator, typename OutputIterator>
    void stable_counting_sort(ForwardIterator __first, ForwardIterator __last, 
                            ReverseIterator __rfirst, ReverseIterator __rlast, OutputIterator __result, 
                            typename std::iterator_traits<ForwardIterator>::value_type const __k,
                            typename std::iterator_traits<ForwardIterator>::value_type const __bitmask = std::numeric_limits<typename std::iterator_traits<ForwardIterator>::value_type>::max())
    {
        // TODO: Statically check the iterators for type sanity.
        typedef typename std::iterator_traits<ForwardIterator>::value_type value_type;
        BOOST_STATIC_ASSERT(!std::numeric_limits<value_type>::is_signed);

        assert(__first != __last);
        assert(__rfirst != __rlast);
        

        // Really, k = 2^(width of 1s in bitmask)
        std::vector<uintmax_t> __C(__k + 1); // NOTE: Could be a std::dynarray in C++14?
#ifdef __GXX_EXPERIMENTAL_CXX0X__
        std::for_each(__first, __last, [&](value_type const __e){ ++__C[__e & __bitmask]; });
        std::transform(std::begin(__C) + 1, std::end(__C), std::begin(__C), std::begin(__C) + 1, [](uintmax_t const __input1, uintmax_t const __input2){ return __input1 + __input2; });
        std::for_each(__rfirst, __rlast, [&](value_type const __e)
        {
            *(__result + --__C[__e & __bitmask]) = __e;
        });
#else
        class sum_inputs
        {
        public:
            value_type operator()(value_type const &input1, value_type const &input2) const { return input1 + input2; }
        };
        
        
        for(ForwardIterator __it(__first); __it != __last; __it++)
        {
            __C[__e & __bitmask]++;
        }
        
        std::transform(__C.begin() + 1, __C.end(), __C.begin(), __C.begin() + 1, sum_inputs());
        
        for(ReverseIterator __it(__rfirst); __it != __rlast; __it++)
        {
            *(__result + --__C[__e & __bitmask]) = __e;
        }
#endif
    }


    #include <cstring>
    #include <climits>


    
    /**
    * In-place but not stable.
    * Based on code from Rosetta Code downloaded July 2013.
    */
    template <typename T>
    void counting_sort( T* const arr, std::size_t const len, T const __max, T const __min = 0 )
    {
        unsigned long z = 0;
        unsigned long nlen = ( __max - __min ) + 1;
        unsigned long temp[nlen];
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

    
    template <typename InputIterator>
    void counting_sort( InputIterator __first, InputIterator __last,
                    typename std::iterator_traits<InputIterator>::value_type const __max,
                    typename std::iterator_traits<InputIterator>::value_type const __min = 0) 
    {
        typedef typename std::iterator_traits<InputIterator>::value_type value_type;
        
        assert(__first != __last);
        assert(__max > __min);
        
        uintmax_t const nlen = ( __max - __min ) + 1;
        assert(nlen != 0);
        uintmax_t temp[nlen];
        std::fill_n((uintmax_t*)(temp), nlen, uintmax_t());
#ifdef __GXX_EXPERIMENTAL_CXX0X__
        std::for_each(__first, __last, [&](value_type const e){ assert(__min <= e && e <= __max); temp[e - __min]++; });
#else
        for(InputIterator __it(__first); __it != __last; __it++)
        {
            assert(__min <= *__it && *__it <= __max);
            temp[*__it - __min]++;
        }
#endif
        InputIterator it(__first);
        for( value_type i = __min; i <= __max; i++ )
            while( temp[i - __min]-- )
            {
                *(it++) = i;
            }
    }
}
#endif

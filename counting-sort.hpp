#include <iterator>
#include <algorithm>
#include <cassert>

template <typename InputIterator>
void fastcsort( InputIterator __first, InputIterator __last,
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


#include <cstring>
#include <climits>


template <typename T>
void fastcsort( T* const arr, std::size_t const len, T const __max, T const __min = 0 )
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

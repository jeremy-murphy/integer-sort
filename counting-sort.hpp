//  (C) Copyright Jeremy W. Murphy 2013.
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef COUNTING_SORT
#define COUNTING_SORT

#include <iterator>
#include <algorithm>
#include <cassert>
#include <limits>
#include <functional>

#include <boost/static_assert.hpp>


namespace boost
{
    /**
    * Requires that client allocates space for result beforehand.
    * 
    * @brief Generalized stable counting-sort.
    * 
    * \c BidirectionalIterator Type of input iterator.
    * \c OutputIterator Type of output iterator: only required to be forward.
    * 
    * \c _first Input iterator that points to the first element of the unsorted data.
    * \c _last Input iterator that points past the last element of the unsorted data.
    * \c _result Output iterator that points to the first element where the sorted data will go.
    * \c _k The largest value present in the input >> _r * _d.
    * \c _r The radix or width of digit to consider.
    * \c _d Which digit to consider.
    */
    template <typename BidirectionalIterator, typename OutputIterator>
    void stable_counting_sort(BidirectionalIterator _first, BidirectionalIterator _last, OutputIterator _result, 
                            typename std::iterator_traits<BidirectionalIterator>::value_type const _k,
                            typename std::iterator_traits<BidirectionalIterator>::value_type const _min = 0,
                            unsigned const _r = sizeof(typename std::iterator_traits<BidirectionalIterator>::value_type) * 8,
                            unsigned const _d = 0)
    {
        // TODO: Statically check the iterators for type sanity.
        typedef typename std::iterator_traits<BidirectionalIterator>::value_type value_type;
        typedef std::reverse_iterator<BidirectionalIterator> ReverseIterator;
        BOOST_STATIC_ASSERT(std::numeric_limits<value_type>::is_integer);
        BOOST_STATIC_ASSERT(!std::numeric_limits<value_type>::is_signed);

        if(_first != _last)
        {
            assert(_r != 0);
            assert(_k != std::numeric_limits<uintmax_t>::max()); // Because otherwise _k + 1 == 0.
            unsigned const _shift = _r * _d;
            uintmax_t const _bitmask = (1ul << _r) - 1;
            std::vector<uintmax_t> _C(static_cast<uintmax_t>(_k - _min) + 1); // NOTE: Could be a std::dynarray in C++14?
            ReverseIterator _rfirst(_last);
            ReverseIterator const _rlast(_first);

            for(; _first != _last; _first++)
                _C[((*_first >> _shift) - _min) & _bitmask]++;

            std::transform(_C.begin() + 1, _C.end(), _C.begin(), _C.begin() + 1, std::plus<value_type>());

            for(; _rfirst != _rlast; _rfirst++)
                *(_result + --_C[((*_rfirst >> _shift) - _min) & _bitmask]) = *_rfirst;
        }
    }

    
    /**
    * In-place but not stable.
    * Based on code from Rosetta Code downloaded July 2013.
    */
    template <typename InputIterator>
    void counting_sort( InputIterator _first, InputIterator _last,
                    typename std::iterator_traits<InputIterator>::value_type const _max,
                    typename std::iterator_traits<InputIterator>::value_type const _min = 0) 
    {
        typedef typename std::iterator_traits<InputIterator>::value_type value_type;
        
        BOOST_STATIC_ASSERT(std::numeric_limits<value_type>::is_integer);
        BOOST_STATIC_ASSERT(!std::numeric_limits<value_type>::is_signed);
        
        if(_first != _last && _max > _min)
        {
            uintmax_t const nlen = ( _max - _min ) + 1;
            assert(nlen != 0);
            uintmax_t temp[nlen];
            std::fill_n((uintmax_t*)(temp), nlen, uintmax_t());
            for(InputIterator _it(_first); _it != _last; _it++)
            {
                assert(_min <= *_it);
                assert(*_it <= _max);
                temp[*_it - _min]++;
            }
            InputIterator it(_first);
            for( value_type i = _min; i <= _max; i++ )
                while( temp[i - _min]-- )
                {
                    *(it++) = i;
                }
        }
    }
}
#endif

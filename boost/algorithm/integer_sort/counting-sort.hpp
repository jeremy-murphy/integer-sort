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
#include <vector>

#include <boost/static_assert.hpp>
#include <boost/integer.hpp>
#include <boost/concept_check.hpp>
#include <boost/concept/requires.hpp>


namespace boost {
namespace algorithm {
    namespace detail {
        
        template <typename Value, typename Shift, typename Bitmask>
        inline Value count_index(Value const a, Shift const b, Value const c, Bitmask const d)
        {
            return ((a >> b) - c) & d;
        }
        
    }
    /**
    * Requires that client allocates space for result beforehand.
    * 
    * @brief Generalized stable counting-sort.
    * 
    * \c BidirectionalIterator Type of input iterator.
    * \c OutputIterator Type of output iterator: only required to be forward.
    * 
    * \c first Input iterator that points to the first element of the unsorted data.
    * \c last Input iterator that points past the last element of the unsorted data.
    * \c result Output iterator that points to the first element where the sorted data will go.
    * \c k The largest value present in the input >> r * d.
    * \c r The radix or width of digit to consider.
    * \c d Which digit to consider.
    */
    template <typename InputIterator, typename OutputIterator>
        BOOST_CONCEPT_REQUIRES(((BidirectionalIterator<InputIterator>))
            ((Mutable_RandomAccessIterator<OutputIterator>))
            ((UnsignedInteger<typename std::iterator_traits<InputIterator>::value_type>)), 
                           (void))
    stable_counting_sort(InputIterator first, InputIterator last, OutputIterator result, 
                            typename std::iterator_traits<InputIterator>::value_type const k,
                            typename std::iterator_traits<InputIterator>::value_type const min = 0,
                            unsigned const r = sizeof(typename std::iterator_traits<InputIterator>::value_type) * 8,
                            unsigned const d = 0)
    {
        typedef std::reverse_iterator<InputIterator> ReverseIterator;

        if(first != last)
        {
            assert(r != 0);
            assert(k - min != std::numeric_limits<uintmax_t>::max()); // Because otherwise k - min + 1 == 0.
            unsigned const shift = r * d;
            uintmax_t const bitmask = (1ul << r) - 1;
            std::vector<uintmax_t> C(static_cast<uintmax_t>(k - min) + 1);
            ReverseIterator rfirst(last);
            ReverseIterator const rlast(first);

            // TODO: Could this be done faster by left-shifting _min and _bitmask once instead of right-shifting the value n times?
            for(; first != last; first++)
                C[detail::count_index(*first, shift, min, bitmask)]++;

            // Accumulate the counts in the temporary array.
            std::transform(C.begin() + 1, C.end(), C.begin(), C.begin() + 1, std::plus<uintmax_t>());

            for(; rfirst != rlast; rfirst++)
                *(result + --C[detail::count_index(*rfirst, shift, min, bitmask)]) = *rfirst;
        }
    }

    
    /**
     * \brief Unstable in-place counting-sort.
     * 
     * Based on code from Rosetta Code downloaded July 2013
     * This function will crash your machine if _k - _min is huge because it needs to allocate
     * _k - _min * sizeof(uintmax_t) contiguous bytes.
     * It is therefore recommended for use on data where _k - _min is relatively small.
     */
    template <typename InputIterator>
        BOOST_CONCEPT_REQUIRES(((Mutable_ForwardIterator<InputIterator>))
        ((UnsignedInteger<typename std::iterator_traits<InputIterator>::value_type>)), 
        (void))
    counting_sort(InputIterator first, InputIterator last,
                    typename std::iterator_traits<InputIterator>::value_type const k,
                    typename std::iterator_traits<InputIterator>::value_type const min = 0) 
    {
        if(first != last && k > min)
        {
            uintmax_t const nlen = ( k - min ) + 1;
            assert(nlen != 0);
            uintmax_t temp[nlen];
            std::fill_n((uintmax_t*)(temp), nlen, uintmax_t());
            for(InputIterator it(first); it != last; it++)
            {
                assert(min <= *it);
                assert(*it <= k);
                temp[*it - min]++;
            }

            for( uintmax_t i = min; i <= k; i++ )
                while( temp[i - min]-- )
                    *(first++) = i;
        }
    }
}
}
#endif

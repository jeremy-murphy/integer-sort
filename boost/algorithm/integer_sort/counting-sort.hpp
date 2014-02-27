//  (C) Copyright Jeremy W. Murphy 2013.
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/** \file counting-sort.hpp
 *  \brief Stable counting sort.
 */

#ifndef COUNTING_SORT
#define COUNTING_SORT

#include <iterator>
#include <numeric>
#include <cassert>
#include <limits>
#include <functional>
#include <vector>

#include <boost/concept_check.hpp>
#include <boost/concept/requires.hpp>
#include <boost/integer.hpp>
#include <boost/static_assert.hpp>
#include <boost/unordered_map.hpp>
#include <boost/utility/result_of.hpp>


namespace boost {
namespace algorithm {
    namespace detail {
        
        template <typename Value, typename Shift, typename Bitmask>
        inline Value count_index(Value const a, Shift const b, Value const c, Bitmask const d)
        {
            return ((a >> b) - c) & d;
        }
    }
    
    
    template <typename T>
    struct no_op
    {
        typedef T result_type;

        T const &operator()(T const &x) const
        {
            return x;
        }
    };
    
    
    /**
    * Requires that client allocates space for result beforehand.
    * 
    * @brief Generalized stable counting-sort.
    * 
    * \c Input Bidirectional input iterator.
    * \c Output Random access output iterator.
    * 
    * \param first Input iterator that points to the first element of the unsorted data.
    * \param last Input iterator that points past the last element of the unsorted data.
    * \param result Output iterator that points to the first element where the sorted data will go.
    * \param conv Function object that converts the input type to an unsigned integral type.
    * \param min The smallest value present in the input >> r * d.
    * \param max The largest value present in the input >> r * d.
    * \param r The radix or width of digit to consider.
    * \param d Which digit to consider.
    */
    template <typename Input, typename Output, typename Conversion>
        BOOST_CONCEPT_REQUIRES(((BidirectionalIterator<Input>))
            ((Mutable_RandomAccessIterator<Output>))
            ((UnsignedInteger<typename result_of<Conversion(typename std::iterator_traits<Input>::value_type)>::type>)), 
                           (void))
    stable_counting_sort(Input first, Input last, Output result, 
        Conversion conv = no_op<typename std::iterator_traits<Input>::value_type>(),
        typename result_of<Conversion(typename std::iterator_traits<Input>::value_type)>::type const min = 0,
        typename result_of<Conversion(typename std::iterator_traits<Input>::value_type)>::type const max = std::numeric_limits<typename result_of<Conversion(typename std::iterator_traits<Input>::value_type)>::type>::max(),
        unsigned const r = sizeof(typename result_of<Conversion(typename std::iterator_traits<Input>::value_type)>::type) * 8,
        unsigned const d = 0)
    {
        typedef std::reverse_iterator<Input> ReverseIterator;
        
        if(first != last)
        {
            Input next(first);
            next++;
            if(next == last)
                *result++ = *first;
            else
            {
                assert(r != 0);
                // TODO: Maybe this next assertion should be an exception?
                assert(max - min != std::numeric_limits<uintmax_t>::max()); // Because otherwise k - min + 1 == 0.
                unsigned const shift = r * d;
                uintmax_t const bitmask = (1ul << r) - 1;
                std::vector<uintmax_t> C(static_cast<uintmax_t>(max - min) + 1);
                ReverseIterator rfirst(last);
                ReverseIterator const rlast(first);
                
                assert(std::distance(first, last) == std::distance(rfirst, rlast));

                // TODO: Could this be done faster by left-shifting _min and _bitmask once instead of right-shifting the value n times?
                for(; first != last; first++)
                    C[detail::count_index(conv(*first), shift, min, bitmask)]++;

                std::partial_sum(C.begin(), C.end(), C.begin());

                for(; rfirst != rlast; rfirst++)
                {
                    std::size_t const offset = --C[detail::count_index(conv(*rfirst), shift, min, bitmask)];
                    *(result + offset) = *rfirst;
                }
            }
        }
    }

    
    /**
     * \brief Unstable in-place counting-sort.
     * 
     * Based on code from Rosetta Code downloaded July 2013
     * This function will crash your machine if max - min is huge because it 
     * needs to allocate max - min * sizeof(uintmax_t) contiguous bytes.
     * It is therefore recommended for use on data where max - min is relatively small.
     */
    template <typename Input>
        BOOST_CONCEPT_REQUIRES(((Mutable_ForwardIterator<Input>))
        ((UnsignedInteger<typename std::iterator_traits<Input>::value_type>)), 
        (void))
    counting_sort(Input first, Input last,
                typename std::iterator_traits<Input>::value_type const max,
                typename std::iterator_traits<Input>::value_type const min = 0) 
    {
        assert(max >= min);
        if(first != last && max > min)
        {
            uintmax_t const nlen = ( max - min ) + 1;
            uintmax_t temp[nlen];
            std::fill_n(temp, nlen, uintmax_t());
            for(Input it(first); it != last; it++)
            {
                assert(min <= *it);
                assert(*it <= max);
                temp[*it - min]++;
            }

            for( uintmax_t i = min; i <= max; i++ )
                while( temp[i - min]-- )
                    *(first++) = i;
        }
    }
}
}
#endif

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
#include <boost/algorithm/minmax_element.hpp>
#include <boost/scoped_array.hpp>


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
        
        no_op() {}

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
    stable_counting_sort(Input first, Input last, Output result, Conversion conv,
        typename result_of<Conversion(typename std::iterator_traits<Input>::value_type)>::type const min,
        typename result_of<Conversion(typename std::iterator_traits<Input>::value_type)>::type const max,
        unsigned const radix, unsigned char const digit)
    {
        typedef std::reverse_iterator<Input> ReverseIterator;

        if(first != last)
        {
            Input next(first);
            ++next;
            if(next == last)
                *result++ = *first;
            else
            {
                assert(radix != 0);
                // TODO: Maybe this next assertion should be an exception?
                assert(max - min != std::numeric_limits<uintmax_t>::max()); // Because k - min + 1 == 0.
                unsigned const shift = radix * digit;
                uintmax_t const bitmask = (1ul << radix) - 1;
                std::vector<uintmax_t> C(static_cast<uintmax_t>(max - min) + 1);
                ReverseIterator rfirst(last);
                ReverseIterator const rlast(first);

                // TODO: Could this be done faster by left-shifting _min and _bitmask once instead of right-shifting the value n times?
                for(; first != last; first++)
                    C[detail::count_index(conv(*first), shift, min, bitmask)]++;

                std::partial_sum(C.begin(), C.end(), C.begin());

                for(; rfirst != rlast; rfirst++)
                    *(result + --C[detail::count_index(conv(*rfirst), shift, min, bitmask)]) = *rfirst;
            }
        }
    }

    
    template <typename Input, typename Output, typename Conversion>
        BOOST_CONCEPT_REQUIRES(((BidirectionalIterator<Input>))
        ((Mutable_RandomAccessIterator<Output>))
        ((UnsignedInteger<typename result_of<Conversion(typename std::iterator_traits<Input>::value_type)>::type>)), 
                           (void))
    stable_counting_sort(Input first, Input last, Output result, Conversion conv,
        typename result_of<Conversion(typename std::iterator_traits<Input>::value_type)>::type const min,
        typename result_of<Conversion(typename std::iterator_traits<Input>::value_type)>::type const max)
    {
        unsigned const radix(sizeof(typename result_of<Conversion(typename std::iterator_traits<Input>::value_type)>::type) * 8);
        return stable_counting_sort(first, last, result, conv, min, max, radix, 0);
    }
    
    
    template <typename Input, typename Output, typename Conversion>
        BOOST_CONCEPT_REQUIRES(((BidirectionalIterator<Input>))
        ((Mutable_RandomAccessIterator<Output>))
        ((UnsignedInteger<typename result_of<Conversion(typename std::iterator_traits<Input>::value_type)>::type>)), 
                           (void))
    stable_counting_sort(Input first, Input last, Output result, Conversion conv)
    {
        if(first != last)
        {
            std::pair<Input, Input> const bound(boost::minmax_element(first, last));
            return stable_counting_sort(first, last, result, conv, *bound.first, *bound.second);
        }
    }
    
    
    template <typename Input, typename Output>
        BOOST_CONCEPT_REQUIRES(((BidirectionalIterator<Input>))
        ((Mutable_RandomAccessIterator<Output>)),
                           (void))
    stable_counting_sort(Input first, Input last, Output result)
    {
        return stable_counting_sort(first, last, result, no_op<typename std::iterator_traits<Input>::value_type>());
    }
    
    
    /**
     * \brief Unstable in-place counting-sort.
     * 
     * \param first Input iterator that points to the first element of the unsorted data.
     * \param last Input iterator that points past the last element of the unsorted data.
     * \param min The smallest value present in the input >> r * d.
     * \param max The largest value present in the input >> r * d.
     * 
     * Based on code from Rosetta Code downloaded July 2013
     * It is recommended for use on data where max - min is relatively small.
     */
    template <typename Input>
        BOOST_CONCEPT_REQUIRES(((Mutable_ForwardIterator<Input>))
        ((UnsignedInteger<typename std::iterator_traits<Input>::value_type>)), 
        (void))
    counting_sort(Input first, Input last,
                typename std::iterator_traits<Input>::value_type const min,
                typename std::iterator_traits<Input>::value_type const max)
    {
        assert(max >= min);
        if(first != last)
        {
            Input next(first);
            ++next;
            if(next != last && min < max)
            {
                uintmax_t const nlen = ( max - min ) + 1;
                scoped_array<uintmax_t> temp(new uintmax_t[nlen]);
                std::fill_n(&temp[0], nlen, 0);
                for(Input it(first); it != last; ++it)
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
    
    
    /**
     * \brief Unstable in-place counting-sort.
     * 
     * \param first Input iterator that points to the first element of the unsorted data.
     * \param last Input iterator that points past the last element of the unsorted data.
     */
    template <typename Input>
        BOOST_CONCEPT_REQUIRES(((Mutable_ForwardIterator<Input>))
        ((UnsignedInteger<typename std::iterator_traits<Input>::value_type>)), 
                           (void))
    counting_sort(Input first, Input last)
    {
        if(first != last)
        {
            std::pair<Input, Input> const bound(boost::minmax_element(first, last));
            return counting_sort(first, last, *bound.first, *bound.second);
        }
    }
}
}
#endif

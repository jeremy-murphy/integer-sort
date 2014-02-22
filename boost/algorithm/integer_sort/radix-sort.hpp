//  (C) Copyright Jeremy W. Murphy 2013.
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file radix-sort.hpp
/// \brief Stable LSD radix sort.

#ifndef RADIX_SORT
#define RADIX_SORT

#include <boost/algorithm/integer_sort/counting-sort.hpp>

#include <iterator>
#include <cmath>

#ifndef NDEBUG
#include <iostream>
#endif

namespace boost {
namespace algorithm {
    /**
     * \fn stable_radix_sort
     * \brief Stable LSD radix-sort that uses counting-sort.
     * 
     * \c Input Bidirectional input iterator.
     * \c Output Random access output iterator.
     * 
     * \param first Input iterator that points to the first element of the unsorted data.
     * \param last Input iterator that points past the last element of the unsorted data.
     * \param result Output iterator that points to the first element where the sorted data will go.
     * \param conv Function object that converts the input type to an unsigned integral type.
     * \param k The largest value present in the input >> r * d.
     * \param min The smallest value present in the input >> r * d.
     */
    template <typename Input, typename Output, typename Conversion>
    BOOST_CONCEPT_REQUIRES(((BidirectionalIterator<Input>))
        ((Mutable_RandomAccessIterator<Output>))
        ((UnsignedInteger<typename result_of<Conversion(typename std::iterator_traits<Input>::value_type)>::type>)), 
                           (void))
    stable_radix_sort(Input first, Input last, Output result, 
        Conversion conv = no_op<typename std::iterator_traits<Input>::value_type>(),
        typename result_of<Conversion(typename std::iterator_traits<Input>::value_type)>::type const k = std::numeric_limits<typename result_of<Conversion(typename std::iterator_traits<Input>::value_type)>::type>::max(),
        typename result_of<Conversion(typename std::iterator_traits<Input>::value_type)>::type const min = 0,
        unsigned const radix = 0)
    {
        typedef typename std::iterator_traits<Input>::difference_type difference_type;
        typedef typename std::iterator_traits<Input>::value_type value_type;
        typedef typename result_of<Conversion(typename std::iterator_traits<Input>::value_type)>::type uint_type;
        
        assert(k >= min);
        
        if(first != last)
        {
            difference_type const n(std::distance(first, last));
            unsigned const b(sizeof(value_type) * 8);
            
            /* TODO: Calculate and use effective b.
            * It's assumed that value_type is no larger than necessary.  That is, k >= 2^(b/2).
            * However, if k << 2^b, then there are a lot of unused significant bits that do not need sorting.
            * Using effective b could reduce the number of passes required to sort.
            */
            
            unsigned const flgn(floor(log2(n))); // TODO: Is there a faster way to calculate integer log2?
            unsigned const r(radix == 0 ? std::min((b < flgn ? b : flgn), 8u) : radix); // Limit ourselves to sane values.
            unsigned const d(ceil(static_cast<double>(b) / static_cast<double>(r)));
            
            assert(r * d >= b);
            
            if(d == 1)
                stable_counting_sort(first, last, result, conv, k, min);
            else
            {
                std::vector<value_type> tmp1(first, last);
                uint_type const dk = (uint_type(1) << r) - 1; // TODO: This can be improved.
                // NOTE: Is there an easy way to utilize minimum here?
                
                if(d == 2)
                {
                    stable_counting_sort(first, last, tmp1.begin(), conv, dk, 0, r, 0);
                    stable_counting_sort(tmp1.begin(), tmp1.end(), result, conv, dk, 0, r, 1);
                }
                else
                {
                    std::vector<value_type> tmp2(n);
                    for(unsigned i = 0; i < d; i++)
                    {
                        stable_counting_sort(tmp1.begin(), tmp1.end(), tmp2.begin(), conv, dk, 0, r, i);
                        std::swap(tmp1, tmp2);
                    }
                    
                    std::copy(tmp1.begin(), tmp1.end(), result);
                }
            }
        }
    }
}
}
#endif

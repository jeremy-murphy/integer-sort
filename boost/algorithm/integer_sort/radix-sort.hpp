//  (C) Copyright Jeremy W. Murphy 2013.
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
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
     * \c InputIterator 
     * \param first ...
     */
    template <typename Input, typename Output>
    BOOST_CONCEPT_REQUIRES(((BidirectionalIterator<Input>))
        ((Mutable_RandomAccessIterator<Output>))
        ((UnsignedInteger<typename std::iterator_traits<Input>::value_type>)), 
                           (void))
    stable_radix_sort(Input first, Input last, Output result, 
                        typename std::iterator_traits<Input>::value_type const k, 
                        typename std::iterator_traits<Input>::value_type const minimum = 0, 
                        unsigned const radix = 0)
    {
        typedef typename std::iterator_traits<Input>::difference_type difference_type;
        typedef typename std::iterator_traits<Input>::value_type value_type;
        
        assert(k >= minimum);
        
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
            unsigned const r(radix == 0 ? std::min((b < flgn ? b : flgn), 16u) : radix); // Limit ourselves to sane values.
            unsigned const d(ceil(static_cast<double>(b) / static_cast<double>(r)));
            
            assert(r * d >= b);
            
            if(d == 1)
                stable_counting_sort(first, last, result, no_op<value_type>(), k, minimum);
            else
            {
                std::vector<value_type> input(first, last);
                value_type const dk = (value_type(1) << r) - 1; // TODO: This can be improved.
                // NOTE: Is there an easy way to utilize minimum here?
                
                if(d == 2)
                {
                    stable_counting_sort(first, last, input.begin(), no_op<value_type>(), dk, 0, r, 0);
                    stable_counting_sort(input.begin(), input.end(), result, no_op<value_type>(), dk, 0, r, 1);
                }
                else
                {
                    std::vector<value_type> output(n);
                    for(unsigned i = 0; i < d; i++)
                    {
                        stable_counting_sort(input.begin(), input.end(), output.begin(), no_op<value_type>(), dk, 0, r, i);
                        std::swap(input, output);
                    }
                    
                    std::copy(input.begin(), input.end(), result);
                }
            }
        }
    }
}
}
#endif

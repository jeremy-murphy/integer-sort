//  (C) Copyright Jeremy W. Murphy 2013.
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef RADIX_SORT
#define RADIX_SORT

#include "counting-sort.hpp"

#include <iterator>
#include <cmath>

#ifndef NDEBUG
#include <iostream>
#endif

namespace boost
{
    /**
     * \fn stable_radix_sort
     * \brief Stable LSD radix-sort that uses counting-sort.
     * 
     * \c InputIterator 
     * \param _first ...
     */
    template <typename InputIterator, typename OutputIterator>
    void stable_radix_sort(InputIterator _first, InputIterator _last, OutputIterator _result, 
                           typename std::iterator_traits<InputIterator>::value_type const _k, 
                           typename std::iterator_traits<InputIterator>::value_type const _min = 0, 
                           unsigned const _radix = 0)
    {
        typedef typename std::iterator_traits<InputIterator>::difference_type difference_type;
        typedef typename std::iterator_traits<InputIterator>::value_type value_type;
        
        assert(_k >= _min);
        
        // Calculate some useful values.
        difference_type const n(std::distance(_first, _last));
        unsigned const b(sizeof(value_type) * 8);
        
        /* TODO: Calculate and use effective b.
         * It's assumed that value_type is no larger than necessary.  That is, k >= 2^(b/2).
         * However, if k << 2^b, then there are a lot of unused significant bits that do not need sorting.
         * Using effective b could reduce the number of passes required to sort.
         */
        
        unsigned const flgn(floor(log2(n))); // TODO: Is there a faster way to calculate integer log2?
        unsigned const r(_radix == 0 ? std::min((b < flgn ? b : flgn), 16u) : _radix); // Limit ourselves to sane values.
        unsigned const d(ceil(static_cast<float>(b) / static_cast<float>(r)));
        
        // std::cout << "n = " << n << ", " << "b = " << b << ", " << "⌊lg(n)⌋ = " << flgn << ", r = " << r << ", d = " << d << std::endl;
        assert(r * d >= b);
        
        if(d == 1)
        {
            boost::stable_counting_sort(_first, _last, _result, _k, _min);
        }
        else
        {
            std::vector<value_type> _input(_first, _last);
            value_type const _dk = (value_type(1) << r) - 1; // TODO: This can be improved.
            // NOTE: Is there an easy way to utilize _min here?
            
            if(d == 2)
            {
                boost::stable_counting_sort(_first, _last, _input.begin(), _dk, 0, r, 0);
                boost::stable_counting_sort(_input.begin(), _input.end(), _result, _dk, 0, r, 1);
            }
            else
            {
                std::vector<value_type> _output(n);
                for(unsigned _i = 0; _i < d; _i++)
                {
                    boost::stable_counting_sort(_input.begin(), _input.end(), _output.begin(), _dk, 0, r, _i);
                    std::swap(_input, _output);
                }
                
                std::copy(_input.begin(), _input.end(), _result);
            }
        }
    }
}
#endif

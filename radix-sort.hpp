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
    namespace detail
    {
        // We want r = 2^k and r <= lg(n).
        unsigned greatest_r(unsigned const b, unsigned const flgn)
        {
            unsigned r = 8;
            for(unsigned __i = 4; __i < 8; __i++)
            {
                unsigned const __s(1 << __i);
                if(__s <= b && __s <= flgn)
                    r = __s;
                else
                    break;
            }
            
            return r;
        }
    }
    
    template <typename InputIterator, typename OutputIterator>
    void radix_sort(InputIterator __first, InputIterator __last, OutputIterator __result, typename std::iterator_traits<InputIterator>::value_type const k, unsigned const __radix = 0)
    {
        typedef typename std::iterator_traits<InputIterator>::difference_type difference_type;
        typedef typename std::iterator_traits<InputIterator>::value_type value_type;
        
        // Calculate some useful values.
        difference_type const n(std::distance(__first, __last));
        unsigned const b(sizeof(value_type) * 8);
        unsigned const flgn(floor(log2(n)));
        
#ifdef WORLD_ACCORDING_TO_CLRS
        unsigned const r(b < flgn ? b : flgn);
#else
        unsigned const r(__radix == 0 ? detail::greatest_r(b, flgn) : __radix);
#endif
        unsigned const d(ceil(b / r));

        assert(d % 2 == 0);
        
#ifndef NDEBUG
        std::cout << "n = " << n << ", " << "b = " << b << ", " << "⌊lg(n)⌋ = " << flgn << ", r = " << r << ", d = " << d << std::endl;
#endif
        
        std::vector<value_type> __input(__first, __last), __output(n);
        
        for(unsigned __i = 0; __i < d; __i++)
        {
            unsigned __bitmask = (1 << r) - 1;
            __bitmask <<= __i * r;
            boost::stable_counting_sort(__input.begin(), __input.end(), __output.begin(), k, __bitmask);
            
            std::swap(__input, __output);
        }
        
        // Swap is always called an even number of times, so __input is the result.
#ifdef __GXX_EXPERIMENTAL_CXX0X__
        std::move(__input.begin(), __input.end(), __result);
#else
        std::copy(__input.begin(), __input.end(), __result);
#endif
    }
}
#endif

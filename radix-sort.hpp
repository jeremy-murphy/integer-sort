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
                unsigned const __s(pow(2, __i));
                if(__s <= b && __s <= flgn)
                    r = __s;
                else
                    break;
            }
            
            return r;
        }
    }
    
    template <typename InputIterator, typename ReverseIterator, typename OutputIterator>
    void radix_sort(InputIterator __first, InputIterator __last, ReverseIterator __rfirst, ReverseIterator __rlast, OutputIterator __result, unsigned const __radix = 0)
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

#ifndef NDEBUG
        std::cout << "n = " << n << ", " << "b = " << b << ", " << "⌊lg(n)⌋ = " << flgn << ", r = " << r << ", d = " << d << std::endl;
#endif
        
        for(unsigned i = 0; i < d; i++)
        {
            
        }
    }
}
#endif

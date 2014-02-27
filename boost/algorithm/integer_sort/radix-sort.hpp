//  (C) Copyright Jeremy W. Murphy 2013.
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file radix-sort.hpp
/// \brief Stable LSD radix sort.

#ifndef RADIX_SORT
#define RADIX_SORT

#include <boost/algorithm/integer_sort/counting-sort.hpp>
#include <boost/algorithm/minmax_element.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include <iterator>
#include <cmath>

#ifndef NDEBUG
#include <iostream>
#endif

/* TODO: Calculate and use effective b?
 * It's assumed that value_type is no larger than necessary.  That is, k >= 2^(b/2).
 * However, if k << 2^b, then there are a lot of unused significant bits that do not need sorting.
 * Using effective b could reduce the number of passes required to sort.
 */

namespace boost {
namespace algorithm {
    
    namespace detail
    {
        template <class T>
        class ra_raw_storage_iterator
        : public boost::iterator_facade<ra_raw_storage_iterator<T>, T, random_access_traversal_tag>
        {
            friend class boost::iterator_core_access;
            
            T& dereference() const { return *M_iter; }
            
            bool equal(ra_raw_storage_iterator const &other) const
            {
                return M_iter == other.M_iter;
            }
            
            void increment() { ++M_iter; }
            void decrement() { --M_iter; }
            
            void advance(ssize_t n) { M_iter += n; }
            
            std::ptrdiff_t distance_to(ra_raw_storage_iterator const &z) const { return z.M_iter - M_iter; }
            
            struct enabler {};
        protected:
            T * M_iter;
            
        public:
            explicit
            ra_raw_storage_iterator(T * x) : M_iter(x) {}

            template <typename U>
            ra_raw_storage_iterator(ra_raw_storage_iterator<U> const &other, typename enable_if<is_convertible<U*, T*>, enabler>::type = enabler()) : M_iter() {}

            /*
            ra_raw_storage_iterator&
            operator*() { return *this; }
            */
            
            ra_raw_storage_iterator&
            operator=(const T& element)
            {
                // std::_Construct(addressof(*M_iter), element);
                ::new(static_cast<void *>(addressof(*M_iter))) T(element);
                return *this;
            }
        };
        
        
        template <typename T>
        struct destroy
        {
            destroy() {}
            void operator()(T &x) const
            {
                x.~T();
            }
        };
    }
    
    
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
     * \param min The smallest value present in the input >> r * d.
     * \param max The largest value present in the input >> r * d.
     */
    template <typename Input, typename Output, typename Conversion>
    BOOST_CONCEPT_REQUIRES(((BidirectionalIterator<Input>))
        ((Mutable_RandomAccessIterator<Output>))
        ((UnsignedInteger<typename result_of<Conversion(typename std::iterator_traits<Input>::value_type)>::type>)), 
                           (void))
    stable_radix_sort(Input first, Input last, Output result, Conversion conv,
        typename result_of<Conversion(typename std::iterator_traits<Input>::value_type)>::type const min,
        typename result_of<Conversion(typename std::iterator_traits<Input>::value_type)>::type const max,
        unsigned const radix)
    {
        typedef typename std::iterator_traits<Input>::value_type value_type;
        typedef typename result_of<Conversion(typename std::iterator_traits<Input>::value_type)>::type uint_type;
        typedef typename std::iterator_traits<Input>::difference_type difference_type;
        
        if(first != last)
        {
            assert(max >= min);
            
            Input next(first);
            next++;
            if(next == last)
                *result++ = *first;
            else
            {
                char unsigned const bits(sizeof(uint_type) * 8), 
                                digits(ceil(static_cast<double>(bits) / static_cast<double>(radix)));
            
                assert(radix * digits >= bits);
                
                if(digits == 1)
                    stable_counting_sort(first, last, result, conv, min, max);
                else
                {
                    difference_type const n = std::distance(first, last);
                    std::pair<value_type *, std::ptrdiff_t> p = std::get_temporary_buffer<value_type>(n);
                    if(p.second != n)
                        throw std::bad_alloc();
                    value_type *tmp1 = p.first;
                    uint_type const dk = (uint_type(1) << radix) - 1; // TODO: This can be improved.
                    // NOTE: Is there an easy way to utilize minimum here?
                    
                    detail::ra_raw_storage_iterator<value_type> begin1(tmp1);
                    
                    if(digits == 2)
                    {
                        stable_counting_sort(first, last, begin1, conv, 0, dk, radix, 0);
                        stable_counting_sort(tmp1, tmp1 + n, result, conv, 0, dk, radix, 1);
                    }
                    else
                    {
                        std::pair<value_type *, std::ptrdiff_t> q = std::get_temporary_buffer<value_type>(n);
                        if(q.second != n)
                            throw std::bad_alloc();
                        value_type *tmp2 = q.first;
                        uint_type const dk = (uint_type(1) << radix) - 1; // TODO: This can be improved.
                        // NOTE: Is there an easy way to utilize minimum here?
                        stable_counting_sort(first, last, begin1, conv, 0, dk, radix, 0);
                        
                        for(unsigned i = 1; i < digits; i++)
                        {
                            detail::ra_raw_storage_iterator<value_type> begin2(tmp2);
                            stable_counting_sort(tmp1, tmp1 + n, begin2, conv, 0, dk, radix, i);
                            std::swap(tmp1, tmp2);
                            std::for_each(tmp2, tmp2 + n, detail::destroy<value_type>());
                        }
                        
                        std::return_temporary_buffer(tmp2);
                        std::copy(tmp1, tmp1 + n, result);
                    }
                    
                    std::for_each(tmp1, tmp1 + n, detail::destroy<value_type>());
                    std::return_temporary_buffer(tmp1);
                }
            }
        }
    }
    
    
    template <typename Input, typename Output, typename Conversion>
    BOOST_CONCEPT_REQUIRES(((BidirectionalIterator<Input>))
    ((Mutable_RandomAccessIterator<Output>))
    ((UnsignedInteger<typename result_of<Conversion(typename std::iterator_traits<Input>::value_type)>::type>)), 
                           (void))
    stable_radix_sort(Input first, Input last, Output result, Conversion conv,
        typename result_of<Conversion(typename std::iterator_traits<Input>::value_type)>::type const min,
        typename result_of<Conversion(typename std::iterator_traits<Input>::value_type)>::type const max)
    {
        typedef typename result_of<Conversion(typename std::iterator_traits<Input>::value_type)>::type uint_type;
        typedef typename std::iterator_traits<Input>::difference_type difference_type;
        
        difference_type const n = std::distance(first, last);
        char unsigned const  b(sizeof(uint_type) * 8),
                        flgn(floor(log2(n))), // TODO: Is there a faster way to calculate integer log2?
                        radix(std::min(std::min(b, flgn), static_cast<char unsigned>(8u))); // Limit ourselves to sane values.
        return stable_radix_sort(first, last, result, conv, min, max, radix);
    }
    
    
    template <typename Input, typename Output, typename Conversion>
        BOOST_CONCEPT_REQUIRES(((BidirectionalIterator<Input>))
        ((Mutable_RandomAccessIterator<Output>))
        ((UnsignedInteger<typename result_of<Conversion(typename std::iterator_traits<Input>::value_type)>::type>)), 
                            (void))
    stable_radix_sort(Input first, Input last, Output result, 
        Conversion conv = no_op<typename std::iterator_traits<Input>::value_type>())
    {
        std::pair<Input, Input> const bounds = minmax_element(first, last);
        return stable_radix_sort(first, last, result, conv, conv(*bounds.first), conv(*bounds.second));
    }
}
}
#endif

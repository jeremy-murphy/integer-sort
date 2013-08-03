integer-sort
============

C++ implementations of textbook integer sorting algorithms (with a view to inclusion in Boost).

The C++ standard library includes comparison sort functions, which run in Ω(n lg n) time.
Counting sort algorithms do not have this lower bound and can sort in Θ(n).  See CLRS for details.

Currently, there is a stable counting-sort, unstable in-place counting-sort and an LSD radix sort.


Jeremy Murphy
jeremy.william.murphy@gmail.com

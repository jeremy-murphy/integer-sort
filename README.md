integer-sort
============

C++ implementation of textbook integer sorting algorithms (with a view to inclusion in Boost).

The C++ standard library includes comparison sort functions, which run in Ο(n lg n) time.
Counting sort algorithms can sort in Θ(n).  See CLRS for details.

Currently, there is a stable counting-sort, unstable in-place counting-sort and a stable LSD radix sort.

### Feedback

I'm really keeen to get feedback from people at the moment!  The main step for testing is to calculate the min and max values in your input that become the min and k parameters.  Oh and making a function object to convert your input type to an unsigned integral type, if the input type is not unigned integral already.  Thanks, cheers.


Jeremy Murphy
jeremy.william.murphy@gmail.com

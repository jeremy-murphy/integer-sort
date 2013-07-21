integer-sort
============

C++ implementations of textbook integer sorting algorithms (with a view to inclusion in Boost).

The C++ standard library includes a number of functions to sort, all of which use a comparison 
sort that has a lower bound of Ω(n lg n).  Counting sort algorithms do not have this lower bound
and can sort in Θ(n).  See CLRS for details.

Note: The main.cpp is just there for my testing purposes, you will probably have to comment out
some missing headers if you want to compile and use it.

Jeremy Murphy
jeremy.william.murphy@gmail.com

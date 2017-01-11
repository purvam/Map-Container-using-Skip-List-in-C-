# Map-Container-using-Skip-List-in-C++

Simulated std::map container including its iterators, const iterators and reverse iterators, in C++ using templates.

Used Skip Lists as underlying structure, to change the maximum height dynamically for better performance. This gives O(log(N)) time complexity for search, insertion and deletion.

Map class also has nested clases: Iterator, ConstIterator and ReverseIterator, which are similar to iterator, const_iterator,reverse_iterator.

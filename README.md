stlxx
=====
C++11 header only thread-safe STL containers

Motivation:
-----------
  * thread-safe STL containers and *practically* any other type
  * deadlock avoidance algorithm [info](http://en.cppreference.com/w/cpp/thread/lock)
  * initial support of transactional memory [info](http://en.cppreference.com/w/cpp/language/transactional_memory)
  * implemented as reference counting objects
  * usage of recursive_mutex per default
  * access through operator->()
  * custom type should provide move constructor / operator=

Test compilation on Linux:
--------------------------
  * g++ -Wall --std=c++11 -O3 -s test.cpp -o test -lpthread
  * -lpthread may needed due to gcc bug

Contributions are welcome
-------------------------

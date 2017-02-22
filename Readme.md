A high performance lock free data structure lib only for X86

## Stack

A high performance lock free stack

### Feature

> * Linearizable

> * Safety Memory Reclamation

> * ABA safe

> * High Performance with Elimination Backoff

> * Customized Memory Allocator

### Demo

g++ -pthread -O2 -lrt src/test_hplfds_stack.cpp

### Reference

[A Scalable Lock Free Stack Algorithm](http://www.cs.tau.ac.il/~shanir/nir-pubs-web/Papers/Lock_Free.pdf)

## Queue

A high performance lock free unbouned queue. AKA, MS queue.

### Feature

> * Linearizable

> * Multi-producer, Multi-consumer 

> * Safety Memory Reclamation

> * ABA safe

> * Customized Memory Allocator

### Demo

g++ -pthread -O2 -lrt src/test_hplfds_ms_queue.cpp

### Reference

[Simple, Fast, and Practical Non-Blocking and Blocking Concurrent Queue Algorithms](https://www.research.ibm.com/people/m/michael/podc-1996.pdf)

## Epoch Based Reclamation

An effective lock free garbage collection impl for lock free data strcuture

### Feature

> * Single Writer, Multi Readers

### Demo

g++ -pthread -O2 src/test_hplfds_ebr.cpp

### Reference

Chinese Version:

[Lock Free中的Epoch Based Reclamation](http://www.yebangyu.org/blog/2016/09/09/epochbasedreclamation/)

## HashMap


# Common Data-Structures

> My implementations during university studies,
the structures are well tested without memory leaks.

- Generic **AvlRankTree**
  - Insert,Remove,Delete in `O(logn)`
  - The ranks are **Keys** with **Max** values.
  - Merge trees `O(n)`.
  - Initial tree with sorted array in `O(n)`.
  - Get sorted array of entries in `O(n)`.
- Generic **HashTable**
  - Dynamic array.
  - Chain Hashing with Avl Tree.
  - Insert,Remove,Delete in `O(1) average amortized` 
- Generic **IterableList**
  - Implements `==`,`!=`, and `Iterator` interface.
- Generic **LinkedList**
  - Doubly linked list.
- **MinHeap** (integers)
  - `log(n)` operations.
  - Getting min in `O(1)` complexity.
- Generic **SortedSet**
  - Implemented as Linked list
- **UnionFind** (numbered groups)
  - `O(log*n)` operations.

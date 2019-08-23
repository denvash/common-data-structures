# Common Data-Structures

> My implementations during university studies,
the structures are well tested without memory leaks.

- Generic **AvlRankTree**
  - The ranks are **Keys** with **Max** values.
  - Merge trees `O(n)`.
  - Initial tree with sorted array at *O(n)* time.
  - Get sorted array of keys.
  - Get sorted array of data.
- Generic **HashTable**
  - Implemented with AvlTree.
  - Dynamic array.
  - Chain Hashing.
- Generic **IterableList**
  - Implements `==`,`!=`, and `Iterator` interface.
- Generic **LinkedList**
  - Doubly linked list.
- **MinHeap** (integers)
  - `log(n)` operations.
  - Getting min in *O(1)* complexity.
- Generic **SortedSet**
  - Implemented as Linked list
- **UnionFind** (numbered groups)
  - `O(log*n)` operations.

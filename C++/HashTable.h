#include "AVLTree.h"

#define nullptr 0
#define EMPTY_SIZE 0

class HashKeyDoesNotExist : public exception {
};

template<class Value>
class HashTable {
private:
    struct Node {
        int key{};
        Value value;

        Node(int key, Value value) : key(key), value(value) {}

        Node() = default;
    };

    AVLTree<int, Node> **hashTable;
    int size;
    int counter;

    void init(AVLTree<int, Node> **array, int key, Value value);

    void extend();

    void destroyHash();

public:

    class DestroyFunction {
    public:
        virtual void operator() (Value&) {}
    };

    HashTable() : size(EMPTY_SIZE), counter(EMPTY_SIZE) {}

    explicit HashTable(int initial_size);

    ~HashTable();

    Value& getValue(int key);

    int hashFunction(int key);

    void insert(int key, Value value);

    void remove(int key);

    bool containsKey(int key);

    bool isEmpty();

    int getSize();

    void destroyHash(DestroyFunction* f);
};

template<class Value>
HashTable<Value>::HashTable(int initial_size) {
    size = initial_size * 2;
    counter = EMPTY_SIZE;
    hashTable = new AVLTree<int, Node> *[size];

    for (int i = 0; i < size; ++i) {
        hashTable[i] = NULL;
    }
}

template<class Value>
HashTable<Value>::~HashTable() {
//    destroyHash();
}

template<class Value>
int HashTable<Value>::hashFunction(int key) {
    return key % size;
}

template<class Value>
void HashTable<Value>::insert(int key, const Value value) {
    if (size == 0) {
        size = 1;
        auto initHash = new AVLTree<int, Node> *[size];
        for (int i = 0; i < size; i++) { initHash[i] = nullptr; }


        init(initHash, key, value);
        hashTable = initHash;
    } else {
        if (counter == size) { extend(); }
        init(hashTable, key, value);
    }
    ++counter;
}

template<class Value>
Value& HashTable<Value>::getValue(int key) {
    AVLTree<int, Node> *tree = hashTable[hashFunction(key)];
    if (!tree) { throw HashKeyDoesNotExist(); }
    return tree->getValue(key).value;
}

template<class Value>
void HashTable<Value>::extend() {
    int prev_size = size;
    size *= 2;

    auto new_hash = new AVLTree<int, Node> *[size];
    for (int i = 0; i < size; i++) { new_hash[i] = nullptr; }

    for (int i = 0; i < prev_size; i++) {

        AVLTree<int, Node> *tree = hashTable[i];
        if (tree == nullptr) continue;

        Node *array = tree->getValueSorted();
        for (int j = 0; j < tree->getSize(); ++j) {
            init(new_hash, array[j].key, array[j].value);
        }
        delete[] array;
    }
    for (int i = 0; i < prev_size; ++i) {
        if (hashTable[i] != nullptr) { delete hashTable[i]; }
    }
    delete[] hashTable;

    hashTable = new_hash;
}

template<class Value>
bool HashTable<Value>::containsKey(int key) {
    if (isEmpty()) { return false; }
    AVLTree<int, Node> *tree = hashTable[hashFunction(key)];
    return !(!tree || tree->getSize() == EMPTY_SIZE || !tree->contains(key));
}

template<class Value>
void HashTable<Value>::remove(int key) {
    if (!containsKey(key)) return;
    AVLTree<int, Node> *tree = hashTable[hashFunction(key)];
    tree->remove(key);
}

template<class Value>
void HashTable<Value>::init(AVLTree<int, Node> **array, int key, Value value) {
    int index = hashFunction(key);

    AVLTree<int, Node> *newTree;
    if (!array[index]) {
        newTree = new AVLTree<int, Node>();
        array[index] = newTree;
    }
    newTree = array[index];
    Node node = Node(key, value);
    newTree->insert(key, node);
}

template<class Value>
void HashTable<Value>::destroyHash() {
    for (int i = 0; i < size; ++i) {
        if (hashTable[i] != nullptr) { delete hashTable[i]; }
    }
    if (size > 0) delete[] hashTable;
}

template<class Value>
bool HashTable<Value>::isEmpty() {
    return size <= EMPTY_SIZE;
}

template<class Value>
int HashTable<Value>::getSize() {
    return isEmpty() ? EMPTY_SIZE : size;
}

template<class Value>
void HashTable<Value>::destroyHash(DestroyFunction* f) {
    for (int i = 0; i < size; ++i) {
        if (hashTable[i] != nullptr) {
            if (!hashTable[i]->isEmpty()) {
                auto sortedKeys = hashTable[i]->getKeySorted();
                for (int k = 0; k < hashTable[i]->getSize(); ++k) {
                    int key = *sortedKeys[k];
                    f->operator()(hashTable[i]->getValue(key).value);
                }
                delete[] sortedKeys;
            }
            delete hashTable[i];
        }
    }
    if (size > 0) delete[] hashTable;
}
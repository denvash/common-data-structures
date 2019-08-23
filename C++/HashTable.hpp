#include "AvlRankTree.hpp"

#define EMPTY_SIZE 0

class HashKeyDoesNotExist : public exception {
};

template<class V>
class HashTable {
private:
    struct Node {
        int _key;
        V _value;

        Node(int key, V value) : _key(key), _value(value) {}
    };

    AVLRankTree<int, Node> **_hashTable;
    int _size;
    int _counter;

    void init(AVLRankTree<int, Node> **array, int key, V value);

    void resize(bool toShrink);

public:

    class ValueDestroyFunction {
    public:
        virtual void operator()(V) {}
    };

    HashTable() : _size(EMPTY_SIZE), _counter(EMPTY_SIZE) {}

    explicit HashTable(int initial_size);

    V getValue(int key);

    int hashFunction(int key);

    void insert(int key, V value);

    void remove(int key);

    bool includesKey(int key);

    bool isEmpty();

    int getSize();

    void destroyHash(ValueDestroyFunction *f);
};

template<class V>
HashTable<V>::HashTable(int initial_size) {
    _size = initial_size * 2;
    _counter = EMPTY_SIZE;
    _hashTable = new AVLRankTree<int, Node> *[_size];

    for (int i = 0; i < _size; ++i) {
        _hashTable[i] = NULL;
    }
}

template<class V>
int HashTable<V>::hashFunction(int key) {
    return key % _size;
}

template<class V>
void HashTable<V>::insert(int key, const V value) {
    if (_size == 0) {
        _size = 1;
        auto initHash = new AVLRankTree<int, Node> *[_size];
        for (int i = 0; i < _size; i++) initHash[i] = nullptr;


        init(initHash, key, value);
        _hashTable = initHash;
    } else {
        if (_counter == _size) resize(false);
        init(_hashTable, key, value);
    }
    ++_counter;
}

template<class V>
V HashTable<V>::getValue(int key) {
    auto tree = _hashTable[hashFunction(key)];
    if (!tree) { throw HashKeyDoesNotExist(); }
    return tree->getValue(key)->_value;
}

template<class V>
void HashTable<V>::resize(bool toShrink) {
    int prev_size = _size;

    if (toShrink) _size /= 2;
    else _size *= 2;

    auto new_hash = new AVLRankTree<int, Node> *[_size];
    for (int i = 0; i < _size; i++) { new_hash[i] = nullptr; }

    for (int i = 0; i < prev_size; i++) {

        auto tree = _hashTable[i];
        if (tree == nullptr) continue;

        auto array = tree->getValueSorted();
        for (int j = 0; j < tree->getSize(); ++j) {
            init(new_hash, array[j]->_key, array[j]->_value);
        }
        delete[] array;
    }
    for (int i = 0; i < prev_size; ++i) {
        if (_hashTable[i] != nullptr) { delete _hashTable[i]; }
    }
    delete[] _hashTable;

    _hashTable = new_hash;
}

template<class V>
bool HashTable<V>::includesKey(int key) {
    if (isEmpty()) { return false; }
    auto tree = _hashTable[hashFunction(key)];
    return !(!tree || tree->getSize() == EMPTY_SIZE || !tree->includes(key));
}

template<class V>
void HashTable<V>::remove(int key) {
    if (!includesKey(key)) return;
    auto tree = _hashTable[hashFunction(key)];
    tree->remove(key);
    _counter--;
    if (_size >= 40 && _counter == _size / 4) { resize(true); }
}

template<class V>
void HashTable<V>::init(AVLRankTree<int, Node> **array, int key, V value) {
    int index = hashFunction(key);

    AVLRankTree<int, Node> *newTree = nullptr;
    if (!array[index]) {
        newTree = new AVLRankTree<int, Node>();
        array[index] = newTree;
    }
    newTree = array[index];
    auto node = new Node(key, value);
    newTree->insert(key, node);
}

template<class V>
bool HashTable<V>::isEmpty() {
    return _size <= EMPTY_SIZE;
}

template<class V>
int HashTable<V>::getSize() {
    return isEmpty() ? EMPTY_SIZE : _size;
}

template<class V>
void HashTable<V>::destroyHash(ValueDestroyFunction *f) {
    for (int i = 0; i < _size; ++i) {
        if (_hashTable[i] != nullptr) {
            if (!_hashTable[i]->isEmpty()) {
                auto sortedKeys = _hashTable[i]->getKeySorted();
                for (int k = 0; k < _hashTable[i]->getSize(); ++k) {
                    auto key = sortedKeys[k];
                    auto value = _hashTable[i]->getValue(key)->_value;
                    f->operator()(value);
                }
                delete[] sortedKeys;
            }
            _hashTable[i]->destroy();
            delete _hashTable[i];
        }
    }
    if (_size > 0) delete[] _hashTable;
}
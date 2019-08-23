#ifndef AVLRankTree_H_
#define AVLRankTree_H_

#include <stdlib.h>
#include <exception>

#define DEFAULT_VALUE 0
#define DEFAULT_RANK 1

using namespace std;

class AvlKeyAlreadyExists : public exception {
};

class AvlKeyDoesNotExists : public exception {
};

class AvlIllegalInput : public exception {
};

template<class K, class V=int *>
class AVLRankTree {
private:
    struct AvlNode {
        K _key;
        V *_value;

        int _height;
        int _rank;

        AvlNode *_left;
        AvlNode *_right;
        AvlNode *_parent;

        int getBalance();
    };

    AvlNode *_root;
    int _size;

    static AvlNode **getSortedNodesArray(AvlNode **nodesArray, AvlNode *node);

    AvlNode *getNodeByKey(K key);

    AvlNode *treeFromSortedNodes(K *sortedKeysArray, V **sortedDataArray, int length, AvlNode *parent);

    void setRanks(AvlNode *root);

    void balance(AvlNode *node);

    void removeNode(AvlNode *node);

    void updateRanks(AvlNode *node);

    void llRotation(AvlNode *node);

    void rrRotation(AvlNode *node);

    void rlRotation(AvlNode *node);

    void lrRotation(AvlNode *node);

    void updateHeight(AvlNode *node);

    void destroy(AvlNode *node);

    void parentPointTo(AvlNode *child, AvlNode *newChild);

    void setTreeFromSortedNodes(K *sortedKeysArray, V **sortedDataArray, int length);

    static int getMergedSize(AvlNode **nodes1, int size1, AvlNode **nodes2, int size2);

    static AvlNode **
    getMergedArray(AVLRankTree::AvlNode **nodes1, int size1, AVLRankTree::AvlNode **nodes2, int size2,
                   int mergedSize);

public:
    AVLRankTree() : _root(nullptr), _size(0) {}

    virtual ~AVLRankTree();

    void insert(K key, V *data);

    void insert(K key);

    void remove(K key);

    void destroy();

    int getSize();

    int isEmpty();

    bool includes(K key);

    K *getKeySorted();

    V *getValue(K key);

    V **getValueSorted();

    // Tree values have to overload operator +.
    static AVLRankTree<K, V> *mergeTrees(AVLRankTree<K, V> *tree1, AVLRankTree<K, V> *tree2);

    AVLRankTree<K, V> *getCopy();
};

template<class K, class V>
int AVLRankTree<K, V>::AvlNode::getBalance() {
    int leftHeight = 0;
    int rightHeight = 0;

    if (_left) leftHeight = _left->_height;
    if (_right) rightHeight = _right->_height;

    return leftHeight - rightHeight;
}


template<class K, class V>
void AVLRankTree<K, V>::destroy() {
    destroy(_root);

    _size = 0;
    _root = nullptr;
}

template<class K, class V>
void AVLRankTree<K, V>::setTreeFromSortedNodes(K *sortedKeysArray, V **sortedDataArray, int length) {
    for (int i = 0; i < length - 1; i++) if (!(sortedKeysArray[i] < sortedKeysArray[i + 1])) throw AvlIllegalInput();
    destroy();
    _root = treeFromSortedNodes(sortedKeysArray, sortedDataArray, length, nullptr);
    setRanks(_root);
    _size = length;
}

template<class K, class V>
AVLRankTree<K, V>::~AVLRankTree() {
    destroy(_root);
    _root = nullptr;
}

template<class K, class V>
int AVLRankTree<K, V>::getSize() {
    return _size;
}

template<class K, class V>
void AVLRankTree<K, V>::insert(K key, V *data) {
    if (includes(key)) throw AvlKeyAlreadyExists();

    auto newNode = new AvlNode();
    newNode->_key = key;
    newNode->_value = data;

    newNode->_height = 1;
    newNode->_rank = DEFAULT_RANK;

    newNode->_left = nullptr;
    newNode->_right = nullptr;
    newNode->_parent = nullptr;

    if (_root == nullptr) _root = newNode;
    else {
        auto current = _root;
        AvlNode *parent = nullptr;

        while ((current != nullptr) && (current->_key != key)) {
            parent = current;
            current = key < current->_key ? current->_left : current->_right;
        }

        if (parent != nullptr) key < parent->_key ? parent->_left = newNode : parent->_right = newNode;
        newNode->_parent = parent;
        updateHeight(newNode);
        balance(newNode);

    }
    _size++;
    updateRanks(newNode);
}

template<class K, class V>
void AVLRankTree<K, V>::remove(K key) {
    AvlNode *node = getNodeByKey(key);
    if (node) removeNode(node);
    if (_root != nullptr) updateRanks(_root);
}

template<class K, class V>
V *AVLRankTree<K, V>::getValue(K key) {
    AvlNode *node = getNodeByKey(key);
    if (!node) throw AvlKeyDoesNotExists();
    return node->_value;
}

template<class K, class V>
bool AVLRankTree<K, V>::includes(K key) {
    return getNodeByKey(key) != nullptr;
}

template<class K, class V>
void AVLRankTree<K, V>::removeNode(AvlNode *node) {
    if (!(node->_left) && !(node->_right)) parentPointTo(node, nullptr);

    else if (!(node->_left) && (node->_right)) {
        parentPointTo(node, node->_right);
        node->_right->_parent = node->_parent;

    } else if ((node->_left) && !(node->_right)) {
        parentPointTo(node, node->_left);
        node->_left->_parent = node->_parent;

    } else {
        auto current = node->_right;

        while (current->_left) current = current->_left;

        K tempKey = current->_key;
        V *tempData = current->_value;

        current->_key = node->_key;
        node->_key = tempKey;

        current->_value = node->_value;
        node->_value = tempData;

        removeNode(current);

        return;
    }

    if (node->_parent) {
        updateHeight(node->_parent);
        updateRanks(node->_parent);
        balance(node->_parent);
    }

    delete node->_value;
    delete node;
    _size--;
}

template<class K, class T>
void AVLRankTree<K, T>::updateHeight(AvlNode *node) {
    while (node != nullptr) {
        int leftHeight = 0;
        int rightHeight = 0;

        if (node->_left) leftHeight = node->_left->_height;
        if (node->_right) rightHeight = node->_right->_height;

        node->_height = ((leftHeight > rightHeight) ? leftHeight : rightHeight) + 1;

        node = node->_parent;
    }
}

template<class K, class V>
void AVLRankTree<K, V>::updateRanks(AvlNode *node) {
    while (node != nullptr) {
        int leftRank = 0;
        int rightRank = 0;

        if (node->_left != nullptr) {
            leftRank = node->_left->_rank;
        }

        if (node->_right != nullptr) {
            rightRank = node->_right->_rank;
        }

        node->_rank = leftRank + rightRank + 1;
        node = node->_parent;
    }
}

template<class K, class T>
void AVLRankTree<K, T>::setRanks(AvlNode *root) {
    if (!root) return;

    int leftRank = 0;
    int rightRank = 0;

    if (root->_left) {
        setRanks(root->_left);
        leftRank = root->_left->_rank;
    }
    if (root->_right) {
        setRanks(root->_right);
        rightRank = root->_right->_rank;
    }

    updateRanks(root);
    root->_rank = leftRank + rightRank + 1;
}

template<class K, class T>
void AVLRankTree<K, T>::balance(AvlNode *node) {
    int factor = node->getBalance();
    if (factor >= 2) node->_left->getBalance() >= 0 ? llRotation(node) : lrRotation(node);
    else if (factor <= -2) node->_right->getBalance() > 0 ? rlRotation(node) : rrRotation(node);
    if (node->_parent) balance(node->_parent);
}

template<class K, class T>
void AVLRankTree<K, T>::llRotation(AvlNode *node) {
    auto parent = node->_parent;
    auto left = node->_left;

    parentPointTo(node, left);
    left->_parent = parent;

    auto leftRight = node->_left->_right;

    left->_right = node;
    node->_parent = left;
    node->_left = leftRight;

    if (leftRight) leftRight->_parent = node;

    updateHeight(node);
    updateRanks(node);
}

template<class K, class T>
void AVLRankTree<K, T>::rrRotation(AvlNode *node) {
    auto parent = node->_parent;
    auto right = node->_right;

    parentPointTo(node, right);
    right->_parent = parent;

    auto rightLeft = node->_right->_left;
    right->_left = node;
    node->_parent = right;
    node->_right = rightLeft;

    if (rightLeft) rightLeft->_parent = node;

    updateHeight(node);
    updateRanks(node);
}

template<class K, class T>
void AVLRankTree<K, T>::lrRotation(AvlNode *node) {
    rrRotation(node->_left);
    llRotation(node);
}

template<class K, class T>
void AVLRankTree<K, T>::rlRotation(AvlNode *node) {
    llRotation(node->_right);
    rrRotation(node);
}

template<class K, class T>
typename AVLRankTree<K, T>::AvlNode *AVLRankTree<K, T>::getNodeByKey(K key) {
    auto curr = _root;
    while (curr != nullptr && curr->_key != key) curr = key < curr->_key ? curr->_left : curr->_right;
    return curr;
}

template<class K, class T>
void AVLRankTree<K, T>::parentPointTo(AvlNode *child, AvlNode *newChild) {
    if (child->_parent == nullptr) {
        _root = newChild;
        updateRanks(_root);
    } else child->_parent->_left == child ? child->_parent->_left = newChild : child->_parent->_right = newChild;
}

template<class K, class T>
typename AVLRankTree<K, T>::AvlNode **
AVLRankTree<K, T>::getSortedNodesArray(AVLRankTree::AvlNode **nodesArray, AVLRankTree::AvlNode *node) {
    if (node == nullptr) return nodesArray;
    nodesArray = getSortedNodesArray(nodesArray, node->_left);
    *nodesArray = node;
    nodesArray++;
    return getSortedNodesArray(nodesArray, node->_right);
}

template<class K, class T>
T **AVLRankTree<K, T>::getValueSorted() {
    auto sortedNodes = new AvlNode *[getSize()];
    getSortedNodesArray(sortedNodes, _root);

    auto sortedValues = new T *[getSize()];
    for (int i = 0; i < getSize(); ++i) sortedValues[i] = sortedNodes[i]->_value;

    delete[] sortedNodes;
    return sortedValues;
}

template<class K, class T>
K *AVLRankTree<K, T>::getKeySorted() {
    auto sortedNodes = new AvlNode *[getSize()];
    getSortedNodesArray(sortedNodes, _root);

    auto sortedValues = new K[getSize()];
    for (int i = 0; i < getSize(); ++i) sortedValues[i] = sortedNodes[i]->_key;

    delete[] sortedNodes;
    return sortedValues;
}

template<class K, class T>
void AVLRankTree<K, T>::destroy(AvlNode *node) {
    if (node == nullptr) return;
    destroy(node->_left);
    destroy(node->_right);
    delete node->_value;
    delete node;
}

template<class K, class T>
typename AVLRankTree<K, T>::AvlNode *
AVLRankTree<K, T>::treeFromSortedNodes(K *sortedKeysArray, T **sortedDataArray, int length, AvlNode *parent) {

    if (length == 0) return nullptr;

    auto node = new AvlNode();

    int pos = length / 2;
    node->_key = sortedKeysArray[pos];
    node->_value = sortedDataArray[pos];
    node->_parent = parent;

    node->_left = treeFromSortedNodes(sortedKeysArray, sortedDataArray, pos, node);

    updateRanks(node);
    updateRanks(node->_left);

    node->_right = treeFromSortedNodes(sortedKeysArray + pos + 1, sortedDataArray + pos + 1, length - pos - 1, node);

    updateRanks(node);
    updateRanks(node->_right);

    if (!node->_left && !node->_right) node->_height = 1;

    else if (!node->_left) node->_height = node->_right->_height + 1;

    else if (!node->_right) node->_height = node->_left->_height + 1;

    else node->_height = (node->_left->_height > node->_right->_height) ? node->_left->_height : node->_right->_height;

    return node;
}

template<class K, class V>
int AVLRankTree<K, V>::isEmpty() {
    return getSize() <= 0;
}


template<class K, class V>
AVLRankTree<K, V> *AVLRankTree<K, V>::mergeTrees(AVLRankTree<K, V> *tree1, AVLRankTree<K, V> *tree2) {
    if (!tree1 && !tree2) return nullptr;
    else if (!tree1 || tree1->isEmpty()) return tree2->getCopy();
    else if (!tree2 || tree2->isEmpty()) return tree1->getCopy();

    auto sortedNodes1 = new AvlNode *[tree1->_size];
    auto sortedNodes2 = new AvlNode *[tree2->_size];

    // From Small to Big
    getSortedNodesArray(sortedNodes1, tree1->_root);
    getSortedNodesArray(sortedNodes2, tree2->_root);

    int size1 = tree1->getSize();
    int size2 = tree2->getSize();


    int mergedSize = getMergedSize(sortedNodes1, size1, sortedNodes2, size2);
    auto mergedArray = getMergedArray(sortedNodes1, size1, sortedNodes2, size2, mergedSize);

    delete[] sortedNodes1;
    delete[] sortedNodes2;

    // Work on merged tree
    auto mergedTree = new AVLRankTree<K, V>();
    mergedTree->_size = mergedSize;

    auto sortedKeys = new K[mergedSize];
    auto sortedData = new V *[mergedSize];

    for (int k = 0; k < mergedSize; k++) {
        sortedKeys[k] = K(mergedArray[k]->_key);
        sortedData[k] = new V(*(mergedArray[k]->_value));
        delete mergedArray[k]->_value;
        delete mergedArray[k];
    }
    delete[] mergedArray;


    mergedTree->setTreeFromSortedNodes(sortedKeys, sortedData, mergedSize);

    delete[] sortedKeys;
    delete[] sortedData;
    return mergedTree;
}

template<class K, class V>
void AVLRankTree<K, V>::insert(K key) {
    insert(key, DEFAULT_VALUE);
}

template<class K, class V>
int AVLRankTree<K, V>::getMergedSize(AvlNode **nodes1, int size1, AvlNode **nodes2, int size2) {
    int c1 = 0;
    int c2 = 0;
    int total = 0;

    while (c1 < size1 || c2 < size2) {
        if (c1 < size1 && c2 < size2) {
            K key1 = nodes1[c1]->_key;
            K key2 = nodes2[c2]->_key;
            if (key1 < key2) c1++;
            else if (key1 > key2) c2++;
            else {
                c1++;
                c2++;
            }

        } else if (c1 < size1) c1++;
        else c2++;
        total++;
    }

    return total;
}

template<class K, class V>
typename AVLRankTree<K, V>::AvlNode **
AVLRankTree<K, V>::getMergedArray(AVLRankTree::AvlNode **nodes1,
                                  int size1,
                                  AVLRankTree::AvlNode **nodes2,
                                  int size2,
                                  int mergedSize) {
    auto mergedArray = new AvlNode *[mergedSize];

    for (int i = 0, c1 = 0, c2 = 0; i < mergedSize; ++i) {
        auto node = new AvlNode();
        node->_value = new V();

        if (c1 < size1 && c2 < size2) {
            auto node1 = nodes1[c1];
            auto node2 = nodes2[c2];
            if (node1->_key < node2->_key) {
                node->_key = node1->_key;
                *(node->_value) = *(node1->_value);
                c1++;
            } else if (node1->_key > node2->_key) {
                node->_key = node2->_key;
                *(node->_value) = *(node2->_value);
                c2++;
            } else { // node1._key == node2._key
                node->_key = node1->_key;

                // Overloaded operator +.
                *(node->_value) = *(node1->_value) + *(node2->_value);

                c1++;
                c2++;
            }
        } else if (c1 < size1) {
            auto node1 = nodes1[c1];
            node->_key = node1->_key;
            *(node->_value) = *(node1->_value);
            c1++;
        } else {
            auto node2 = nodes2[c2];
            node->_key = node2->_key;
            *(node->_value) = *(node2->_value);
            c2++;
        }
        mergedArray[i] = node;
    }
    return mergedArray;
}

template<class K, class V>
AVLRankTree<K, V> *AVLRankTree<K, V>::getCopy() {
    auto sortedKeys = this->getKeySorted();
    auto sortedValues = this->getValueSorted();

    auto keysCopies = new K[_size];
    auto valuesCopies = new V *[_size];

    for (int i = 0; i < _size; i++) {
        keysCopies[i] = K(sortedKeys[i]);
        valuesCopies[i] = new V(*(sortedValues[i]));
    }

    delete[] sortedKeys;
    delete[] sortedValues;

    auto root = this->treeFromSortedNodes(keysCopies, valuesCopies, _size, nullptr);

    delete[] keysCopies;
    delete[] valuesCopies;

    auto newTree = new AVLRankTree<K, V>();

    newTree->_root = root;
    newTree->_size = _size;
    return newTree;
}

#endif /* AVLRankTree_H_ */

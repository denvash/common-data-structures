#ifndef AVLRankTree_H_
#define AVLRankTree_H_

#include <iostream>
#include <assert.h>
#include <stdlib.h>
#include <exception>

using namespace std;

class AvlKeyAlreadyExist : public exception {
};

class AvlKeyDoesNotExist : public exception {
};

using namespace std;

template<class K, class T>
class AVLRankTree {
private:
    struct Node {
        K key;
        T data;
        int height = 0;
        int rank = -1;
        K maxDataKey;
        T maxData;
        Node *left;
        Node *right;
        Node *parent;


        int getBalance() {
            int leftHeight = 0, rightHeight = 0;
            if (left) {
                leftHeight = left->height;
            }
            if (right) {
                rightHeight = right->height;
            }
            return leftHeight - rightHeight;
        }
    };

    Node *root;
    int size;

    void RemoveNode(Node *node);

    void updateHeights(Node *node);

    void updateRanks(Node *node);

    void setAllRanks(Node *subroot);

    void balance(Node *node);

    void llRotation(Node *node);

    void rrRotation(Node *node);

    void rlRotation(Node *node);

    void lrRotation(Node *node);

    Node *getNode(K key);

    void ParentPointTo(Node *child, Node *newChild);

    void print2(Node *nodeToPrint, int level);

    // can't merge next functions because of return type
    T *getDataSortedArray2(T *array, Node *node);

    K *getKeySortedArray2(K *array, Node *node);

    void destroy2(Node *node);

    Node *LoadSortedArray2(K *sortedKeysArray, T *sortedDataArray, int length,
                           Node *parent);

public:
    AVLRankTree();

    virtual ~AVLRankTree();
    
    int getSize();

    int isEmpty();

    void insert(K key, T data);

    void remove(K key);

    T getByKey(K key);

    bool containes(K key);

    K *getKeySortedArray();

    T getMaxDataKey();

    void print();
};

template<class K, class T>
AVLRankTree<K, T>::AVLRankTree() {
    root = NULL;
    size = 0;
}

template<class K, class T>
AVLRankTree<K, T>::~AVLRankTree() {
    destroy2(root); // release allocated memory
    root = NULL;
}

// Time complexity: O(1)
template<class K, class T>
int AVLRankTree<K, T>::getSize() {
    return this->size;
}

// Time complexity: O(log(n))
template<class K, class T>
void AVLRankTree<K, T>::insert(K key, T data) {
    if (containes(key)) {
        throw AvlKeyAlreadyExist();
    }

    // Create the node
    Node *new_node = new Node();
    new_node->height = 1; // height of the subtree with this node as root.
    new_node->rank = 1;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->parent = NULL;
    new_node->key = key;
    new_node->data = data;

    // choose where to add the node and add it.
    if (root == NULL) { // tree is empty
        root = new_node;
        this->size++;
        updateRanks(new_node);
        return;
    } else { // tree is not empty.
        Node *current = root;
        Node *parent = NULL;

        while ((current != NULL) && (current->key != key)) {
            parent = current;
            if (key < current->key) { // left subtree
                current = current->left;
            } else { // right subtree
                current = current->right;
            }
        }

        // add to tree

        if (parent != NULL) {
            if (key < parent->key) { parent->left = new_node; }
            else {
                parent->right = new_node;
            }
        }

        new_node->parent = parent;

        updateHeights(new_node);
        updateRanks(new_node);
        this->balance(new_node);

        this->size++;

        return;
    }
}

// Time complexity: O(log(n))
template<class K, class T>
void AVLRankTree<K, T>::remove(K key) {
    Node *node = getNode(key);
    if (node) {
        RemoveNode(node);
    }
}

// Time complexity: log(n)
template<class K, class T>
T AVLRankTree<K, T>::getByKey(K key) {
    Node *node = getNode(key);
    if (!node)
        throw AvlKeyDoesNotExist();
    return node->data;
}

// Time complexity: log(n)
template<class K, class T>
bool AVLRankTree<K, T>::containes(K key) {
    return (getNode(key) != NULL);
}

template<class K, class T>
K *AVLRankTree<K, T>::getKeySortedArray() {
    K *result = new K[this->size];

    getKeySortedArray2(result, root); // placing the tree in "result"
    return result;
}

// printing the tree. using the recursion print2
template<class K, class T>
void AVLRankTree<K, T>::print() {
    print2(root, 1);
}

template<class K, class T>
void AVLRankTree<K, T>::RemoveNode(Node *node) {
    // if leaf
    if (!(node->left) && !(node->right)) {
        ParentPointTo(node, NULL);
        if (node->parent) {
            updateHeights(node->parent);
            updateRanks(node->parent);
            this->balance(node->parent);
        }
        this->size--;
        delete node;
        return;
    }
        // if only right child
    else if (!(node->left) && (node->right)) {
        ParentPointTo(node, node->right);
        node->right->parent = node->parent;

        if (node->parent) {
            updateHeights(node->parent);
            updateRanks(node->parent);
            this->balance(node->parent);
        }
        delete node;
        this->size--;
        return;
    }

        // if only left child
    else if ((node->left) && !(node->right)) {
        ParentPointTo(node, node->left);
        node->left->parent = node->parent;

        if (node->parent) {
            updateHeights(node->parent);
            updateRanks(node->parent);
            this->balance(node->parent);
        }
        delete node;
        this->size--;
        return;
    }

        // if node has 2 children
    else {
        // find new root for subtree
        Node *current = node->right;
        while (current->left)
            current = current->left;

        // switch current and node.
        K backupK = current->key;
        current->key = node->key;
        node->key = backupK;

        T backupT = current->data;
        current->data = node->data;
        node->data = backupT;

        RemoveNode(current); // recursive call. will happen only once, because now node doesn't have 2 children.
    }
}

// Updating the height property of the node and his ancestors.
template<class K, class T>
void AVLRankTree<K, T>::updateHeights(Node *node) {
    Node *current = node;
    while (current != NULL) {
        int leftHeight = 0, rightHeight = 0;
        if (current->left) {
            leftHeight = current->left->height;
        }
        if (current->right) {
            rightHeight = current->right->height;
        }
        current->height =
                ((leftHeight > rightHeight) ? leftHeight : rightHeight) + 1;
        current = current->parent;
    }
}

// Updating the height property of the node and his ancestors.
template<class K, class T>
void AVLRankTree<K, T>::updateRanks(Node *node) {
    Node *current = node;
    while (current != NULL) {
        int leftRank = 0, rightRank = 0;
        K max_key = current->key;
        T max_data = current->data;

        if (current->left) {
            leftRank = current->left->rank;
            if (current->left->maxData > max_data){
                max_key = current->left->maxDataKey;
                max_data = current->left->maxData;
            }
            if (current->left->maxData == max_data){
                max_key = (current->left->maxDataKey > max_key) ? current->left->maxDataKey : max_key;
            }

        }

        if (current->right) {
            rightRank = current->right->rank;
            if (current->right->maxData > max_data){
                max_key = current->right->maxDataKey;
                max_data = current->right->maxData;
            }
            if (current->right->maxData == max_data){
                max_key = (current->right->maxDataKey > max_key) ? current->right->maxDataKey : max_key;
            }
        }

        current->rank = leftRank + rightRank + 1;
        current->maxDataKey = max_key;
        current->maxData = max_data;
        current = current->parent;
    }
}

//running post order and setting the ranks of the entire tree
template<class K, class T>
void AVLRankTree<K, T>::setAllRanks(Node *subroot) {
    if (!subroot)
        return; // empty tree , only if root==null
    int leftRank = 0;
    int rightRank = 0;

    K max_key = subroot->key;
    T max_data = subroot->data;

    if (subroot->left) {
        setAllRanks(subroot->left);
        leftRank = subroot->left->rank;

        if (subroot->left->maxData > max_data){
            max_key = subroot->left->maxDataKey;
            max_data = subroot->left->maxData;
        }
        if (subroot->left->maxData == max_data){
            max_key = (subroot->left->maxDataKey > max_key) ? subroot->left->maxDataKey : max_key;
        }
    }
    if (subroot->right) {
        setAllRanks(subroot->right);
        rightRank = subroot->right->rank;

        if (subroot->right->maxData > max_data){
            max_key = subroot->right->maxDataKey;
            max_data = subroot->right->maxData;
        }
        if (subroot->right->maxData == max_data){
            max_key = (subroot->right->maxDataKey > max_key) ? subroot->right->maxDataKey : max_key;
        }
    }
    subroot->rank = leftRank + rightRank + 1;
}

template<class K, class T>
void AVLRankTree<K, T>::balance(Node *node) {
    int balanceFactor = node->getBalance();
    if (balanceFactor >= 2) { //left Heavy
        //assert(node->left->getBalance() != 0);
        if (node->left->getBalance() >= 0) {
            llRotation(node);
        } else {
            lrRotation(node);
        }
    } else if (balanceFactor <= -2) { //left Heavy
        //assert(node->right->getBalance() != 0);
        if (node->right->getBalance() > 0) {
            rlRotation(node);
        } else {
            rrRotation(node);
        }
    }

    if (node->parent) {
        balance(node->parent);
    }

}

// node: the node with the |balance|>=2
template<class K, class T>
void AVLRankTree<K, T>::llRotation(Node *node) {
    Node *parent = node->parent;
    Node *lChild = node->left;

    // parent-node relation
    ParentPointTo(node, lChild);
    lChild->parent = parent;

    Node *lrChild = node->left->right; //backup
    // node-child relation
    lChild->right = node;
    node->parent = lChild;
    // restore lost chain
    node->left = lrChild;
    if (lrChild)
        lrChild->parent = node;

    updateHeights(node);
    updateRanks(node);
}

// node: the node with the |balance|>=2
template<class K, class T>
void AVLRankTree<K, T>::rrRotation(Node *node) {
    Node *parent = node->parent;
    Node *rChild = node->right;

    // parent-node relation
    ParentPointTo(node, rChild);
    rChild->parent = parent;

    Node *rlChild = node->right->left; //backup
    // node-child relation
    rChild->left = node;
    node->parent = rChild;
    // restore lost chain
    node->right = rlChild;
    if (rlChild)
        rlChild->parent = node;

    updateHeights(node);
    updateRanks(node);
}

// node: the node with the |balance|>=2
template<class K, class T>
void AVLRankTree<K, T>::lrRotation(Node *node) {
    rrRotation(node->left);
    llRotation(node);
}

// node: the node with the |balance|>=2
template<class K, class T>
void AVLRankTree<K, T>::rlRotation(Node *node) {
    llRotation(node->right);
    rrRotation(node);
}

template<class K, class T>
typename AVLRankTree<K, T>::Node *AVLRankTree<K, T>::getNode(K key) {
    Node *current = root;
    while ((current != NULL) && (current->key != key)) { // while wasn't placed yet
        if (key < current->key) { // left subtree
            current = current->left;
        } else { // right subtree
            current = current->right;
        }
    }
    return current;
}

template<class K, class T>
void AVLRankTree<K, T>::ParentPointTo(Node *child, Node *newChild) {
    if (child->parent == NULL)
        root = newChild;
    else {
        if (child->parent->left == child) {
            child->parent->left = newChild;
        } else {
            child->parent->right = newChild;
        }

    }
}

template<class K, class T>
void AVLRankTree<K, T>::print2(Node *nodeToPrint, int level) {
    int i;
    if (nodeToPrint) {
        print2(nodeToPrint->right, level + 1);
        cout << "\n";
        if (nodeToPrint == root) {
            cout << "Root ->";
        }

        for (i = 0; i < level && nodeToPrint != root; i++) {
            cout << "       ";
        }
        cout << nodeToPrint->key;
        print2(nodeToPrint->left, level + 1);
    }
}

// placing the subtree starts with node in the array, and return pointer to the next place in the array
template<class K, class T>
T *AVLRankTree<K, T>::getDataSortedArray2(T *array, Node *node) {
    if (node == NULL)
        return array;
    array = getDataSortedArray2(array, node->left);
    *array = node->data;
    array++;
    return getDataSortedArray2(array, node->right);
}

template<class K, class T>
K *AVLRankTree<K, T>::getKeySortedArray2(K *array, Node *node) {
    if (node == NULL)
        return array;
    array = getKeySortedArray2(array, node->left);
    *array = node->key;
    array++;
    return getKeySortedArray2(array, node->right);
}

// destroying the array, post order
template<class K, class T>
void AVLRankTree<K, T>::destroy2(Node *node) {
    if (node == NULL)
        return;
    destroy2(node->left);
    destroy2(node->right);
    delete node;
}

template<class K, class T>
typename AVLRankTree<K, T>::Node *AVLRankTree<K, T>::LoadSortedArray2(
        K *sortedKeysArray, T *sortedDataArray, int length, Node *parent) {
    if (length == 0)
        return NULL;
    Node *node = new Node();

    int pos = length / 2;
    node->key = sortedKeysArray[pos];
    node->data = sortedDataArray[pos];
    node->parent = parent;
    node->left = LoadSortedArray2(sortedKeysArray, sortedDataArray, pos, node);
    node->right = LoadSortedArray2(sortedKeysArray + pos + 1,
                                   sortedDataArray + pos + 1, length - pos - 1, node);

    // find the height
    if (!node->left && !node->right) // no child
        node->height = 1;
    else if (!node->left) // right child only
        node->height = node->right->height + 1;
    else if (!node->right) // left child only
        node->height = node->left->height + 1;
    else
        node->height =
                (node->left->height > node->right->height) ?
                node->left->height : node->right->height; // both children exist

    return node;
}

template<class K, class T>
T AVLRankTree<K, T>::getMaxDataKey() {
    return root->maxDataKey;
}

template<class K, class T>
int AVLRankTree<K, T>::isEmpty() {
    return getSize() <= 0;
}

#endif /* AVLRankTree_H_ */
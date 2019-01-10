#ifndef AvlTree_h
#define AvlTree_h


#include<iostream>
#include <exception>

using namespace std;

class AvlExceptions : public exception {
};

class AvlKeyAlreadyExists : public AvlExceptions {

};

class AvlKeyDoesNotExists : public AvlExceptions {

};

class AvlTreeIsEmpty : public AvlExceptions {
};

template<class T, class K = int>
struct AvlNode {
    T key;
    K value;
    AvlNode *left;
    AvlNode *right;
    int height;
};

template<class Key, class Value = int>
class AVLTree {

    AvlNode<Key, Value> *root;
    int size;
    Key highest_key;
    Key lowest_key;

    typedef enum {
        RIGHT,
        LEFT
    } RotateMode;

    typedef enum {
        MIN,
        MAX
    } FindMode;

    AvlNode<Key, Value> *avlInsert(Key key, Value &value, AvlNode<Key, Value> *node);

    AvlNode<Key, Value> *avlInsert(Key key, AvlNode<Key, Value> *node);

    AvlNode<Key, Value> *singleRotate(AvlNode<Key, Value> *node, RotateMode mode);

    AvlNode<Key, Value> *doubleRotate(AvlNode<Key, Value> *&node, RotateMode mode);

    AvlNode<Key, Value> *findWithMode(AvlNode<Key, Value> *node, FindMode mode);

    AvlNode<Key, Value> *avlRemove(Key &x, AvlNode<Key, Value> *node);

    AvlNode<Key, Value> *findNode(AvlNode<Key, Value> *node, Key key);

    Key &avlGetHighestKey();

    Key &avlGetLowestKey();

    int height(AvlNode<Key, Value> *node);

    void displayInOrder(AvlNode<Key, Value> *t);

    void treeToValueArray(AvlNode<Key, Value> *root, Value *array, int *index) const;

    void treeToKeyArray(AvlNode<Key, Value> *root, Key **keys, int *index);

    void AvlDisplay(AvlNode<Key, Value> *node, int level);

    void destroy(AvlNode<Key, Value> *node);

public:

    AVLTree() : root(), size(0), highest_key(-1), lowest_key(-1) {
        root = nullptr;
    }

    ~AVLTree() {
        if (root == nullptr) return;
        destroy(root);
    }

    void insert(Key key, Value &value);

    void insert(Key key);

    void remove(Key x);

    int getHeight();

    bool isEmpty() const;

    bool contains(Key key);

    int getSize() const;

    Value &getValue(Key key);

    Value *getValueSorted() const;

    Key &getBestKey();

    Key &getMin();

    Key **getKeySorted();

    void deleteByNode(void *node);

    void destroy();

    AvlNode<Key, Value> *findNodeByKey(int key);
};

template<class Key, class Value>
AvlNode<Key, Value> *AVLTree<Key, Value>::avlInsert(Key key, Value &value, AvlNode<Key, Value> *node) {
    if (node == nullptr) {
        node = new AvlNode<Key, Value>();
        node->key = key;
        node->value = value;
        node->height = 0;
        node->left = nullptr;
        node->right = nullptr;
        node->height = max(height(node->left), height(node->right)) + 1;
        size++;
        return node;
    } else if (key < node->key) {
        node->left = avlInsert(key, value, node->left);
        if (height(node->left) - height(node->right) == 2) {
            if (key < node->left->key)
                node = singleRotate(node, RIGHT);
            else
                node = doubleRotate(node, RIGHT);
        }
    } else if (key > node->key) {
        node->right = avlInsert(key, value, node->right);
        if (height(node->right) - height(node->left) == 2) {
            if (key > node->right->key)
                node = singleRotate(node, LEFT);
            else
                node = doubleRotate(node, LEFT);
        }
    } else if (key == node->key) {
        throw AvlKeyAlreadyExists();
    }
    node->height = max(height(node->left), height(node->right)) + 1;

    return node;
}

template<class Key, class Value>
void AVLTree<Key, Value>::insert(Key key, Value &value) {
    root = avlInsert(key, value, root);
    highest_key = avlGetHighestKey();
    lowest_key = avlGetLowestKey();
}

template<class Key, class Value>
void AVLTree<Key, Value>::insert(Key key) {
    root = avlInsert(key, root);
    highest_key = avlGetHighestKey();
    lowest_key = avlGetLowestKey();
}

template<class Key, class Value>
void AVLTree<Key, Value>::remove(Key x) {
    if (findNode(root, x) == NULL) {
        throw AvlKeyDoesNotExists();
    }
    root = avlRemove(x, root);
    if (--size <= 0) {
        highest_key = -1;
        lowest_key = -1;
        return;
    }
    highest_key = avlGetHighestKey();
    lowest_key = avlGetLowestKey();
}

template<class Key, class Value>
int AVLTree<Key, Value>::getHeight() {
    return this->root->height;
}

template<class Key, class Value>
bool AVLTree<Key, Value>::isEmpty() const {
    return getSize() <= 0;
}

template<class Key, class Value>
bool AVLTree<Key, Value>::contains(Key key) {
    try {
        getValue(key);
    } catch (const AvlKeyDoesNotExists &e) {
        return false;
    }
    return true;
}

template<class Key, class Value>
int AVLTree<Key, Value>::getSize() const {
    return size;
}

template<class Key, class Value>
Value &AVLTree<Key, Value>::getValue(Key key) {

    AvlNode<Key, Value> *node = findNode(root, key);
    if (node == nullptr) {
        throw AvlKeyDoesNotExists();
    }
    return node->value;
}

template<class Key, class Value>
Value *AVLTree<Key, Value>::getValueSorted() const {

    if (isEmpty()) return nullptr;

    auto sorted_values = new Value[getSize()];

    int index = 0;

    try {
        treeToValueArray(root, sorted_values, &index);
    } catch (const AvlTreeIsEmpty &e) {
        return nullptr;
    }
    return sorted_values;
}

template<class Key, class Value>
Key &AVLTree<Key, Value>::getBestKey() {
    if (isEmpty())
        throw AvlKeyDoesNotExists();
    return highest_key;
}

template<class Key, class Value>
Key &AVLTree<Key, Value>::getMin() {
    if (isEmpty())
        throw AvlKeyDoesNotExists();
    return lowest_key;
}

template<class Key, class Value>
Key **AVLTree<Key, Value>::getKeySorted() {
    if (isEmpty())
        throw AvlTreeIsEmpty();

    auto sorted_keys = new Key *[getSize()];

    int index = 0;
    treeToKeyArray(root, sorted_keys, &index);
    return sorted_keys;
}

template<class Key, class Value>
void AVLTree<Key, Value>::deleteByNode(void *node) {
    auto *avlNode = (AvlNode<Key, Value> *) node;
    remove(avlNode->key);
}

template<class Key, class Value>
void AVLTree<Key, Value>::destroy() {
    destroy(root);
    size = 0;
    highest_key = -1;
    lowest_key = -1;
    root = nullptr;
}

template<class Key, class Value>
AvlNode<Key, Value> *AVLTree<Key, Value>::findNodeByKey(int key) {
    return findNode(root, key);
}

template<class Key, class Value>
AvlNode<Key, Value> *AVLTree<Key, Value>::avlInsert(Key key, AvlNode<Key, Value> *node) {
    if (node == nullptr) {
        node = new AvlNode<Key, Value>;
        node->key = key;
        node->value = -1;
        node->height = 0;
        node->left = nullptr;
        node->right = nullptr;
        node->height = max(height(node->left), height(node->right)) + 1;
        size++;
        return node;
    } else if (key < node->key) {
        node->left = avlInsert(key, node->left);
        if (height(node->left) - height(node->right) == 2) {
            if (key < node->left->key)
                node = singleRotate(node, RIGHT);
            else
                node = doubleRotate(node, RIGHT);
        }
    } else if (key > node->key) {
        node->right = avlInsert(key, node->right);
        if (height(node->right) - height(node->left) == 2) {
            if (key > node->right->key)
                node = singleRotate(node, LEFT);
            else
                node = doubleRotate(node, LEFT);
        }
    } else if (key == node->key) {
        throw AvlKeyAlreadyExists();
    }
    node->height = max(height(node->left), height(node->right)) + 1;

    return node;
}

template<class Key, class Value>
AvlNode<Key, Value> *AVLTree<Key, Value>::singleRotate(AvlNode<Key, Value> *node, AVLTree::RotateMode mode) {
    if (!node || !node->left || !node->right)
        return node;
    AvlNode<Key, Value> *u = mode == RIGHT ? node->left : node->right;
    if (!u) { return u; }

    if (mode == RIGHT) {
        node->left = u->right;
        u->right = node;
    } else {
        node->right = u->left;
        u->left = node;
    }

    node->height = max(height(node->left), height(node->right)) + 1;

    u->height = mode == RIGHT ? max(height(u->left), node->height) + 1 :
                max(height(node->right), node->height) + 1;
    return u;
}

template<class Key, class Value>
AvlNode<Key, Value> *AVLTree<Key, Value>::doubleRotate(AvlNode<Key, Value> *&node, AVLTree::RotateMode mode) {
    if (!node) { return nullptr; }
    if (mode == LEFT) {
        node->right = singleRotate(node->right, RIGHT);
        return singleRotate(node, LEFT);
    }
    node->left = singleRotate(node->left, LEFT);
    return singleRotate(node, RIGHT);
}

template<class Key, class Value>
AvlNode<Key, Value> *AVLTree<Key, Value>::findWithMode(AvlNode<Key, Value> *node, AVLTree::FindMode mode) {
    if (node == nullptr) { return nullptr; }
    if (mode == MIN) {
        if (node->left == nullptr) { return node; }
        return findWithMode(node->left, MIN);
    } else {
        if (node->right == nullptr) { return node; }
        return findWithMode(node->right, MAX);
    }
}

template<class Key, class Value>
AvlNode<Key, Value> *AVLTree<Key, Value>::avlRemove(Key &x, AvlNode<Key, Value> *node) {
    AvlNode<Key, Value> *temp;

    if (node == nullptr) { return nullptr; }

    else if (x < node->key) { node->left = avlRemove(x, node->left); }
    else if (x > node->key) { node->right = avlRemove(x, node->right); }

        //with 0 child
    else if (node->right == nullptr && node->left == nullptr) {
        delete node;
        node = nullptr;
        return node;
    }

        //with 1 child from right
    else if (node->left == NULL) {
        temp = node;
        node = node->right;
        delete temp;
        return node;
    }
        //with 1 child from left
    else if (node->right == NULL) {
        temp = node;
        node = node->left;
        delete temp;
        return node;
    }

        // Element found - with 2 children
    else if (node->left && node->right) {
        temp = findWithMode(node->right, MIN);
        node->key = temp->key;
        node->value = temp->value;
        node->right = avlRemove(temp->key, node->right);
        return node;
    }

    node->height = max(height(node->left), height(node->right)) + 1;

    // If AvlNode is unbalanced

    // If left AvlNode is deleted, right case
    if (height(node->left) - height(node->right) == 2) {
        // right right case
        if (height(node->left->left) - height(node->left->right) == 1)
            return singleRotate(node, LEFT);
            // right left case
        else
            return doubleRotate(node, LEFT);
    }
        // If right AvlNode is deleted, left case
    else if (height(node->right) - height(node->left) == 2) {
        // left left case
        if (height(node->right->right) - height(node->right->left) == 1)
            return singleRotate(node, RIGHT);
            // left right case
        else
            return doubleRotate(node, RIGHT);
    }
    return node;
}

template<class Key, class Value>
AvlNode<Key, Value> *AVLTree<Key, Value>::findNode(AvlNode<Key, Value> *node, Key key) {
    if (node == nullptr) return node;
    else if (key < node->key) {
        return findNode(node->left, key);
    } else if (key > node->key) {
        return findNode(node->right, key);
    }
    return node;
}

template<class Key, class Value>
Key &AVLTree<Key, Value>::avlGetHighestKey() {
    return findWithMode(root, MAX)->key;
}

template<class Key, class Value>
Key &AVLTree<Key, Value>::avlGetLowestKey() {
    return findWithMode(root, MIN)->key;
}

template<class Key, class Value>
int AVLTree<Key, Value>::height(AvlNode<Key, Value> *node) {
    return (node == nullptr ? -1 : node->height);
}

template<class Key, class Value>
void AVLTree<Key, Value>::displayInOrder(AvlNode<Key, Value> *t) {
    if (t == nullptr) { return; }
    displayInOrder(t->left);
    cout << t->key << " ";
    displayInOrder(t->right);
}

template<class Key, class Value>
void AVLTree<Key, Value>::treeToValueArray(AvlNode<Key, Value> *root, Value *array, int *index) const {
    if (root == nullptr) return;
    treeToValueArray(root->right, array, index);
    array[(*index)++] = root->value;
    treeToValueArray(root->left, array, index);
}

template<class Key, class Value>
void AVLTree<Key, Value>::treeToKeyArray(AvlNode<Key, Value> *root, Key **keys, int *index) {
    if (root == nullptr) { return; }
    treeToKeyArray(root->left, keys, index);
    keys[(*index)++] = &(root->key);
    treeToKeyArray(root->right, keys, index);
}

template<class Key, class Value>
void AVLTree<Key, Value>::AvlDisplay(AvlNode<Key, Value> *node, int level) {
    if (node != nullptr) {
        AvlDisplay(node->right, level + 1);
        printf("\n");
        if (node == root)
            cout << "root -> ";
        for (int i = 0; i < level && node != root; i++)
            cout << "        ";
        cout << node->value;
        AvlDisplay(node->left, level + 1);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::destroy(AvlNode<Key, Value> *node) {
    if (node == nullptr)
        return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}


#endif //AvlTree_h

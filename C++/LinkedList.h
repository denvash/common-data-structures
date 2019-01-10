#ifndef LinkedList_h
#define LinkedList_h

#include <stddef.h>

template<class T>
struct ListNode {
    T data;
    ListNode<T> *next;
    ListNode<T> *prev;
};

template<typename T>
class LinkedList {

protected:
    int _size;
    ListNode<T> *root;
    ListNode<T> *last;

    ListNode<T> *lastNodeGot;
    int lastIndexGot;
    bool isCached;

    ListNode<T> *getNode(int index);

    ListNode<T> *findEndOfSortedString(ListNode<T> *p, int (*cmp)(T &, T &));

public:
    LinkedList();

    ~LinkedList();

    virtual int getSize();

    virtual bool add(int index, T);

    virtual bool insertLast(T);

    virtual bool insertFirst(T);

    /**
     * Set the object T at index.
     * @param index, T
     * @return boolean success status
     */
    virtual bool set(int index, T);


    /**
     * Remove object at index
     * @param index
     * @return The value of removed key or default value if key not exists.
     */
    virtual T remove(int index);

    /**
     * Remove node from list
     * @param listNode
     * @return The value of removed node or default value if key not exists.
     */
    virtual T remove(ListNode<T> * listNode);

    /**
     * Remove last node
     * @return Last node data
     */
    virtual T pop();

    /**
     * Remove first node
     * @return First node data
     */
    virtual T removeFirst();


    /**
     * Get the index'th element on the list;
     * @param index - node index
     * @return Key's value or default value.
     */
    virtual T get(int index);

    /**
     * Get the last node in the linked-list
     * @return Last node
     */
    virtual ListNode<T> *getLastNode();

    /**
     * Get the first node in the linked-list
     * @return First node
     */
    virtual ListNode<T> *getFirstNode();

    virtual void clear();


    /**
     * Sort the list, given a comparison function
     * @param cmp
     */
    virtual void sort(int (*cmp)(T &, T &));


    /**
     * Find by predicate on function-object
     Function Object example:
         class CmpKey {
            private:
                int key;
            public:
                explicit CmpKey(int key) : key(key) {}

                bool operator()(ListEntry a) {
                    return a->key == key;
                }
            };

     Call example:
        auto funcObj = CmpKey(key);
        ListNode<ListEntry>* node = dictionary.findByPredicate(funcObj);
     * @tparam K - function object
     * @param funcObj
     * @return Node that answers the predicate.
     */
    template<typename K>
    ListNode<T> *findByPredicate(K funcObj);

    virtual int findIndexOfNode(ListNode<T> *listNode);

};

// Initialize LinkedList with false values
template<typename T>
LinkedList<T>::LinkedList() {
    root = NULL;
    last = NULL;
    _size = 0;

    lastNodeGot = root;
    lastIndexGot = 0;
    isCached = false;
}

// Clear Nodes and free Memory
template<typename T>
LinkedList<T>::~LinkedList() {
    ListNode<T> *tmp;
    while (root != NULL) {
        tmp = root;
        root = root->next;
        delete tmp;
    }
    last = NULL;
    _size = 0;
    isCached = false;
}

/*
	Actualy "logic" coding
*/

template<typename T>
ListNode<T> *LinkedList<T>::getNode(int index) {

    int _pos = 0;
    ListNode<T> *current = root;

    // Check if the node trying to get is
    // immediatly AFTER the previous got one
    if (isCached && lastIndexGot <= index) {
        _pos = lastIndexGot;
        current = lastNodeGot;
    }

    while (_pos < index && current) {
        current = current->next;

        _pos++;
    }

    // Check if the object index got is the same as the required
    if (_pos == index) {
        isCached = true;
        lastIndexGot = index;
        lastNodeGot = current;

        return current;
    }

    return nullptr;
}

template<typename T>
int LinkedList<T>::getSize() {
    return _size;
}

template<typename T>
bool LinkedList<T>::add(int index, T _t) {

    if (index >= _size)
        return insertLast(_t);

    if (index == 0)
        return insertFirst(_t);

    auto tmp = new ListNode<T>();
    auto _prev = getNode(index - 1);
    tmp->data = _t;

    tmp->next = _prev->next;
    tmp->prev = _prev;

    tmp->next->prev = tmp;

    _prev->next = tmp;

    _size++;
    isCached = false;

    return true;
}

template<typename T>
bool LinkedList<T>::insertLast(T _t) {

    ListNode<T> *tmp = new ListNode<T>();
    tmp->data = _t;
    tmp->next = NULL;
    tmp->prev = last;

    if (root) {
        // Already have elements inserted
        last->next = tmp;
        last = tmp;
    } else {
        // First element being inserted
        root = tmp;
        last = root;
    }

    _size++;
    isCached = false;

    return last;
}

template<typename T>
bool LinkedList<T>::insertFirst(T _t) {

    if (_size == 0)
        return insertLast(_t);

    auto tmp = new ListNode<T>();
    tmp->next = root;
    tmp->prev = NULL;

    tmp->data = _t;
    root = tmp;

    _size++;
    isCached = false;

    return true;
}

template<typename T>
bool LinkedList<T>::set(int index, T _t) {
    // Check if index position is in bounds
    if (index < 0 || index >= _size)
        return false;

    getNode(index)->data = _t;
    return true;
}

template<typename T>
T LinkedList<T>::pop() {
    if (_size <= 0)
        return T();

    isCached = false;

    if (_size >= 2) {
        ListNode<T> *tmp = getNode(_size - 2);
        T ret = tmp->next->data;
        delete (tmp->next);
        tmp->next = NULL;
        last = tmp;
        _size--;
        return ret;
    } else {
        // Only one element left on the list
        T ret = root->data;
        delete (root);
        root = NULL;
        last = NULL;
        _size = 0;
        return ret;
    }
}

template<typename T>
T LinkedList<T>::removeFirst() {
    if (_size <= 0)
        return T();

    if (_size > 1) {
        ListNode<T> *_next = root->next;
        T ret = root->data;
        delete (root);
        root = _next;

        root->prev = NULL;

        _size--;
        isCached = false;

        return ret;
    } else {
        // Only one left, then pop()
        return pop();
    }

}

template<typename T>
T LinkedList<T>::remove(int index) {
    if (index < 0 || index >= _size) {
        return T();
    }

    if (index == 0)
        return removeFirst();

    if (index == _size - 1) {
        return pop();
    }

    ListNode<T> *tmp = getNode(index - 1);
    ListNode<T> *toDelete = tmp->next;
    T ret = toDelete->data;
    tmp->next = tmp->next->next;
    delete (toDelete);
    _size--;
    isCached = false;
    return ret;
}


template<typename T>
T LinkedList<T>::get(int index) {
    ListNode<T> *tmp = getNode(index);

    return (tmp ? tmp->data : T());
}

template<typename T>
void LinkedList<T>::clear() {
    while (getSize() > 0)
        removeFirst();
}

template<typename T>
void LinkedList<T>::sort(int (*cmp)(T &, T &)) {
    if (_size < 2) return; // trivial case;

    for (;;) {

        ListNode<T> **joinPoint = &root;

        while (*joinPoint) {
            ListNode<T> *a = *joinPoint;
            ListNode<T> *a_end = findEndOfSortedString(a, cmp);

            if (!a_end->next) {
                if (joinPoint == &root) {
                    last = a_end;
                    isCached = false;
                    return;
                } else {
                    break;
                }
            }

            ListNode<T> *b = a_end->next;
            ListNode<T> *b_end = findEndOfSortedString(b, cmp);

            ListNode<T> *tail = b_end->next;

            a_end->next = NULL;
            b_end->next = NULL;

            while (a && b) {
                if (cmp(a->data, b->data) <= 0) {
                    *joinPoint = a;
                    joinPoint = &a->next;
                    a = a->next;
                } else {
                    *joinPoint = b;
                    joinPoint = &b->next;
                    b = b->next;
                }
            }

            if (a) {
                *joinPoint = a;
                while (a->next) a = a->next;
                a->next = tail;
                joinPoint = &a->next;
            } else {
                *joinPoint = b;
                while (b->next) b = b->next;
                b->next = tail;
                joinPoint = &b->next;
            }
        }
    }
}

template<typename T>
ListNode<T> *LinkedList<T>::findEndOfSortedString(ListNode<T> *p, int (*cmp)(T &, T &)) {
    while (p->next && cmp(p->data, p->next->data) <= 0) {
        p = p->next;
    }

    return p;
}

template<typename T>
ListNode<T> *LinkedList<T>::getLastNode() {
    return last;
}

template<typename T>
ListNode<T> *LinkedList<T>::getFirstNode() {
    return root;
}

template<typename T>
template<typename K>
ListNode<T> *LinkedList<T>::findByPredicate(K funcObj) {
    ListNode<T> *_node = getFirstNode();
    while (_node != nullptr && funcObj(_node->data) != true) {
        _node = _node->next;
    }
    return _node;
}
template<typename T>
int LinkedList<T>::findIndexOfNode(ListNode<T> *listNode) {
    int _pos = 0;
    ListNode<T> *current = root;
    while (current != listNode) {
        _pos++;
        current = current->next;
    }
    return _pos >= getSize() ? -1 : _pos;
}

template<typename T>
T LinkedList<T>::remove(ListNode<T> *listNode) {
    if (listNode == nullptr) return T();

    auto prev = listNode->prev;
    auto next = listNode->next;
    auto deletedData = listNode->data;

    if (listNode == root) root = next;
    if (listNode == last) last = prev;
    delete(listNode);

    if (prev != nullptr) prev->next = next;
    if (next != nullptr) next->prev = prev;

    _size--;
    return deletedData;
}

#endif
#ifndef SORTEDSET_H
#define SORTEDSET_H

#include <functional>

using namespace std;

/**
 * Sorted Set
 * Implemented as Linked list
 *
 * Template:
 * <TypeName, CompareFunc>
 * Compare function should return compare(a,b) == true if 'a' is less than 'b'
 */


template<class T, class Compare = std::less<T> >
class SortedSet {

private:
    struct Node {
        T *data;
        Node *node;

        void connectNext(Node *next) {
            next->node = node;
            node = next;
        }

        void destroyNext() {
            Node *tmp = node;
            node = node->node;
            delete tmp->data;
            delete tmp;
        }

        bool isLast() { return node == 0; }
    };

    Node *headNode;
    Node *endNode;
    Compare compare;

    void destroyNodeList() {
        for (; headNode->node != endNode
               || !(headNode->node->isLast()); headNode->destroyNext()) {}
    }

public:
    class iterator {
    private:
        Node *current;

        void addObj(const T &obj) {
            current->connectNext(new Node{new T(obj), 0});
        }

        void deleteNextObj() { current->destroyNext(); }

        static const int ITERATOR_BEGINING = 1;

        friend class SortedSet;

    public:
        iterator(const SortedSet *s, int location = ITERATOR_BEGINING) {
            current = s->headNode;
            for (int i = 0; i < location; i++, this->operator++()) {};
        }

        iterator(const iterator &other) = default;

        bool operator==(const iterator &i) {
            return (this->current == i.current);
        }

        bool operator!=(const iterator &i) {
            return (this->current != i.current);
        }

        iterator &operator++() {
            current = ((current->node) ? current->node : current);
            return *this;
        }

        iterator operator++(int) {
            iterator copy(*this);
            ++(*this);
            return copy;
        }

        T *const operator->() const { return current->data; }

        T &operator*() const {
            return *(current->data);
        }
    };

    /*
     * Main constructor - no parameters
     * Initialize a SortedSet with no items in it
     * put in brackets: <myType, compare_functor)
     */
    SortedSet();

    /**
     * CopyConstructor
     */
    SortedSet(const SortedSet &other);

    /**
     * Destructor
     */
    ~SortedSet() {
        destroyNodeList();
        delete headNode;
        delete endNode;
    }

    /**
     * Inserts an item of type T to the set, will be automatically inserted
     * in order just after the closest 'lower' item.
     * If item already exists in SortedSet, nothing happens
     *
     * @return
     * 	if item was inserted - return true
     * 	if no item was inserted - return false
     */
    bool insert(const T &obj);

    /**
     * Removes item corresponding to given item based on provided compare
     * functor
     * If no item corresponds, nothing happens
     *
     * @return
     * 	If the corresponding item is found and deleted - return true
     * 	it not - return false
     */
    bool remove(const T &obj);

    /**
     * Searches in SortedSet for an item corresponding to 'obj'
     *
     * @return
     * 	iterator 'pointing' at requested object
     * 	if no match - iterator 'pointing' at end()
     *
     * Warning - if used, user shouldn't dereference iterator before first
     * checking return value != end()
     */
    iterator find(const T &obj) const;

    /**
     * @return
     * 	number of items in SortedSet
     */
    int size() const;

    /**
     * @return
     * 	iterator 'pointing' at first item in SortedSet
     * 	if Set is empty - iterator 'pointing' at end()
     */
    iterator begin() const { return iterator(this); }

    /**
     * @return
     * 	iterator 'pointing' at invalid data, representing the end of the Set
     */
    iterator end() const { return ++iterator(this, size()); }

    /*
     * Assignment operator
     */
    SortedSet &operator=(const SortedSet &other);

    /**
     * Intersection and assignment operator
     */
    SortedSet &operator&=(const SortedSet &other);

    /**
     * Intersection operator
     */
    SortedSet operator&(const SortedSet &other) const {
        return SortedSet(*this) &= other;
    }

    /**
     * Union and assignment operator
     */
    SortedSet &operator|=(const SortedSet &other);

    /**
     * Union operator
     */
    SortedSet operator|(const SortedSet &other) const {
        return SortedSet(*this) |= other;
    }

    /**
     * Minus and assigment operator
     */
    SortedSet &operator-=(const SortedSet &other);

    /**
     * Minus operator
     */
    SortedSet operator-(const SortedSet &other) const {
        return SortedSet(*this) -= other;
    }

    /**
     * Symetrical difference and assignment operator
     */
    SortedSet &operator^=(const SortedSet &other);

    /**
     * Symetrical difference operator
     */
    SortedSet operator^(const SortedSet &other) const {
        return SortedSet(*this) ^= other;
    }

    /**
     * Is not equal to
     */
    bool operator!=(const SortedSet &other) const;


    /**
     * Is equal to
     */
    bool operator==(const SortedSet &other) const {
        return !(this->operator!=(other));
    }


};


template<class T, class Compare>
SortedSet<T, Compare>::SortedSet() {
    endNode = new Node{0, 0};
    headNode = new Node{0, endNode};
}

template<class T, class Compare>
SortedSet<T, Compare>::SortedSet(const SortedSet &other):SortedSet() {
    for (iterator i = other.begin();
         i != other.end(); ++i) {
        insert(*i);
    }
}

template<class T, class Compare>
typename SortedSet<T, Compare>::
iterator SortedSet<T, Compare>::find(const T &obj) const {
    iterator i = begin();
    for (; i != end() && compare(*i, obj); ++i) {}
    if (i != end() && !compare(obj, *i)) {
        return i;
    }
    return end();
}

template<class T, class Compare>
int SortedSet<T, Compare>::size() const {
    int j = 0;
    for (iterator i(this); !(i.current->isLast()); ++i, ++j) {}
    return j;
}

template<class T, class Compare>
bool SortedSet<T, Compare>::insert(const T &obj) {
    if (size() == 0 || compare(obj, *begin())) {
        headNode->connectNext(new Node{new T(obj), 0});
        return true;
    }
    iterator i = begin();
    iterator previous = begin();
    for (; i != end() && compare(*i, obj); ++i) {
        previous = i;
    }
    if (i != end() && !compare(obj, *i)) {
        return false;
    }
    previous.addObj(obj);
    return true;
}

template<class T, class Compare>
bool SortedSet<T, Compare>::remove(const T &obj) {
    if (size() == 0) {
        return false;
    }
    iterator i = begin();
    iterator previous = begin();
    for (; i != end() && compare(*i, obj); ++i) {
        previous = i;
    }
    if (i != end() && !compare(obj, *i)) {
        if (i == begin()) {
            headNode->destroyNext();
        } else {
            previous.deleteNextObj();
        }
        return true;
    }
    return false;
}

template<class T, class Compare>
SortedSet<T, Compare> &
SortedSet<T, Compare>::operator=(const SortedSet &other) {
    if (this == &other) {
        return *this;
    }
    destroyNodeList();
    for (iterator i = other.begin(); i != other.end(); ++i) {
        insert(*i);
    }
    return *this;
}

template<class T, class Compare>
SortedSet<T, Compare> &
SortedSet<T, Compare>::operator&=(const SortedSet &other) {
    SortedSet tmp(*this);
    for (auto i: tmp) {
        if (other.find(i) != other.end()) {
            insert(i);
        } else {
            remove(i);
        }
    }
    return *this;
}

template<class T, class Compare>
SortedSet<T, Compare> &
SortedSet<T, Compare>::operator|=(const SortedSet &other) {
    for (auto i:other) {
        insert(i);
    }
    return *this;
}

template<class T, class Compare>
SortedSet<T, Compare> &
SortedSet<T, Compare>::operator-=(const SortedSet &other) {
    SortedSet tmp(*this);
    for (auto i: tmp) {
        if (other.find(i) != other.end()) {
            remove(i);
        }
    }
    return *this;
}

template<class T, class Compare>
SortedSet<T, Compare> &
SortedSet<T, Compare>::operator^=(const SortedSet &other) {
    *this = (*this - other) |= (other - *this);
    return *this;
}

template<class T, class Compare>
bool SortedSet<T, Compare>::operator!=(const SortedSet &other) const {
    SortedSet<T, Compare>::iterator other_iterator = other.begin();
    for (T &obj: *this) {
        if (other_iterator != other.end() && (compare(obj, *other_iterator)
                                              ||
                                              compare(*other_iterator, obj))) {
            return true;
        }
        ++other_iterator;
    }
    if (other_iterator != other.end()) {
        return true;
    } else if (other.size() != size()) {
        return true;
    }
    return false;
}


#endif //SORTEDSET_H
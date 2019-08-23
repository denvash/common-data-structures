#include "assert.h"
#include <iostream>

#ifndef LIST_H
#define LIST_H

namespace ListExceptions {
    class ElementNotFound : public std::runtime_error {
    public:
        ElementNotFound() : std::runtime_error("Element not found") {}
    };
}

template<class T>
class List {
 public:
  List();

  List(const List& list);

  ~List();

  List& operator=(const List& list);

  class Iterator {
   public:
    Iterator& operator++();

    Iterator operator++(int);

    Iterator& operator--();

    Iterator operator--(int);

    const T& operator*() const;

    T& operator*();

    bool operator==(const Iterator& it) const;

    bool operator!=(const Iterator& it) const;

    Iterator(const Iterator&) = default;

    Iterator& operator=(const Iterator&) = default;

   private:
    const List<T>* list;
    typename List<T>::Node* current;

    Iterator(const List<T>* list, typename List<T>::Node* current) :
        list(list), current(current) {}

    friend class List<T>;
  };

  Iterator begin() const;

  Iterator end() const;

  void insert(const T& data, Iterator iterator);

  void insert(const T& data);

  void remove(Iterator iterator);

  bool operator==(const List& list) const;

  bool operator!=(const List& list) const;

  template<class Predicate>
  Iterator find(const Predicate& predicate) const;

  template<class Compare>
  void sort(const Compare& compare);

  int getSize() const;

 private:
  int size;

  class Node {
   public:
    Node(const T& data, Node* previous, Node* next) :
        data(new T(data)), previous(previous), next(next) {}

    ~Node() { delete data; }

    T* data;
    Node* previous;
    Node* next;
  };

  Node* first;
  Node* last;

  void removeNodes();

  void insertNodes(const List& list);

  void swapData(T*& data1, T*& data2);
};

/*
 * ***List***
 */

template<class T>
List<T>::List(): size(0), first(NULL), last(NULL) {}

template<class T>
List<T>::List(const List<T>& list) : size(0),
                                     first(NULL), last(NULL) {
  insertNodes(list);
  assert(size == list.getSize());
}

template<class T>
List<T>::~List() {
  removeNodes();
}

template<class T>
void List<T>::removeNodes() {
  int initial_size = size;
  for (int i = 0; i < initial_size; ++i) {
    remove(begin());
  }
  assert(size == 0);
}

template<class T>
void List<T>::insertNodes(const List<T>& list) {
  assert(size == 0);
  for (List<T>::Iterator it = list.begin(); it != list.end(); ++it) {
    insert(*(it.current->data));
  }
}

template<class T>
List<T>& List<T>::operator=(const List<T>& list) {
  if (&list == this) {
    return *this;
  }
  removeNodes();
  insertNodes(list);
  assert(size == list.getSize());
  return *this;
}

template<class T>
typename List<T>::Iterator List<T>::begin() const {
  return Iterator(this, this->first);
}

template<class T>
typename List<T>::Iterator List<T>::end() const {
  return Iterator(this, NULL);
}

template<class T>
void List<T>::insert(const T& data, List<T>::Iterator iterator) {
  if (iterator.list != this) {
    throw ElementNotFound();
  }
  Node* new_node = NULL;
  Node* previous_node = NULL;
  Node* next_node = NULL;
  if (iterator == end()) {
    previous_node = last;
  } else {
    previous_node = iterator.current->previous;
    next_node = iterator.current;
  }
  new_node = new Node(data, previous_node, next_node);
  if (previous_node != NULL) {
    previous_node->next = new_node;
  } else {
    first = new_node;
  }
  if (next_node != NULL) {
    next_node->previous = new_node;
  } else {
    last = new_node;
  }
  size++;
  return;
}

template<class T>
void List<T>::insert(const T& data) {
  insert(data, end());
  return;
}

template<class T>
void List<T>::remove(List<T>::Iterator iterator) {
  if (iterator.list != this || getSize() == 0 || iterator == end()) {
    throw ElementNotFound();
  }
  Node* previous_node = iterator.current->previous;
  Node* next_node = iterator.current->next;
  if (previous_node != NULL) {
    previous_node->next = next_node;
  } else {
    first = next_node;
  }
  if (next_node != NULL) {
    next_node->previous = previous_node;
  } else {
    last = previous_node;
  }
  delete iterator.current;
  size--;
  return;
}

template<class T>
bool List<T>::operator==(const List<T>& list) const {
  if (getSize() != list.getSize()) {
    return false;
  }
  typename List<T>::Iterator it1 = list.begin();
  for (typename List<T>::Iterator it2 = begin(); it2 != end(); ++it2) {
    if (*it1 != *it2) {
      return false;
    }
    ++it1;
  }
  return true;
}

template<class T>
bool List<T>::operator!=(const List<T>& list) const {
  return !(*this == list);
}

template<class T>
template<class Predicate>
typename List<T>::Iterator List<T>::find(const Predicate& predicate) const {
  typename List<T>::Iterator it = begin();
  for (; it != end(); ++it) {
    if (predicate(*it)) {
      return it;
    }
  }
  assert(it == end());
  return it;
}

template<class T>
template<class Compare>
void List<T>::sort(const Compare& compare) {
  for (int i = getSize(); i > 0; --i) {
    List<T>::Iterator it1 = begin();
    List<T>::Iterator it2 = it1;
    it2++;
    for (int j = 1; j <= i - 1; ++j) {
      if (!compare(*it1, *it2)) {
        List<T>::swapData(it1.current->data, it2.current->data);
      }
      ++it1;
      ++it2;
    }
  }
  return;
}

template<class T>
void List<T>::swapData(T*& data1, T*& data2) {
  T* tmp(data1);
  data1 = data2;
  data2 = tmp;
  return;
}

template<class T>
int List<T>::getSize() const {
  return size;
}

/*
 * ***Iterator***
 */

template<class T>
typename List<T>::Iterator& List<T>::Iterator::operator++() {
//    if (*this == list->end()){
//        throw ElementNotFound();
//    }
  current = current->next;
  return *this;
}

template<class T>
typename List<T>::Iterator List<T>::Iterator::operator++(int) {
  Iterator it = *this;
  ++*this;
  return it;
}

template<class T>
typename List<T>::Iterator& List<T>::Iterator::operator--() {
  current = current->previous;
  return *this;
}

template<class T>
typename List<T>::Iterator List<T>::Iterator::operator--(int) {
  Iterator it = *this;
  --*this;
  return it;
}

template<class T>
const T& List<T>::Iterator::operator*() const {
  if (*this == list->end()) {
    throw ElementNotFound();
  }
  return *(this->current->data);
}

template<class T>
T& List<T>::Iterator::operator*() {
  if (*this == list->end()) {
    throw ElementNotFound();
  }
  return *(this->current->data);
}

template<class T>
bool List<T>::Iterator::operator==(const Iterator& it) const {
  return (list == it.list && current == it.current);
}

template<class T>
bool List<T>::Iterator::operator!=(const Iterator& it) const {
  return !(*this == it);
}

#endif //LIST_H
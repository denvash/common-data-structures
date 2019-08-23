#ifndef MINHEAP_H
#define MINHEAP_H

#include<iostream>
#include<climits>

using namespace std;

class HeapIsEmpty : public exception {
};

class HeapIndexOutOfRange : public exception {
};

class HeapIllegalInput : public exception {
};

class MinHeap {
    int* heap_array;
    int capacity;
    int size;
private:

    int parent(int i) { return (i - 1) / 2; }

    int left(int i) { return (2 * i + 1); }

    int right(int i) { return (2 * i + 2); }

    void swap(int* x, int* y) {
        int temp = *x;
        *x = *y;
        *y = temp;
    }

    void swift(int index) {
        int l = left(index);
        int r = right(index);
        int smallest = index;
        if (l < size && heap_array[l] < heap_array[index]) { smallest = l; }
        if (r < size && heap_array[r] < heap_array[smallest]) { smallest = r; }
        if (smallest != index) {
            swap(&heap_array[index], &heap_array[smallest]);
            swift(smallest);
        }
    }

    void increase_capacity() {
        auto bigger = new int[2 * capacity];

        for (int i = 0; i < capacity; ++i) {
            bigger[i] = heap_array[i];
        }
        delete[]heap_array;
        capacity *= 2;
        heap_array = bigger;
    }

public:

    int getSize() { return size; }

    bool isEmpty() { return size <= 0; }

    MinHeap(int cap) {
        if (cap <= 0) { throw HeapIllegalInput(); }
        size = 0;
        capacity = cap;
        heap_array = new int[cap];
    }

    ~MinHeap() { delete[]heap_array; }

    MinHeap(int size, int* array) {
        this->size = size;
        capacity = this->size;
        heap_array = new int[this->size];
        for (int i = 0; i < this->size; ++i) {
            insert(array[i]);
        }
    }

    void remove_min() {
        if (size <= 0)
            throw HeapIsEmpty();
        if (size == 1) {
            size--;
        }

        heap_array[0] = heap_array[size - 1];
        size--;
        swift(0);
    }

    int get_min() { return heap_array[0]; }

    void insert(int k) {
        if (size == capacity) {
            increase_capacity();
        }

        size++;
        int i = size - 1;
        heap_array[i] = k;

        while (i != 0 && heap_array[parent(i)] > heap_array[i]) {
            swap(&heap_array[i], &heap_array[parent(i)]);
            i = parent(i);
        }
    }

    int extract_min() {
        if (size <= 0)
            throw HeapIsEmpty();
        if (size == 1) {
            size--;
            return heap_array[0];
        }

        // Store the minimum value, and remove it from heap
        int root = heap_array[0];
        heap_array[0] = heap_array[size - 1];
        size--;
        swift(0);
        return root;


    }
};

#endif //OASIS_MINHEAP_H
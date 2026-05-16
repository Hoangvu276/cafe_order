#pragma once
#include <stdexcept>
#include <cstddef>


/// hoan doi cho 2 phan tu
template <typename T>
void _swap(T& a, T& b) {
    T temp = a;
    a = b;
    b = temp;
}

template <typename T, typename Comp = std::less<T>>
void bubble_sort(T* arr, std::size_t size, Comp comp = Comp()) {
    for (std::size_t i = 0; i < size - 1; ++i) {
        for (std::size_t j = 0; j < size - i - 1; ++j) {
            if (comp(arr[j + 1], arr[j])) {
                _swap(arr[j], arr[j + 1]);
            }
        }
    }
}

template <typename T, typename Comp = std::less<T>>
void selection_sort(T* arr, std::size_t size, Comp comp = Comp()) {
    for (std::size_t i = 0; i < size - 1; ++i) {
        std::size_t min_index = i;
        for (std::size_t j = i + 1; j < size; ++j) {
            if (comp(arr[j], arr[min_index])) {
                min_index = j;
            }
        }
        if (min_index != i) {
            _swap(arr[i], arr[min_index]);
        }
    }
}

template <typename T, typename Comp = std::less<T>>
void insertion_sort(T* arr, std::size_t size, Comp comp = Comp()) {
    for (std::size_t i = 1; i < size; ++i) {
        T key = arr[i];
        std::size_t j = i;
        while (j > 0 && comp(key, arr[j - 1])) {
            arr[j] = arr[j - 1];
            --j;
        }
        arr[j] = key;
    }
}

template <typename T, typename Comp = std::less<T>>
void heapify(T* arr, std::size_t size, std::size_t root, Comp comp = Comp()) {
    std::size_t largest = root;
    std::size_t left = 2 * root + 1;
    std::size_t right = 2 * root + 2;

    if (left < size && comp(arr[largest], arr[left])) {
        largest = left;
    }
    if (right < size && comp(arr[largest], arr[right])) {
        largest = right;
    }
    if (largest != root) {
        _swap(arr[root], arr[largest]);
        heapify(arr, size, largest, comp);
    }
}

template <typename T, typename Comp = std::less<T>>
void heap_sort(T* arr, std::size_t size, Comp comp = Comp()) {
    for (std::size_t i = size / 2 - 1; i < size; --i) {
        heapify(arr, size, i, comp);
    }
    for (std::size_t i = size - 1; i > 0; --i) {
        _swap(arr[0], arr[i]); 
        heapify(arr, i, 0, comp);
    }
}

template <typename T, typename Comp = std::less<T>>
void quicksort(T* arr, std::size_t lo, std::size_t hi, Comp comp = Comp()){
    if(lo >= hi) return;
    
    int mid = lo + (hi - lo) / 2;
    if(cmp(arr[mid], arr[lo])) _swap(arr[lo], arr[mid]);
    if(cmp(arr[hi],  arr[lo])) _swap(arr[lo], arr[hi]);
    if(cmp(arr[mid], arr[hi])) _swap(arr[hi], arr[mid]);

    int p = _partition(arr, lo, hi, cmp);
    quicksort(arr, lo, p - 1, )
}   
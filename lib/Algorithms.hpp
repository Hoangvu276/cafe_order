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

template<typename T, typename Comp = std::less<T>>
int _partition(T* arr, std::size_t lo, std::size_t hi, Comp comp = Comp()){
    T pivot = arr[lo + (hi - lo) / 2];
    int i = lo - 1;
    int j = hi + 1;
    while(1){
        do{
            ++i;
        }while(cmp(arr[i], pivot))
        do{
            --;
        }while(cmp(pivot, arr[j]));

        if(i>=j) return j;

        _swap(arr[i], arr[j]);
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
    quicksort(arr, lo, p - 1,)
}   

template <typename T, typename Comp = std::less<T>>
void merge(T* arr, std::size_t left, std::size_t mid, std::size_t right, Comp comp = Comp()) {
    std::size_t n1 = mid - left + 1;
    std::size_t n2 = right - mid;

    T* L = new T[n1];
    T* R = new T[n2];

    for (std::size_t i = 0; i < n1; ++i) {
        L[i] = arr[left + i];
    }
    for (std::size_t j = 0; j < n2; ++j) {
        R[j] = arr[mid + 1 + j];
    }

    std::size_t i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (comp(L[i], R[j])) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }
    while (i < n1) {
        arr[k++] = L[i++];
    }
    while (j < n2) {
        arr[k++] = R[j++];
    }

    delete[] L;
    delete[] R;
}

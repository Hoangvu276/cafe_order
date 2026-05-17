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
/**
 * bubbleSort - Sắp xếp nổi bọt - O(n²)
 * arr : mảng cần sắp xếp
 * n   : số phần tử
 * cmp : hàm so sánh (mặc định tăng dần)
 */
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
/**
 * selectionSort - Sắp xếp chọn - O(n²)
 * arr : mảng cần sắp xếp
 * n   : số phần tử
 * cmp : hàm so sánh (mặc định tăng dần)
 */
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
/**
 * insertionSort - Sắp xếp chèn - O(n²), hiệu quả với mảng gần sắp xếp
 * arr : mảng cần sắp xếp
 * n   : số phần tử
 * cmp : hàm so sánh (mặc định tăng dần)
 */
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
/**
 * heapSort - Sắp xếp vun đống - O(n log n)
 * arr : mảng cần sắp xếp
 * n   : số phần tử
 * cmp : hàm so sánh (mặc định tăng dần)
 */
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
        }while(comp(arr[i], pivot));
        do{
            --j;
        }while(comp(pivot, arr[j]));

        if(i>=j) return j;

        _swap(arr[i], arr[j]);
    }
}
/**
 * quickSort - Sắp xếp nhanh - O(n log n) trung bình
 * arr : mảng cần sắp xếp
 * lo  : chỉ số bắt đầu (thường là 0)
 * hi  : chỉ số kết thúc (thường là n-1)
 * cmp : hàm so sánh (mặc định tăng dần)
 */
template <typename T, typename Comp = std::less<T>>
void quicksort(T* arr, std::size_t lo, std::size_t hi, Comp comp = Comp()){
    if(lo >= hi) return;
    
    int mid = lo + (hi - lo) / 2;
    if(comp(arr[mid], arr[lo])) _swap(arr[lo], arr[mid]);
    if(comp(arr[hi],  arr[lo])) _swap(arr[lo], arr[hi]);
    if(comp(arr[mid], arr[hi])) _swap(arr[hi], arr[mid]);

    int p = _partition(arr, lo, hi, comp);
    quicksort(arr, lo, p - 1, comp);
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
/**
 * mergeSort - Sắp xếp trộn - O(n log n), ổn định
 * arr : mảng cần sắp xếp
 * n   : số phần tử
 * cmp : hàm so sánh (mặc định tăng dần)
 */
template <typename T, typename Comp = std::less<T>>
void merge_sort(T* arr, std::size_t left, std::size_t right, Comp comp = Comp()) {
    if (left < right) {
        std::size_t mid = left + (right - left) / 2;
        merge_sort(arr, left, mid, comp);
        merge_sort(arr, mid + 1, right, comp);
        merge(arr, left, mid, right, comp);
    }
}

/**
 * linearSearch - Tìm kiếm tuyến tính - O(n)
 * arr : mảng cần tìm
 * n   : số phần tử
 * key : giá trị cần tìm
 * Trả về: chỉ số phần tử đầu tiên bằng key, hoặc -1 nếu không có
 */
template <typename T, typename Comp = std::less<T>>
int linear_search(T* arr, std::size_t size, const T& target, Comp comp = Comp()) {
    for (std::size_t i = 0; i < size; ++i) {
        if (!comp(arr[i], target) && !comp(target, arr[i])) {
            return static_cast<int>(i);
        }
    }
    return -1; // not found
}
/**
 * binarySearch - Tìm kiếm nhị phân - O(log n)
 * Yêu cầu mảng đã được sắp xếp tăng dần theo cmp.
 * arr : mảng đã sắp xếp
 * n   : số phần tử
 * key : giá trị cần tìm
 * cmp : hàm so sánh tương ứng với thứ tự sắp xếp (mặc định tăng dần)
 * Trả về: chỉ số phần tử tìm thấy (bất kỳ nếu có trùng lặp), hoặc -1
 */
template <typename T, typename Comp = std::less<T>>
int binary_search(T* arr, std::size_t size, const T& target, Comp comp = Comp()) {
    std::size_t left = 0;
    std::size_t right = size - 1;

    while (left <= right) {
        std::size_t mid = left + (right - left) / 2;
        if (comp(arr[mid], target)) {
            left = mid + 1;
        } else if (comp(target, arr[mid])) {
            right = mid - 1;
        } else {
            return static_cast<int>(mid);
        }
    }
    return -1; // not found
}


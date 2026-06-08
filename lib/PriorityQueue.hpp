#pragma once
#include <stdexcept>
#include <functional>
/*
    * PriorityQueue<T> - Hàng đợi ưu tiên tổng quát theo kiểu T.
    * Cài đặt bằng max-heap trên mảng động.
    * Mặc định phần tử lớn nhất (theo Comp = std::less<T>) được ưu tiên cao nhất.
    *
    * Cách dùng (max-heap mặc định):
    *   PriorityQueue<int> pq;
    *   pq.insert(5); pq.insert(1); pq.insert(9);
    *   int top = pq.extract(); // trả về 9
    *
    * Cách dùng (min-heap):
    *   PriorityQueue<int, std::greater<int>> pq;
*/
template <typename T, typename Comp = std::less<T>>
struct PriorityQueue {
private:
    T*   data;
    int  cap;
    int  sz;
    Comp cmp;

    void resize(int newCap) {
        T* newData = new T[newCap];
        for (int i = 0; i < sz; ++i) newData[i] = data[i];
        delete[] data;
        data = newData;
        cap  = newCap;
    }

    // Đẩy phần tử tại vị trí i lên đúng chỗ trong heap
    void siftUp(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (cmp(data[parent], data[i])) {   // parent < child -> đổi chỗ
                T tmp = data[parent]; data[parent] = data[i]; data[i] = tmp;
                i = parent;
            } else break;
        }
    }

    // Đẩy phần tử tại vị trí i xuống đúng chỗ trong heap
    void siftDown(int i) {
        while (true) {
            int best = i;
            int l = 2*i+1, r = 2*i+2;
            if (l < sz && cmp(data[best], data[l])) best = l;
            if (r < sz && cmp(data[best], data[r])) best = r;
            if (best == i) break;
            T tmp = data[i]; data[i] = data[best]; data[best] = tmp;
            i = best;
        }
    }
public:
    // Khởi tạo hàng đợi ưu tiên rỗng
    explicit PriorityQueue(int initCap = 8, Comp c = Comp())
        : data(new T[initCap]), cap(initCap), sz(0), cmp(c) {}

    ~PriorityQueue() {
        delete[] data;
    }

    PriorityQueue(const PriorityQueue& o) : data(new T[o.cap]), cap(o.cap), sz(o.sz), cmp(o.cmp) {
        for (int i = 0; i < sz; ++i) data[i] = o.data[i];
    }
    PriorityQueue& operator=(const PriorityQueue& o) {
        if (this != &o) {
            delete[] data;
            cap = o.cap; sz = o.sz; cmp = o.cmp;
            data = new T[cap];
            for (int i = 0; i < sz; ++i) data[i] = o.data[i];
        }
        return *this;
    }

    // Thêm phần tử vào hàng đợi ưu tiên - O(log n)
    void insert(const T& val) {
        if (sz == cap) resize(cap * 2);
        data[sz++] = val;
        siftUp(sz - 1);
    }

    // Lấy và xóa phần tử có độ ưu tiên cao nhất - O(log n)
    // Ném std::underflow_error nếu hàng đợi rỗng
    T extract() {
        if (empty()) throw std::underflow_error("PriorityQueue::extract on empty queue");
        T top = data[0];
        data[0] = data[--sz];
        siftDown(0);
        return top;
    }

    // Trả về phần tử có độ ưu tiên cao nhất mà không xóa - O(1)
    // Ném std::underflow_error nếu hàng đợi rỗng
    const T& peek() const {
        if (empty()) throw std::underflow_error("PriorityQueue::peek on empty queue");
        return data[0];
    }

    // Trả về số phần tử hiện tại
    int size() const {
        return sz;
    }

    // Kiểm tra hàng đợi có rỗng không
    bool empty() const {
        return sz == 0;
    }
};
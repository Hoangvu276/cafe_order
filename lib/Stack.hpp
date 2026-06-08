#pragma once
#include <stdexcept>

/*
    * Stack<T> - Ngăn xếp tổng quát theo kiểu T (LIFO).
    * Cài đặt nội bộ bằng mảng động tự mở rộng.
    *
    * Cách dùng:
    *   Stack<int> st;
    *   st.push(10);
    *   int val = st.pop();
*/  

template<typename T>
struct Stack {
private:
    T*  data;
    int cap;
    int topIdx;

    void resize(int newCap){
        T* newData = new T[newCap];
        for(int i=0; i<=topIdx; ++i) newData[i] = data[i];
        delete[] data;
        data = newData;
        cap  = newCap;
    }
public:
    // Khởi tạo ngăn xếp rỗng với dung lượng ban đầu
    explicit Stack(int initCap = 8)
        : data(new T[initCap]), cap(initCap), topIdx(-1) {}

    ~Stack() {
        delete[] data;
    }

    Stack(const Stack& o) : data(new T[o.cap]), cap(o.cap), topIdx(o.topIdx) {
        for (int i = 0; i <= topIdx; ++i) data[i] = o.data[i];
    }
    Stack& operator=(const Stack& o) {
        if (this != &o) {
            delete[] data;
            cap = o.cap; topIdx = o.topIdx;
            data = new T[cap];
            for (int i = 0; i <= topIdx; ++i) data[i] = o.data[i];
        }
        return *this;
    }

    // Thêm phần tử vào đỉnh ngăn xếp - O(1) khấu hao
    void push(const T& val) {
        if (topIdx + 1 == cap) resize(cap * 2);
        data[++topIdx] = val;
    }

    // Lấy và xóa phần tử ở đỉnh ngăn xếp - O(1)
    // Ném std::underflow_error nếu ngăn xếp rỗng
    T pop() {
        if (empty()) throw std::underflow_error("Stack::pop on empty stack");
        return data[topIdx--];
    }

    // Trả về true nếu ngăn xếp rỗng, false nếu không
    bool empty() const {
        return topIdx == -1; 
    }
};
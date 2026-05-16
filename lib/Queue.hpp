#pragma once
template <typename T>
#include <Vector>
struct Queue {
    Vector<T> data_;
    int frontIndex_;

    Queue() {
        frontIndex_ = 0;
    }

    void push(const T& value) {
        data_.push_back(value);
    }

    void pop() {
        if (empty()) {
            std :: cout <<"Queue empty, Can't pop()";
        }

        frontIndex_++;

        if (frontIndex_ * 2 >= data_.size()) {
            compact();
        }
    }

    T& front() {
        if (empty()) {
            throw std::out_of_range("Queue::front on empty queue");
        }

        return data_[frontIndex_];
    }

    const T& front() const {
        if (empty()) {
            throw std::out_of_range("Queue::front on empty queue");
        }

        return data_[frontIndex_];
    }

    bool empty() const {
        return frontIndex_ >= data_.size();
    }

    int size() const {
        return data_.size() - frontIndex_;
    }

    void clear() {
        data_.clear();
        frontIndex_ = 0;
    }
    //merge data - loai bo cac phan tu thua
    void compact() {
        Vector<T> newData;

        for (int i = frontIndex_; i < data_.size(); i++) {
            newData.push_back(data_[i]);
        }

        data_ = newData;
        frontIndex_ = 0;
    }
};
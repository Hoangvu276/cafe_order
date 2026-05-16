#pragma once

template<typename T>
struct queue {

    Vector<T> data_;

    void push(const T& item) {
        data_.push_back(item);
    }

    void pop() {
        data_.erase(data_.begin());
    }

    T front() {
        return data_[0];
    }
    T rear(){
    	return data_[data_.size_()-1];
    }
};
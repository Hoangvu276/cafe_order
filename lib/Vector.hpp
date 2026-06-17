#pragma once
#include <initializer_list>
#include <stdexcept>
#include <iostream>

template<typename T>
struct Vector {
    T* data_;
    int size_;
    int capacity_;

    Vector() {
        data_ = nullptr;
        size_ = 0;
        capacity_ = 0;
    }

    Vector(int cap) {
        capacity_ = cap;
        size_ = 0;
        data_ = new T[capacity_];
    }

    Vector(int n, const T& value) {
        size_ = n;
        capacity_ = n;
        data_ = new T[capacity_];
        for (int i = 0; i < size_; i++) {
            data_[i] = value;
        }
    }

    Vector(std::initializer_list<T> list) {
        size_ = list.size();
        capacity_ = size_;
        data_ = new T[capacity_];
        int i = 0;
        for (const T& item : list) {
            data_[i] = item;
            i++;
        }
    }

    Vector(const Vector<T>& other_) {
        size_ = other_.size_;
        capacity_ = other_.capacity_;
        if (capacity_ == 0) {
            data_ = nullptr;
        } else {
            data_ = new T[capacity_];
            for (int i = 0; i < size_; i++) {
                data_[i] = other_.data_[i];
            }
        }
    }

    Vector& operator=(const Vector<T>& other_) {
        if (this != &other_) {
            delete[] data_;
            size_ = other_.size_;
            capacity_ = other_.capacity_;
            if (capacity_ == 0) {
                data_ = nullptr;
            } else {
                data_ = new T[capacity_];
                for (int i = 0; i < size_; i++) {
                    data_[i] = other_.data_[i];
                }
            }
        }
        return *this;
    }

    ~Vector() {
        delete[] data_;
    }

    void push_back(const T& item) {
        if (size_ == capacity_) {
            if (capacity_ == 0) {
                capacity_ = 1;
            } else {
                capacity_ *= 2;
            }
            T* newdata_ = new T[capacity_];
            for (int i = 0; i < size_; i++) {
                newdata_[i] = data_[i];
            }
            delete[] data_;
            data_ = newdata_;
        }
        data_[size_] = item;
        size_++;
    }

    void pop_back() {
        if (size_ == 0) {
            std::cout << "pop_back on empty Vector\n";
            return;
        }
        size_--;
    }

    int size() const {
        return size_;
    }

    void print() const {
        for (int i = 0; i < size_; i++) {
            std::cout << data_[i] << " ";
        }
        std::cout << "\n";
    }

    bool empty() const {
        return (size_ == 0);
    }

    void resize(int newSize, const T& value = T()) {
        if (newSize < 0) return;
        if (newSize <= capacity_) {
            for (int i = size_; i < newSize; i++) {
                data_[i] = value;
            }
            size_ = newSize;
            return;
        }
        int newCap = newSize;
        T* newdata_ = new T[newCap];
        for (int i = 0; i < size_; i++) {
            newdata_[i] = data_[i];
        }
        for (int i = size_; i < newSize; i++) {
            newdata_[i] = value;
        }
        delete[] data_;
        data_ = newdata_;
        size_ = newSize;
        capacity_ = newCap;
    }

    void clear() {
        size_ = 0;
    }

    T* begin() {
        return data_;
    }

    const T* begin() const {
        return data_;
    }
    
    T* end() {
        return data_ + size_;
    }

    const T* end() const {
        return data_ + size_;
    }

    void erase(int index) {
        if (index < 0 || index >= size_) {
            throw std::out_of_range("Vector::erase index out of range");
        }
        for (int i = index; i < size_ - 1; i++) {
            data_[i] = data_[i + 1];
        }
        size_--;
    }

    T& operator[](int index) {
        if (index < 0 || index >= size_) {
            throw std::out_of_range("Vector index out of range");
        }
        return data_[index];
    }

    const T& operator[](int index) const {
        if (index < 0 || index >= size_) {
            throw std::out_of_range("Vector index out of range");
        }
        return data_[index];
    }
};
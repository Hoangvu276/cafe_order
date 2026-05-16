#pragma once
#include <initializer_list>

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
    // khoi tao vector<T> a(n)
    Vector(int cap) {
        capacity_ = cap;
        size_ = 0;
        data_ = new T[capacity_];
    }
    // khoi tao vector <T> a(n,value)
    Vector(int n, const T& value) {
        size_ = n;
        capacity_ = n;
        data_ = new T[capacity_];

        for (int i = 0; i < size_; i++) {
            data_[i] = value;
        }
    }
    // khoi tao vector <T> a = {...}
    Vector(initializer_list<T> list) {
        size_ = list.size_();
        capacity_ = size_;
        data_ = new T[capacity_];

        int i = 0;
        for (const T& item : list) {
            data_[i] = item;
            i++;
        }
    }
    // gan vector <T> a
    // gan vector <T> b = a (Copy)
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
            // hoat dong voi moi truong hop (memcpy chi hoat dong voi cac kieu du lieu co ban)
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
    	cout << "pop_back on empty Vector"<<"\n";
        return;
    }
    size_--;
}


    int size_(){
    	return size_;
    }

    void print() const {
        for (int i = 0; i < size_; i++) {
            cout << data_[i] << " ";
        }
    }
    bool empty(){
    	return (size_ == 0);
    }

    void resize(int size_,int value_){
    	size_ = n;
        capacity_ = n;
        data_ = new T[capacity_];

        for (int i = 0; i < size_; i++) {
            data_[i] = value;
        }
    }
    void clear() {
    	size_ = 0;
	}

	T* begin() {
    	return data_;
	}
	
	T* end() {
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


};
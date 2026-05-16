template<typename T>
struct Queue {

    vector<T> data;

    void push(const T& item) {
        data.push_back(item);
    }

    void pop() {
        data.erase(data.begin());
    }

    T front() {
        return data[0];
    }
    T rear(){
    	return data[data.size()-1];
    }
};
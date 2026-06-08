#pragma once
#include <stdexcept>
#include <functional>

/**
 * LinkedList<T> - Danh sách liên kết đơn tổng quát theo kiểu T.
 *
 * Cách dùng:
 *   LinkedList<int> list;
 *   list.insertBack(1);
 *   list.insertFront(0);
 *   list.remove(1);
 */

 template <typename T>
struct LinkedList {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& val) : data(val), next(nullptr) {}
    };

    Node* head;
    Node* tail;
    int   sz;
public:
    // Khởi tạo danh sách rỗng
    LinkedList() : head(nullptr), tail(nullptr), sz(0) {}

    // Hủy - giải phóng toàn bộ bộ nhớ
    ~LinkedList() { clear(); }

    // Sao chép
    LinkedList(const LinkedList& o) : head(nullptr), tail(nullptr), sz(0) {
        for (Node* cur = o.head; cur; cur = cur->next)
            insertBack(cur->data);
    }
    LinkedList& operator=(const LinkedList& o) {
        if (this != &o) { clear(); for (Node* cur = o.head; cur; cur = cur->next) insertBack(cur->data); }
        return *this;
    }

    // Trả về số phần tử hiện tại
    int size() const { return sz; }

    // Kiểm tra danh sách có rỗng không
    bool empty() const { return sz == 0; }

    // Chèn phần tử vào đầu danh sách - O(1)
    void insertFront(const T& val) {
        Node* node = new Node(val);
        node->next =head;
        head = node;
        if (!tail) tail = head;
        ++sz;
    }

    // Chèn phần tử vào cuối danh sách - O(1)
    void insertBack(const T& val) {
        Node* node = new Node(val);
        if (!tail){
            head = tail = node;
        }
        else{
            tail->next = node; tail = node;
        }
        ++sz;
    }

    // Chèn phần tử tại vị trí index (0-based) - O(n)
    void insertAt(int index, const T& val) {
        if (index < 0 || index > sz) 
            throw std::out_of_range("insertAt: index out of range");
        if (index == 0)  {insertFront(val); return;}
        if (index == sz) {insertBack(val);  return;}
        Node* prev = head;
        for (int i = 0; i < index - 1; ++i) prev = prev->next;
        Node* node = new Node(val);
        node->next = prev->next;
        prev->next = node;
        ++sz;
    }

    // Xóa phần tử đầu tiên có giá trị bằng val - O(n)
    // Trả về true nếu tìm thấy và xóa, false nếu không có
    bool remove(const T& val) {
        if (!head) return false;
        if (head->data == val) {
            Node* tmp = head;
            head = head->next;
            if (!head) tail = nullptr;
            delete tmp; --sz; return true;
        }
        for (Node* cur = head; cur->next; cur = cur->next) {
            if (cur->next->data == val) {
                Node* tmp = cur->next;
                cur->next = tmp->next;
                if (!cur->next) tail = cur;
                delete tmp; --sz; return true;
            }
        }
        return false;
    }

    // Xóa phần tử tại vị trí index (0-based) - O(n)
    // Ném std::out_of_range nếu index ngoài phạm vi
    void removeAt(int index) {
        if (index < 0 || index >= sz) throw std::out_of_range("removeAt: index out of range");
        if (index == 0) {
            Node* tmp = head;
            head = head->next;
            if (!head) tail = nullptr;
            delete tmp; --sz; return;
        }
        Node* prev = head;
        for (int i = 0; i < index - 1; ++i) prev = prev->next;
        Node* tmp = prev->next;
        prev->next = tmp->next;
        if (!prev->next) tail = prev;
        delete tmp; --sz;
    }

    // Tìm phần tử đầu tiên có giá trị val, trả về index (0-based) - O(n)
    // Trả về -1 nếu không tìm thấy
    int find(const T& val) const {
        int idx = 0;
        for (Node* cur = head; cur; cur = cur->next, ++idx)
            if (cur->data == val) return idx;
        return -1;
    }

    // Truy cập phần tử tại vị trí index (0-based) - O(n)
    // Ném std::out_of_range nếu index ngoài phạm vi
    T& at(int index) {
        if (index < 0 || index >= sz) throw std::out_of_range("at: index out of range");
        Node* cur = head;
        for (int i = 0; i < index; ++i) cur = cur->next;
        return cur->data;
    }
    const T& at(int index) const {
        if (index < 0 || index >= sz) throw std::out_of_range("at: index out of range");
        Node* cur = head;
        for (int i = 0; i < index; ++i) cur = cur->next;
        return cur->data;
    }

    // Duyệt từ đầu đến cuối, gọi hàm fn cho mỗi phần tử - O(n)
    void traverseForward(std::function<void(const T&)> fn) const {
        for (Node* cur = head; cur; cur = cur->next) fn(cur->data);
    }

    // Duyệt từ cuối đến đầu (đệ quy), gọi hàm fn cho mỗi phần tử - O(n)
    void traverseBackward(std::function<void(const T&)> fn) const {
        std::function<void(Node*)> rev = [&](Node* cur) {
            if (!cur) return;
            rev(cur->next);
            fn(cur->data);
        };
        rev(head);
    }

    // Xóa toàn bộ phần tử, giải phóng bộ nhớ - O(n)
    void clear() {
        while (head) {
            Node* tmp = head;
            head = head->next;
            delete tmp;
        }
        tail = nullptr;
        sz   = 0;
    }
};
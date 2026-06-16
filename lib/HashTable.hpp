#pragma once
#include "AVL.hpp"
#include <functional>
#include <stdexcept>

/*
    * HashTable<K, V> - Bảng băm với separate chaining dùng AVL<pair<K,V>>.
    * K phải hỗ trợ std::hash<K> và toán tử == , <.
    *
    * Cách dùng:
    *   HashTable<std::string, int> ht;
    *   ht.insert("alice", 100);
    *   int score = ht.find("alice");  // 100
    *   ht.remove("alice");
 */
template <typename K, typename V>
struct HashTable {
private:
    // pair<K,V> có thứ tự theo K để AVL so sánh
    struct KVPair {
        K key;
        mutable V value; // mutable để có thể cập nhật value trong cây

        KVPair() : key(), value() {}
        KVPair(const K& k, const V& v) : key(k), value(v) {}

        bool operator<(const KVPair& o) const { return key < o.key; }
        bool operator==(const KVPair& o) const { return key == o.key; }
    };

    static const int DEFAULT_BUCKETS = 64;

    AVL<KVPair>* buckets; // mảng các cây AVL
    int numBuckets;
    int sz;

    // Hàm băm: ánh xạ key -> chỉ số bucket
    int bucketIndex(const K& key) const {
        std::hash<K> hasher;
        return static_cast<int>(hasher(key) % static_cast<size_t>(numBuckets));
    }

    // Tìm KVPair trong AVL bằng cách duyệt inorder (vì AVL không có getter)
    // Trả về con trỏ tới value, hoặc nullptr nếu không có
    // Dùng kỹ thuật: tìm kiếm trong BST qua inorder + flag
    bool findInBucket(int idx, const K& key, V& outVal) const {
        bool found = false;
        buckets[idx].inorder([&](const KVPair& kv) {
            if (!found && kv.key == key) {
                outVal = kv.value;
                found  = true;
            }
        });
        return found;
    }

    bool containsInBucket(int idx, const K& key) const {
        bool found = false;
        buckets[idx].inorder([&](const KVPair& kv) {
            if (!found && kv.key == key) found = true;
        });
        return found;
    }
public:
    // Khởi tạo bảng băm với số bucket cho trước (mặc định 64
    explicit HashTable(int numB = DEFAULT_BUCKETS)
        : buckets(new AVL<KVPair>[numB]), numBuckets(numB), sz(0) {}
    ~HashTable() { delete[] buckets; }

    HashTable(const HashTable&)            = delete;
    HashTable& operator=(const HashTable&) = delete;

    // Chèn hoặc cập nhật cặp (key, value) - O(log n/B) trung bình
    // Nếu key đã tồn tại, giá trị cũ bị thay thế
    void insert(const K& key, const V& val) {
        int idx = bucketIndex(key);
        if (containsInBucket(idx, key)) {
            // Xóa cũ rồi chèn mới để cập nhật value
            buckets[idx].remove(KVPair(key, val));
            --sz;
        }
        buckets[idx].insert(KVPair(key, val));
        ++sz;
    }

    // Xóa cặp có key khỏi bảng băm - O(log n/B) trung bình
    // Ném std::out_of_range nếu key không tồn tại
    void remove(const K& key) {
        int idx = bucketIndex(key);
        if (!containsInBucket(idx, key))
            throw std::out_of_range("HashTable::remove: key not found");
        buckets[idx].remove(KVPair(key, V{}));
        --sz;
    }

    // Tìm và trả về value ứng với key - O(log n/B) trung bình
    // Ném std::out_of_range nếu key không tồn tại
    V find(const K& key) const {
        int idx = bucketIndex(key);
        V   val;
        if (!findInBucket(idx, key, val))
            throw std::out_of_range("HashTable::find: key not found");
        return val;
    }

    // Kiểm tra key có tồn tại trong bảng không - O(log n/B) trung bình
    bool contains(const K& key) const {
        return containsInBucket(bucketIndex(key), key);
    }

    // Trả về số cặp (key, value) hiện tại
    int size() const { return sz; }

    // Kiểm tra bảng có rỗng không
    bool empty() const { return sz == 0; }

    // Xóa toàn bộ phần tử
    void clear() {
        for (int i = 0; i < numBuckets; ++i) buckets[i].clear();
        sz = 0;
    }

    // Duyệt toàn bộ cặp (key, value) theo thứ tự bucket
    // Gọi fn(key, value) cho mỗi cặp
    void forEach(std::function<void(const K&, const V&)> fn) const {
        for (int i = 0; i < numBuckets; ++i)
            buckets[i].inorder([&](const KVPair& kv) { fn(kv.key, kv.value); });
    }
};

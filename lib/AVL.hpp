#pragma once
#include "BST.hpp"

/**
 * AVL<T> - Cây AVL tự cân bằng, kế thừa từ BST<T>.
 * Đảm bảo độ cao O(log n) bằng cách tái cân bằng sau mỗi
 * thao tác insert/remove thông qua 4 kiểu xoay (LL, RR, LR, RL).
 * Cách dùng:
 *   AVL<int> tree;
 *   tree.insert(30); tree.insert(20); tree.insert(10); // tự cân bằng
 *   tree.inorder([](const int& v){ std::cout << v << " "; });
 **/
template <typename T>
struct AVL : public BST<T> {
protected:
    using Node = typename BST<T>::Node;
    using BST<T>::root;
    using BST<T>::nodeHeight;
    using BST<T>::updateHeight;

    int balanceFactor(Node* n) const {
        return n ? nodeHeight(n->left) - nodeHeight(n->right) : 0;
    }

    // Xoay phải quanh nút y
    Node* rotateRight(Node* y) {
        Node* x  = y->left;
        Node* T2 = x->right;
        x->right = y;
        y->left  = T2;
        updateHeight(y);
        updateHeight(x);
        return x;
    }

    // Xoay trái quanh nút x
    Node* rotateLeft(Node* x) {
        Node* y  = x->right;
        Node* T2 = y->left;
        y->left  = x;
        x->right = T2;
        updateHeight(x);
        updateHeight(y);
        return y;
    }

    // Cân bằng lại nút n sau insert/remove
    Node* rebalance(Node* n) {
        updateHeight(n);
        int bf = balanceFactor(n);

        // LL: lệch trái - trái -> xoay phải
        if (bf > 1 && balanceFactor(n->left) >= 0)
            return rotateRight(n);

        // LR: lệch trái - phải -> xoay trái con trái, rồi xoay phải
        if (bf > 1 && balanceFactor(n->left) < 0) {
            n->left = rotateLeft(n->left);
            return rotateRight(n);
        }
        // RR: lệch phải - phải -> xoay trái
        if (bf < -1 && balanceFactor(n->right) <= 0)
            return rotateLeft(n);
        // RL: lệch phải - trái -> xoay phải con phải, rồi xoay trái
        if (bf < -1 && balanceFactor(n->right) > 0) {
            n->right = rotateRight(n->right);
            return rotateLeft(n);
        }
        return n; // đã cân bằng
    }

    Node* avlInsert(Node* node, const T& val) {
        if (!node) return new Node(val);
        if (val < node->data)      node->left  = avlInsert(node->left,  val);
        else if (node->data < val) node->right = avlInsert(node->right, val);
        else return node; // trùng lặp, bỏ qua
        return rebalance(node);
    }

    Node* avlRemove(Node* node, const T& val) {
        if (!node) return nullptr;
        if (val < node->data)      node->left  = avlRemove(node->left,  val);
        else if (node->data < val) node->right = avlRemove(node->right, val);
        else {
            if (!node->left || !node->right) {
                Node* temp = node->left ? node->left : node->right;
                delete node;
                return temp;
            }
            Node* successor = BST<T>::findMin(node->right);
            node->data = successor->data;
            node->right = avlRemove(node->right, successor->data);
        }
        return rebalance(node);
    }

public:
    AVL() : BST<T>() {}
    void insert(const T& val) {
        root = avlInsert(root, val);
    }
    void remove(const T& val) {
        root = avlRemove(root, val);
    }
};

#pragma once
#include <functional>
#include <stdexcept>

/**
 * BST<T> - Cây tìm kiếm nhị phân tổng quát theo kiểu T.
 * T phải hỗ trợ toán tử < và ==.
 *
 * Cách dùng:
 *   BST<int> tree;
 *   tree.insert(10); tree.insert(5); tree.insert(20);
 *   tree.inorder([](const int& v){ std::cout << v << " "; });
 */
template <typename T>
struct BST {
protected:
    struct Node{
        T data;
        Node* left;
        Node* right;
        int height; 
        explicit Node(const T& val) : data(val), left(nullptr), right(nullptr), height(1) {}
    };

    Node* root;

    int nodeHeight(Node* n) const{
        return n ? n->height : 0;
    }

    void updateHeight(Node* n){
        if(n) n->height = 1 + std::max(nodeHeight(n->left), nodeHeight(n->right));
    }

    Node* insertNode(Node* node, const T& val){
        if(!node) return new Node(val);
        if(val < node->data) node->left = insertNode(node->left, val);
        else if(node->data < val) node->right = insertNode(node->right, val);
        updateHeight(node);
        return node;
    }

    Node* findMin(Node* node) const{
        while(node->left) node = node->left;
        return node;
    }

    Node* removeNode(Node* node, const T& val){
        if(!node) return nullptr;
        if(val < node->data) node->left = removeNode(node->left, val);
        else if(node->data < val) node->right = removeNode(node->right, val);
        else {
            if(!node->left){
                Node* r = node->right; delete node; return r;
            }
            if(!node->right){
                Node* l = node->left;  delete node; return l;
            }
            Node* successor = findMin(node->right);
            node->data = successor->data;
            node->right = removeNode(node->right, successor->data);
        }
        updateHeight(node);
        return node;
    }

    bool searchNode(Node* node, const T& val) const{
        if(!node) return false;
        if(val < node->data) return searchNode(node->left, val);
        else if(node->data < val) return searchNode(node->right, val);
        else return true;
    }

    void preorderNode(Node* node, std::function<void(const T&)> fn) const{
        if(!node) return;
        fn(node->data);
        preorderNode(node->left, fn);
        preorderNode(node->right, fn);
    }

    void inorderNode(Node* node, std::function<void(const T&)> fn) const{
        if(!node) return;
        inorderNode(node->left, fn);
        fn(node->data);
        inorderNode(node->right, fn);
    }
    
    void postorderNode(Node* node, std::function<void(const T&)> fn) const{
        if(!node) return;
        postorderNode(node->left, fn);
        postorderNode(node->right, fn);
        fn(node->data);
    }

    void clearNode(Node* node){
        if(!node) return;
        clearNode(node->left);
        clearNode(node->right);
        delete node;
    }
    
    Node* copyNode(Node* node){
        if(!node) return nullptr;
        Node* newNode = new Node(node->data);
        newNode->left = copyNode(node->left);
        newNode->right = copyNode(node->right);
        return newNode;
    }
public:
    BST() : root(nullptr) {}
    ~BST() { clearNode(root); }
    BST(const BST& o) : root(copyNode(o.root)) {}
    BST& operator=(const BST& o){
        if(this != &o){
            clearNode(root);
            root = copyNode(o.root);
        }
        return *this;
    }

    void insert(const T& val) {
        root = insertNode(root, val);
    }

    void remove(const T& val) {
        root = removeNode(root, val);
    }

    bool search(const T& val) const {
        return searchNode(root, val);
    }

    void preorder(std::function<void(const T&)> fn) const {
        preorderNode(root, fn);
    }

    void inorder(std::function<void(const T&)> fn) const {
        inorderNode(root, fn);
    }

    void postorder(std::function<void(const T&)> fn) const {
        postorderNode(root, fn);
    }

    const T* findMin() const {
        if (!root) return nullptr;
        return &findMin(root)->data;
    }

    bool empty() const {
        return root == nullptr;
    }

    void clear() {
        clearNode(root);
        root = nullptr;
    }
};

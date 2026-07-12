#pragma once

template<typename T>
class Node {
public:
    T* data;
    Node<T>* prev;
    Node<T>* next;

    explicit Node(T* data) : data(data), prev(nullptr), next(nullptr) {}

    Node<T>* getPrev() { return prev; }
    Node<T>* getNext() { return next; }
};

template<typename T>
class DLL {
private:
    Node<T>* head;
    Node<T>* tail;
    int size;

public:
    DLL() : head(nullptr), tail(nullptr), size(0) {}

    void insertAtTail(T* data = nullptr) {
        if (data == nullptr) {
            throw std::invalid_argument("Data cannot be nullptr");
        }
        Node<T>* newNode = new Node<T>(data);
        if (head == nullptr) {
            head = newNode;
            tail = newNode;
            size = 1;
            return;
        }
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
        size++;
    }

    void removeNode(Node<T>* node) {
        if (node == nullptr) {
            throw std::invalid_argument("Data cannot be nullptr");
        }

        if (head == tail) {
            head = nullptr;
            tail = nullptr;
        } else if (node == head) {
            head = head->next;
            head->prev = nullptr;
        } else if (node == tail) {
            tail = tail->prev;
            tail->next = nullptr;
        } else {
            node->prev->next = node->next;
            node->next->prev = node->prev;
        }
        size--;
        delete node;
    }
    DLL(const DLL&) = delete;
    DLL& operator=(const DLL&) = delete;
    ~DLL() {
        while (head != nullptr) {
            Node<T>* temp = head;
            head = head->next;
            delete temp;
        }
    }

    int getSize() const { return size; }
    Node<T>* getHead() const { return head; }
    bool isEmpty() const { return size == 0; }
    Node<T>* getTail() const { return tail; }
};

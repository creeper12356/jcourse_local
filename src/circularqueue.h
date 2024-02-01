#ifndef CIRCULARQUEUE_H
#define CIRCULARQUEUE_H
#include <cassert>
#include <iostream>

//环形队列模板类，用于缓存类数据结构
template <class Type>
class CircularQueue
{
public:
    CircularQueue(int maxSize) {
        assert(maxSize > 0);
        capacity = maxSize + 1;
        data = new Type[capacity];
        front = back = 0;
    }
    ~CircularQueue() {
        delete [] data;
    }
public:
    class Iterator {
    public:
        Iterator(CircularQueue* arg_queue , int arg_index) : queue(arg_queue) , index(arg_index) {
        }
        Iterator& operator++() {
            index = (index + 1) % queue->capacity;
            return *this;
        }
        Type& operator*() {
            return queue->data[index];
        }
        bool operator==(const Iterator& other) const {
            assert(this->queue == other.queue);
            return this->index == other.index;
        }
        bool operator!=(const Iterator& other) const {
            assert(this->queue == other.queue);
            return this->index != other.index;
        }

    private:
        CircularQueue* queue;
        int index;
    };

    class ConstIterator {
    public:
        ConstIterator(const CircularQueue* arg_queue , int arg_index) : queue(arg_queue) , index(arg_index) {
        }
        ConstIterator& operator++() {
            index = (index + 1) % queue->capacity;
            return *this;
        }
        const Type& operator*() {
            return queue->data[index];
        }
        bool operator==(const ConstIterator& other) const {
            assert(this->queue == other.queue);
            return this->index == other.index;
        }
        bool operator!=(const ConstIterator& other) const {
            assert(this->queue == other.queue);
            return this->index != other.index;
        }
    private:
        const CircularQueue* queue;
        int index;
    };

    void push(const Type& newOne) {
        data[back] = newOne;
        back = (back + 1) % capacity;
        if(back == front){
            //队列已满
            front = (front + 1) % capacity;
        }
    }
    Type pop() {
        Type temp = data[front];
        front = (front + 1) % capacity;
        return temp;
    }
    int size() const {
        return (back - front + capacity) % capacity;
    }
    Iterator begin() {
        return Iterator(this,front);
    }
    ConstIterator begin() const {
        return ConstIterator(this,front);
    }
    Iterator end() {
        return Iterator(this,back);
    }
    ConstIterator end() const{
        return ConstIterator(this,back);
    }

    //for debug
    void printQueue() const {
        for(int i = front;i != back;i = (i + 1) % capacity) {
            std::cout << data[i] << " ";
        }
        std::cout << std::endl;
    }

private:
    Type *data;
    int capacity;
    int front;
    int back;
};

#endif // CIRCULARQUEUE_H

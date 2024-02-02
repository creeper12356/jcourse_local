#ifndef CIRCULARQUEUE_H
#define CIRCULARQUEUE_H
#include <cassert>
#include <iostream>

template <class Type>
/*!
 * \brief 可迭代的环形队列模板
 */
class CircularQueue
{
public:
    CircularQueue(int maxSize) {
        assert(maxSize > 0);
        capacity = maxSize + 1;
        data = new Type[capacity];
        front = back = 0;
    }
    CircularQueue(const CircularQueue& other) {
        capacity = other.capacity;
        data = new Type[capacity];
        front = other.front;
        back = other.back;
        for(int i = front ;i != back ;i = (i + 1) % capacity) {
            data[i] = other.data[i];
        }
    }
    ~CircularQueue() {
        delete [] data;
    }

public:
    /*!
     * \brief 容器的迭代器
     */
    class Iterator {
    public:
        Iterator(CircularQueue* arg_queue , int arg_index) : queue(arg_queue) , index(arg_index) {}
        Iterator& operator++() {
            index = (index + 1) % queue->capacity;
            return *this;
        }
        Iterator& operator--() {
            index = (index + queue->capacity - 1) % queue->capacity;
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

    /*!
     * \brief 容器的常量迭代器
     */
    class ConstIterator {
    public:
        ConstIterator(const CircularQueue* arg_queue , int arg_index) : queue(arg_queue) , index(arg_index) {}
        ConstIterator(const Iterator& it) : queue(it.queue) , index(it.index) {}

        ConstIterator& operator++() {
            index = (index + 1) % queue->capacity;
            return *this;
        }
        ConstIterator& operator--() {
            index = (index + queue->capacity - 1) % queue->capacity;
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
        //保证队列不为空
        assert(front != back);
        Type temp = data[front];
        front = (front + 1) % capacity;
        return temp;
    }
    bool empty() const {
        return front == back;
    }
    int size() const {
        return (back - front + capacity) % capacity;
    }
    CircularQueue& operator=(const CircularQueue& other) {
        if(this == &other) {
            return *this;
        }
        delete [] data;
        capacity = other.capacity;
        front = other.front;
        back = other.back;
        data = new Type[capacity];
        for(int i = front ; i != back ; i = (i + 1) % capacity) {
            data[i] = other.data[i];
        }
        return *this;
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

protected:
    Type *data;
    int capacity;
    int front;
    int back;
};

#endif // CIRCULARQUEUE_H

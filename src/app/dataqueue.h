#ifndef DATAQUEUE_H
#define DATAQUEUE_H

#endif // DATAQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

using namespace std;

template <class T>
class DataQueue
{
private:
    queue<T> m_q;
    mutex m_mu;
    condition_variable m_cvAdd, m_cvGet;
    size_t m_maxEl {1000};

public:
    DataQueue(size_t items): m_maxEl(items) {}
    DataQueue(void) {}

    void add(T item)
    {
        unique_lock<mutex> lock(m_mu);
        m_cvAdd.wait(lock,[this]{return !isFull();});
        m_q.push(item);
        m_cvGet.notify_one();
    }

    T get()
    {
        unique_lock<mutex> lock(m_mu);
        m_cvGet.wait(lock,[this]{return !isEmpty();});
        T tmp =  m_q.front();
        m_q.pop();
        m_cvAdd.notify_one();
        return tmp;
    }

    bool isFull()
    {
        //unique_lock<mutex> lock(m_mu);
        return (m_q.size() >= m_maxEl);
    }

    bool isEmpty()
    {
        //unique_lock<mutex> lock(m_mu);
        return m_q.empty();
    }

    bool extIsEmpty()
    {
        unique_lock<mutex> lock(m_mu);
        return m_q.empty();
    }

};

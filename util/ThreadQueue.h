/* 
 * Moudule: util
 * Author: cloudyang@2015/04/17
 */

#ifndef COBRA_UTIL_THREADQUEUE
#define COBRA_UTIL_THREADQUEUE

#include <deque>
#include <vector>
#include "base/WaitableEvent.h"

namespace CobraUtil {

template<typename T, typename Q = std::deque<T> >
class ThreadQueue {
public:
    typedef Q QueueType;

    ThreadQueue() : m_event(), m_size(0) { }
    virtual ~ThreadQueue() { clear(); notify(); }

    bool pop_front(T& data, int ms = 0);
    void push_back(const T& data);
    void push_back(QueueType& q);

    void push_front(const T& data);
    void push_front(QueueType& q);
    
    bool swap(QueueType& q, int ms = 0);
    void notify();
    size_t size() const;
    void clear();
    bool empty() const;

private:
    QueueType m_queue;
    base::WaitableEvent m_event;
    size_t m_size;
};

template<typename T, typename Q>
bool ThreadQueue<T, Q>::pop_front(T& data, int ms)
{
    base::AutoLock lock(const_cast<base::Lock&>(m_event.getLock()));

    if (m_queue.empty()) {
        if (ms == 0)
            return false;
        if (ms == -1)
            m_event.waitWithoutLock();
        else {
            if (!m_event.timedWaitWithoutLock(ms))
                return false;
        }
    }

    if (m_queue.empty())
        return false; 
    data = m_queue.front();
    m_queue.pop_front();
    --m_size;
    return true;
}

template<typename T, typename Q>
void ThreadQueue<T, Q>::push_back(const T& data) 
{
    base::AutoLock lock(const_cast<base::Lock&>(m_event.getLock()));

    m_event.notifyWithoutLock();
    m_queue.push_back(data);
    ++m_size;
}

template<typename T, typename Q>
void ThreadQueue<T, Q>::push_back(Q& q) 
{
    base::AutoLock lock(const_cast<base::Lock&>(m_event.getLock()));

    typename Q::iterator it = q.begin();
    while (it != q.end()) {
        m_queue.push_back(*it);
        ++it;
        ++m_size;
        m_event.notifyWithoutLock();
    }
}

template<typename T, typename Q>
void ThreadQueue<T, Q>::push_front(const T& data) 
{
    base::AutoLock lock(const_cast<base::Lock&>(m_event.getLock()));
    m_event.notifyWithoutLock();
    m_queue.push_front(data);
    ++m_size;
}

template<typename T, typename Q>
void ThreadQueue<T, Q>::push_front(Q& q) 
{
    base::AutoLock lock(const_cast<base::Lock&>(m_event.getLock()));
    typename Q::iterator it = q.begin();
    while (it != q.end()) {
        m_queue.push_front(*it);
        ++it;
        ++m_size;
        m_event.notifyWithoutLock();
    }
}

template<typename T, typename Q>
bool ThreadQueue<T, Q>::swap(Q& q, int ms) 
{
    base::AutoLock lock(const_cast<base::Lock&>(m_event.getLock()));
    
    if (m_queue.empty()) {
        if (ms == 0)
            return false;

        if (ms == -1)
            m_event.waitWithoutLock();
        else {
            if (!m_event.timedWaitWithoutLock(ms))
                return false;
        }
    }

    if (m_queue.empty())
        return false;

    q.swap(m_queue);
    m_size = q.size();
    return true;
}

template<typename T, typename Q>
void ThreadQueue<T, Q>::notify() 
{
    base::AutoLock lock(const_cast<base::Lock&>(m_event.getLock()));
    m_event.notifyAllWithoutLock();
}

template<typename T, typename Q>
size_t ThreadQueue<T, Q>::size() const 
{
    base::AutoLock lock(const_cast<base::Lock&>(m_event.getLock()));
    return m_size;
}

template<typename T, typename Q>
void ThreadQueue<T, Q>::clear()
{
    base::AutoLock lock(const_cast<base::Lock&>(m_event.getLock()));
    m_queue.clear();
    m_size = 0;
}

template<typename T, typename Q>
bool ThreadQueue<T, Q>::empty() const
{
    base::AutoLock lock(const_cast<base::Lock&>(m_event.getLock()));
    return m_queue.empty();
}

}

#endif

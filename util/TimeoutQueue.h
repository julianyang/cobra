/* 
 * Moudule: util
 * Author: cloudyang@2015/04/17
 */

#ifndef COBRA_UTIL_TIMEOUTQUEUE
#define COBRA_UTIL_TIMEOUTQUEUE

#include <deque>
#include <map>
#include <list>
#include "base/Lock.h"
#include "base/RefCountedThreadSafe.h"

namespace CobraUtil {

template<typename T>
class TimeoutQueue : public base::RefCountedThreadSafe<TimeoutQueue<T> > {
public:
    typedef T ElementType;
    typedef void (*TimeoutHandler)(ElementType& data);

protected:
    friend class base::RefCountedThreadSafe<TimeoutQueue<T> >;
    struct DataElement;
    struct TimeElement;

    typedef std::map<unsigned int, DataElement> DataList;
    typedef std::list<TimeElement> TimeList;

    struct DataElement {
        ElementType m_data;
        TimeList::iterator m_timeIter;
    };

    struct TimeElement {
        bool m_poped;
        long long m_createTime;
        DataList::iterator m_dataIter;
    };

public:
    TimeoutQueue(int timeout = 5000)
        : m_lock()
        , m_timeout(timeout)
        , m_uniqId(0)
    {
        m_firstNotPopIter = m_timeList.end();
    }

    virtual ~TimeoutQueue()
    {
        m_dataList.clear();
        m_timeList.clear();
    }

    unsigned int generateId();
    ElementType get(unsigned int id, bool erase = true);
    ElementType erase(unsigned int id);

    bool push(ElementType& data, unsigned int id);
    void timeout(TimeoutHandler handler = NULL);

    bool pop(ElementType& data);
    bool swap(std::deque<T>& q);

    void setTimeout(int timeout) { AutoLock lock(m_lock); m_timeout = timeout; }

    size_t size() const { AutoLock lock(m_lock); return m_dataList.size(); }

private:
    base::Lock m_lock;
    int m_timeout;
    unsigned int m_uniqId;

    DataList m_dataList;
    TimeList m_timeList;

    TimeList::iterator m_firstNotPopIter;
};

template<typename T>
unsigned int TimeoutQueue<T>::generateId()
{
    AutoLock lock(m_lock);
    return ++m_uniqId;
}

template<typename T>
T TimeoutQueue<T>::get(unsigned int id, bool erase)
{
    AutoLock lock(m_lock);

    DataList::iterator it = m_dataList.find(id);
    if (it == m_dataList.end())
        return NULL;

    T tmp = it->second.m_data;
    if (erase) {
        if (m_firstNotPopIter == it->second.m_timeIter)
            ++m_firstNotPopIter;

        m_timeList.erase(it->second.m_timeIter);
        m_dataList.erase(it);
    }

    return tmp;
}

template<typename T>
T TimeoutQueue<T>::erase(unsigned int id)
{
    AutoLock lock(m_lock);

    DataList::iterator it = m_dataList.find(id);
    if (it == m_dataList.end())
        return NULL;

    T tmp = it->second.m_data;
    if (m_firstNotPopIter == it->second.m_timeIter)
        ++m_firstNotPopIter;

    m_timeList.erase(it->second.m_timeIter);
    m_dataList.erase(it);

    return tmp;
}

template<typename T>
bool TimeoutQueue<T>::push(T& data, unsigned int id)
{
    AutoLock lock(m_lock);
    
    DataElement dataInfo;
    dataInfo.m_data = data;

    std::pair<DataList::iterator, bool> result;
    result = m_dataList.insert(std::make_pair(id, dataInfo));
    if (result.second == false) 
        return false;

    TimeElement timeInfo;
    timeinfo.m_poped = false;
    timeInfo.m_createTime = base::Time::now().toInternalValue();
    timeInfo.m_dataIter = result.first;

    m_timeList.push_back(timeInfo);

    TimeList::iterator tmpIter = m_timeList.end();
    result.first.second.m_timeIter = --tmpIter;

    if (m_firstNotPopIter == TimeList.endl())
        m_firstNotPopIter = tmpIter;
    
    return true;
}

template<typename T>
void TimeoutQueue<T>::timeout(TimeoutHandler handler) 
{
    long long now = base::Time::now().toInternalValue();

    while (true) {
        T data;
        {
            AutoLock lock(m_lock);

            TimeList::iterator timeIter = m_timeList.begin();
            if (it != m_timeList.end() && (it->m_createTime + m_timeout) < now) {
                data = *(it->m_dataIter).second.m_data;
                m_dataList.erase(it->m_dataIter);
                if (m_firstNotPopIter == it)
                    ++m_firstNotPopIter;
                m_timeList.erase(it);
            } else
                break;
        }

        if (handler) {
            try {
                handler(data);
            } catch (...) { }
        }
    }
}


template<typename T>
bool TimeoutQueue<T>::pop(T& data)
{
    AutoLock lock(m_lock);

    if (m_timeList.empty())
        return false;

    TimeList::iterator timeIter = m_timeList.begin();
    if (timeIter->m_poped)
        timeIter = m_firstNotPopIter;

    if (timeIter == m_timeList.end())
        return false;

    data = *(timeIter->m_dataIter).second.m_data;
    timeIter->m_poped = true;

    m_firstNotPopIter = timeIter;
    ++m_firstNotPopIter;
    
    return true;
}

template<typename T>
bool TimeoutQueue<T>::swap(std::deque<T>& q)
{
    AutoLock lock(m_lock);
    if (m_timeList.empty())
        return false;

    TimeList::iterator timeIter = m_timeList.begin();
    while (timeIter != m_timeList.end()) {
        if (timeIter->m_poped)
            timeIter = m_firstNotPopIter;
        if (timeIter == m_timeList.end())
            break;

        T data = timeIter->m_dataIter->second.m_data;
        timeIter->m_poped = true;
        m_firstNotPopIter = timeIter;
        ++m_firstNotPopIter;
        q.push_back(data);
        ++timeIter;
    }

    return !q.empty();
}

}

#endif

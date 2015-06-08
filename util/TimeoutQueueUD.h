/* 
 * Moudule: util
 * Author: cloudyang@2015/04/17
 */

#ifndef COBRA_UTIL_TIMEOUTQUEUEUD
#define COBRA_UTIL_TIMEOUTQUEUEUD

#include <map>
#include <list>
#include "base/Lock.h"
#include "base/RefCountedThreadSafe.h"
#include "base/Time.h"

namespace CobraUtil {

template<typename T>
class TimeoutQueueUD : public base::RefCountedThreadSafe<TimeoutQueueUD<T> > {
public:
    typedef T ElementType;
    typedef void (*TimeoutHandler)(ElementType& data, void* p);

protected:
    friend class base::RefCountedThreadSafe<TimeoutQueueUD<T> >;
    struct DataElement;

    typedef std::map<unsigned int, DataElement> DataList;
    typedef std::multimap<long long, typename DataList::iterator>  TimeList;

    struct DataElement {
        ElementType m_data;
        typename TimeList::iterator m_timeIter;
    };

public:
    TimeoutQueueUD()
        : m_lock()
        , m_uniqId(0)
    {

    }

    virtual ~TimeoutQueueUD()
    {
        m_dataList.clear();
        m_timeList.clear();
    }

    unsigned int generateId();
    ElementType get(unsigned int id, bool erase = true);
    ElementType erase(unsigned int id);

    bool push(ElementType& data, unsigned int id, int timeout /*ms*/);
    void timeout(TimeoutHandler handler = NULL, void* p = NULL);

    size_t size() const { base::AutoLock lock(m_lock); return m_dataList.size(); }

private:
    base::Lock m_lock;
    unsigned int m_uniqId;

    DataList m_dataList;
    TimeList m_timeList;
};

template<typename T>
unsigned int TimeoutQueueUD<T>::generateId()
{
    base::AutoLock lock(m_lock);
    return ++m_uniqId;
}

template<typename T>
T TimeoutQueueUD<T>::get(unsigned int id, bool erase)
{
    base::AutoLock lock(m_lock);

    typename DataList::iterator it = m_dataList.find(id);
    if (it == m_dataList.end())
        return NULL;

    T tmp = it->second.m_data;
    if (erase) {
        m_timeList.erase(it->second.m_timeIter);
        m_dataList.erase(it);
    }

    return tmp;
}

template<typename T>
T TimeoutQueueUD<T>::erase(unsigned int id)
{
    base::AutoLock lock(m_lock);

    typename DataList::iterator it = m_dataList.find(id);
    if (it == m_dataList.end())
        return NULL;

    T tmp = it->second.m_data;

    m_timeList.erase(it->second.m_timeIter);
    m_dataList.erase(it);

    return tmp;
}

template<typename T>
bool TimeoutQueueUD<T>::push(T& data, unsigned int id, int timeout)
{
    base::AutoLock lock(m_lock);
    
    DataElement dataInfo;
    dataInfo.m_data = data;

    std::pair<typename DataList::iterator, bool> result;
    result = m_dataList.insert(std::make_pair(id, dataInfo));
    if (result.second == false) 
        return false;

    int future = base::Time::now().toInternalValue() + timeout * base::Time::kMicrosecondsPerMillisecond;

    typename TimeList::iterator tempIter = m_timeList.insert(std::make_pair(future, result.first));

    (result.first->second).m_timeIter = tempIter;

    return true;
}

template<typename T>
void TimeoutQueueUD<T>::timeout(TimeoutHandler handler, void* p) 
{
    long long now = base::Time::now().toInternalValue();

    while (true) {
        T data;
        {
            base::AutoLock lock(m_lock);

            typename TimeList::iterator it = m_timeList.begin();
            if (it != m_timeList.end() && it->first > now) {
                data = (it->second->second).m_data;
                m_dataList.erase(it->second);
                m_timeList.erase(it);
            } else
                break;
        }

        if (handler) {
            try {
                handler(data, p);
            } catch (...) { }
        }
    }
}


}

#endif

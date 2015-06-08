/* 
 * Moudule: core
 * Author: cloudyang@2015/04/17
 */


#include "core/ConnectionList.h"

#include <assert.h>
#include "base/Time.h"
#include "core/Connection.h"


namespace CobraCore {

#define ID_CHECK(id) { \
    int prix = id & 0xFF000000; \
    assert(prix == m_idPrefix); \
}

ConnectionList::ConnectionList(int index, ConnectionListClient* client, size_t maxConns) 
    : m_idPrefix((index << 24) & (0xFF000000))
    , m_client(client)
    , m_lock()
    , m_connections(0)
    , m_freeSize(0)
    , m_connectionList(new DataType[maxConns+1])
{
    assert(m_client);
    for (size_t i=0; i<maxConns; i++) {
        m_connectionList[i].first = NULL;
        m_freeList.push_back(i);
        ++m_freeSize;
    }
}

ConnectionList::~ConnectionList() 
{
    if (m_connectionList)
        delete m_connectionList;
}

int ConnectionList::add(base::RefPtr<Connection>& conn, int timeout)
{
    base::AutoLock lock(m_lock);
    int future = base::Time::now().toInternalValue()/base::Time::kMicrosecondsPerMillisecond + timeout;

    int id = m_freeList.front();
    m_freeList.pop_front();
    --m_freeSize;

    m_connectionList[id] = std::make_pair(conn, m_timeList.insert(std::make_pair(future, id)));

    return (m_idPrefix | id);
}

void ConnectionList::del(int id)
{
    ID_CHECK(id);
    base::AutoLock lock(m_lock);

    int index = id & 0x00FFFFFF;
    m_timeList.erase(m_connectionList[index].second);
    m_connectionList[index].first = NULL;

    m_freeList.push_back(index);
    ++m_freeSize;
}

void ConnectionList::refresh(int id, int timeout)
{
    ID_CHECK(id);
    base::AutoLock lock(m_lock);
    int future = base::Time::now().toInternalValue()/base::Time::kMicrosecondsPerMillisecond + timeout;

    int index = id & 0x00FFFFFF;
    
    m_timeList.erase(m_connectionList[index].second);
    m_connectionList[index].second = m_timeList.insert(std::make_pair(future, index));
}

base::RefPtr<Connection> ConnectionList::get(int id)
{
    ID_CHECK(id);
    base::AutoLock lock(m_lock);

    int index = id & 0x00FFFFFF;
    return m_connectionList[index].first;
}

void ConnectionList::checkTimeout()
{
    base::AutoLock lock(m_lock);

    int now = base::Time::now().toInternalValue()/base::Time::kMicrosecondsPerMillisecond;
    
    std::multimap<unsigned int, int>::iterator it = m_timeList.begin();
    while (it != m_timeList.end()) {
        if (it->first > static_cast<unsigned int>(now))
            break;

        int index = it->second;
        // CompleteMe: close connection.        
        m_client->timeout(m_connectionList[index].first);
        m_timeList.erase(m_connectionList[index].second);
        m_connectionList[index].first = NULL;
        m_freeList.push_back(index);
        ++m_freeSize;
        it++;
    }
}

}

/* 
 * Moudule: core
 * Author: cloudyang@2015/04/17
 */

#ifndef COBRA_CORE_CONNECTION_LIST
#define COBRA_CORE_CONNECTION_LIST

#include <map>
#include <list> 
#include "base/RefPtr.h"
#include "base/Lock.h"

namespace CobraCore {

class Connection;

class ConnectionList {
public:
    class ConnectionListClient {
    public:
        virtual void timeout(base::RefPtr<Connection>& conn) = 0;

    protected:
        ConnectionListClient() {}
        virtual ~ConnectionListClient() {}
    };

    ConnectionList(int index, ConnectionListClient* client, size_t maxConns = 1000);
    ~ConnectionList();

    int add(base::RefPtr<Connection>& conn, int timeout);
    void del(int id);
    void refresh(int id, int timeout);
    base::RefPtr<Connection> get(int id);
    void checkTimeout();

private:
    typedef std::pair<base::RefPtr<Connection>, std::multimap<unsigned int, int>::iterator> DataType;

    int m_idPrefix;
    ConnectionListClient* m_client;
    base::Lock m_lock;
    volatile size_t m_connections; // number of connections.
    std::list<int> m_freeList; // free list.
    volatile size_t m_freeSize; // free list size.
    DataType* m_connectionList; // connection list.

    std::multimap<unsigned int, int> m_timeList; // time info list.
};

}

#endif
